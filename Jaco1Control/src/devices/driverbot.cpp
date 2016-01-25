/*
 * driverbot.cpp
 *
 *  Created on: Jun 9, 2015
 *      Author: vale
 */
#include "driverbot.hpp"
#include "../Interface/VREP/extApi.hpp"
#include "../Interface/VREP/extApiCustom.hpp"


driverbot::driverbot(bool _sync,std::string joint_base_name,model * _bot)
: running(true)
, running_cleaner(true)
, first_write(false){
	std::string connectionAddress= "127.0.0.1";
	simxInt connectionPort = 19998;
	simxInt timeOutInMs = 2000;
	simxInt commThreadCycleInMs = 5;
	//  if different from zero, then the function blocks until connected (or timed out).
	simxUChar waitUntilConnected = 'a';
	// if different from zero, then the communication thread will not attempt a second connection if a connection was lost.
	simxUChar doNotReconnectOnceDisconnected = 'a';
	//DEBUG
	std::cout <<idclient << std::endl;
	//---
	//open connection
 	idclient = simxStart(connectionAddress.c_str(),connectionPort,waitUntilConnected,doNotReconnectOnceDisconnected,timeOutInMs,commThreadCycleInMs);
 	if(idclient == -1)
 		std::cout<<"connection error with vrep simulator"<<std::endl;
 	//DEBUG
 	std::cout <<idclient << std::endl;
 	//---
 	sync = _sync;
 	if(sync){
 		simxUChar en='e'; // to check
 		int syncres = simxSynchronous(this->idclient,en);
 		//DEBUG
 		std::cout<< "syncres = " << syncres <<std::endl;
 		//---
 	}

 	// read joint handle
 	bool more_joint = true;
 	simxInt handle[1];
 	NJoints =0;
 	while(more_joint){
 		std::string current_joint_name(joint_base_name);
 		std::string s = SSTR(NJoints + 1);
 		s = current_joint_name + s;
 		//DEBUG
 		std::cout<< s << std::endl;
 		//---
 		int res = simxGetObjectHandle(this->idclient,s.c_str(),handle,simx_opmode_oneshot_wait);
        // DEBUG
 		std::cout<<handle[0]<<std::endl;
        //---
 		if(res == 0)
 			joint_handle.push_back(handle[0]);
 		else
 			more_joint = false;
 		NJoints++;
 	}
 	NJoints = NJoints - 1;
 	//DEBUG
 	std::cout<<joint_handle.size()<<std::endl;
 	for(unsigned int ij =0;ij<joint_handle.size();ij++){
 		std::cout<< joint_handle[ij]<< " ";
 	}
 	std::cout<<std::endl;
 	//---
 	bot = _bot;
 	this->tStart = new simxFloat[1];
 	this->tStart[0] = 0;
 	already_saving = false;
 	reader_stats = NULL;
 	garbage_collection = NULL;
 	Max_DS_allowed = 10000;
}

driverbot::~driverbot(){
	delete tStart;
	// close simulator
	simxStopSimulation(this->idclient,simx_opmode_oneshot_wait);
	simxFinish(this->idclient);
}

std::vector<State> driverbot::FirstRead(std::vector<std::string> type){
	std::vector<State> res;
    boost::this_thread::sleep(boost::posix_time::milliseconds(10)); // this sleep is necessary because at the begining i read a lot of nasty value
	driverbot::GetLastValue(res,type);
    // DEBUG
	//std::cout<<" FirstRead result  "<<result<<std::endl;
	//---
	return res;
}

bool driverbot::GetLastValue(std::vector<State>& res, std::vector<std::string> & type){
	if(first_write.load(boost::memory_order_acquire)){
		//DEBUG
		//State * ptr = NULL;
		//std::cout<<"type.size() "<<type.size()<<std::endl;
		//---
		for(unsigned int i =0;i<type.size();i++){
			State app;
			//DEBUG
			//std::cout<<"type[i]"<<type[i]<<std::endl;
			//----
			if(type[i].compare("j_pos") == 0){
				app = *(this->dl_ang_pos.load(boost::memory_order_acquire));
				//DEBUG
				//ptr =this->dl_ang_pos.load(boost::memory_order_acquire);
				//std::cout<< "address try inside"<<this->dl_ang_pos.load(boost::memory_order_acquire)<<std::endl;
				//
			}else if(type[i].compare("j_vel") == 0){
				app = *(this->dl_ang_vel.load(boost::memory_order_acquire));
			}else if(type[i].compare("j_tau") == 0){
			   app = *(this->dl_ang_tau.load(boost::memory_order_acquire));
			}else if(type[i].compare("cart_pos") == 0){
				app = *(this->dl_cart.load(boost::memory_order_acquire));
			}
			res.push_back(app);
		}
		//DEBUG
		//std::cout<< "get last value joint position"<<std::endl;
		//std::cout<< "address try "<<this->dl_ang_pos.load(boost::memory_order_acquire)<<std::endl;
		//std::cout<<"address last value "<<ptr<<std::endl;
		//std::cout<<"&(ds_ang_pos.back()) "<<&(ds_ang_pos.back())<<std::endl;
		//std::cout<< "res[0].size() "<<res[0].size()<<std::endl;
		//for(unsigned int ik =0;ik<res[0].size();ik++)
		//		std::cout<<res[0][ik]<<" ";
		//std::cout<<std::endl;
		//----
		return true;
	}
	return false;
}

void driverbot::Start(){
	this->running.store(true,boost::memory_order_release);
	this->running_cleaner.store(true,boost::memory_order_release);
	this->reader_stats = new boost::thread(boost::bind(&driverbot::Reading,this));
	this->garbage_collection = new boost::thread(boost::bind(&driverbot::Cleaning,this));
	simxStartSimulation(this->idclient,simx_opmode_oneshot);
}

void driverbot::Stop(){
	this->running.store(false,boost::memory_order_release);
	this->running_cleaner.store(false,boost::memory_order_release);
	this->reader_stats->join();
	this->garbage_collection->join();
	std::cout<<"close all thread"<<std::endl;
}
void driverbot::Reading(){
	while(this->running.load(boost::memory_order_acquire)){
		if(first_write.load(boost::memory_order_acquire)){
			this->ReadTimeStamp(simx_opmode_oneshot);
			State pos=this->ReadJoints(simx_opmode_oneshot);
			this->ReadCartesian(pos,simx_opmode_oneshot);
		}else{
			this->ReadTimeStamp(simx_opmode_oneshot_wait);
			State pos=this->ReadJoints(simx_opmode_oneshot_wait);
			this->ReadCartesian(pos,simx_opmode_oneshot_wait);
			first_write.store(true,boost::memory_order_release);
		}
		//boost::this_thread::sleep(boost::posix_time::milliseconds(5));
	}
}
void driverbot::StartSaving(std::vector<std::string>  & type){
	// first i have to stop the garbage collector process
	this->running_cleaner.store(false,boost::memory_order_release);
    this->garbage_collection->join();
    // start the global time for logging
    simxCustomGetTime(this->idclient,tStart,simx_opmode_oneshot_wait);
    // initiliaze bookmark
    for(unsigned int ii =0;ii<type.size();ii++){
    	std::vector<DataStoreIt> init;
    	this->bookmarks.push_back(init);
    }
    std::cout<<"starting saving"<<std::endl;
	if(first_write.load(boost::memory_order_acquire)){
		for(unsigned int i =0;i<type.size();i++){
            DataStoreIt app;
            if(type[i].compare("comp_t")==0){
				app = this->ds_t.end();
				app--;
            }if(type[i].compare("j_pos") == 0){
                app = this->ds_ang_pos.end();
				app--;
            }else if(type[i].compare("j_vel") == 0){
				app = this->ds_ang_vel.end();
				app--;
            }else if(type[i].compare("j_tau") == 0){
				app = this->ds_ang_tau.end();
				app--;
			}else if(type[i].compare("cart_pos") == 0){
				app = this->ds_cart.end();
				app--;
			}
			this->bookmarks[i].push_back(app); //---
		}
		this->active_bookmarks.push_back(1); //---
		this->already_saving = true; //---
	 }
}
void driverbot::SaveCheckPoint(std::vector<std::string>  & type){
	if(already_saving){
		for(unsigned int i =0;i<type.size();i++){
			DataStoreIt app;
			if(type[i].compare("comp_t")==0){
				app = this->ds_t.end();
				app--;
			}if(type[i].compare("j_pos") == 0){
				app = this->ds_ang_pos.end();
				app--;
			}else if(type[i].compare("j_vel") == 0){
				app = this->ds_ang_vel.end();
				app--;
			}else if(type[i].compare("j_tau") == 0){
				app = this->ds_ang_tau.end();
				app--;
			}else if(type[i].compare("cart_pos") == 0){
				app = this->ds_cart.end();
				app--;
			}
			this->bookmarks[i].push_back(app);
		}
		this->active_bookmarks.push_back(1); //---
	}
}

void driverbot::DeleteCheckPoint(){
	if(already_saving){
		for(unsigned int i = (active_bookmarks.size()-1);i>0;i--){
			if(active_bookmarks[i] == 1 && i>= 0){
				active_bookmarks[i]=0;
				break;
			}
		}
	}
}
std::vector<Log> driverbot::StopSaving(std::vector<std::string>  & type){
	std::vector<Log> result;
	std::vector<std::pair<int,int> > seq; //---
    // stop the reading thread
    this->running.store(false,boost::memory_order_release);
    this->reader_stats->join();
    // save the last positions
    this->SaveCheckPoint(type);
    // analisys active_bookmarks
    seq = ContSeq(this->active_bookmarks);
    //DEBUG
	std::cout << "active bookmarks" << std::endl;
	for(unsigned int ij =0;ij<this->active_bookmarks.size();ij++){
		std::cout<<active_bookmarks[ij]<<" ";
	}
	std::cout<<std::endl;
	std::cout<<"seq vec"<<std::endl;
	for(unsigned int j = 0; j<seq.size();j++){
		std::cout<<seq[j].first<<" "<<seq[j].second<<std::endl;
	}
	//---
	for(unsigned int i =0;i<type.size();i++){
		if(type[i].compare("comp_t")==0){
			Log app_tot;
			State time_displacement(1);
			time_displacement[0]=0;
			for(unsigned int j = 0;j<seq.size();j++){
				Log app(this->bookmarks[i][seq[j].first],this->bookmarks[i][seq[j].second]);
				for(unsigned int i =0;i<app.size();i++){
					app[i]=(app[i]-app[0]) + time_displacement;
				}
				app_tot.insert(app_tot.end(), app.begin(), app.end());
				time_displacement = app.back(); // here i acquire the last time of first sequence after correction;
			}
			result.push_back(app_tot);
		}else if(type[i].compare("j_pos") == 0){
			Log app_tot;
			for(unsigned int j = 0;j<seq.size();j++){
				Log app(this->bookmarks[i][seq[j].first],this->bookmarks[i][seq[j].second]);
				app_tot.insert(app_tot.end(), app.begin(), app.end());
			}
			result.push_back(app_tot);
        }else if(type[i].compare("j_vel") == 0){
        	Log app_tot;
			for(unsigned int j = 0;j<seq.size();j++){
				Log app(this->bookmarks[i][seq[j].first],this->bookmarks[i][seq[j].second]);
				app_tot.insert(app_tot.end(), app.begin(), app.end());
			}
			result.push_back(app_tot);
        }else if(type[i].compare("j_tau") == 0){
        	Log app_tot;
			for(unsigned int j = 0;j<seq.size();j++){
				Log app(this->bookmarks[i][seq[j].first],this->bookmarks[i][seq[j].second]);
				app_tot.insert(app_tot.end(), app.begin(), app.end());
			}
			result.push_back(app_tot);
		}else if(type[i].compare("cart_pos") == 0){
			Log app_tot;
			for(unsigned int j = 0;j<seq.size();j++){
				Log app(this->bookmarks[i][seq[j].first],this->bookmarks[i][seq[j].second]);
				app_tot.insert(app_tot.end(), app.begin(), app.end());
			}
			result.push_back(app_tot);
		}
	}
    // reactivate the reading thread
    this->running.store(true,boost::memory_order_release);
    this->reader_stats = new boost::thread(boost::bind(&driverbot::Reading,this));
	// reactivate the cleaner tasks
	this->running_cleaner.store(true,boost::memory_order_release);
    this->garbage_collection = new boost::thread(boost::bind(&driverbot::Cleaning,this));
	// clean bookmarks and active_bookmarks
	this->bookmarks.clear();
	this->active_bookmarks.clear();
	this->already_saving = false;
    std::cout << "before return stop" << std::endl;
	return result;
}

// cleaning is too slow if i do not put some kind of sleep in the reading thread
void driverbot::Cleaning(){
	 std::cout<<"start cleaning thread "<<std::endl;
	 while(this->running_cleaner.load(boost::memory_order_acquire)){
		if(this->ds_ang_pos.size() > (unsigned int)(this->Max_DS_allowed) ){
			//DEBUG
			//std::cout<<"this->ds_ang_pos.size() "<<this->ds_ang_pos.size()<<std::endl;
			//std::cout<<"im in cleaning"<<std::endl;
			//---
			this->ds_ang_pos.pop_front();
			this->ds_ang_vel.pop_front();
			this->ds_ang_tau.pop_front();
			this->ds_cart.pop_front();
			this->ds_t.pop_front();
		}
		//boost::this_thread::sleep(boost::posix_time::milliseconds(5));
	}
	std::cout<<"im out of Cleaning thread"<<std::endl;
}

void driverbot::ReadTimeStamp(int operationMode){
	State t_cur(1);
	simxFloat _time[1];
	// DEBUG
	//std::cout<<_time <<std::endl;
	//---
	int result =simxCustomGetTime(this->idclient,_time,operationMode);
	// DEBUG
	//std::cout<< result << std::endl;
	//std::cout<<_time[0] <<std::endl;
	//---
	if(result == 0){
		t_cur[0] = _time[0] - this->tStart[0];
		this->ds_t.push_back(t_cur);
		this->dl_t.store( &(ds_t.back()),boost::memory_order_release);
	}
}
State driverbot::ReadJoints(int operationMode){
	State app_pos(this->NJoints),app_vel(this->NJoints),app_tau(this->NJoints);
	simxFloat p[1];
	simxInt parameterID = 2012; // this parameter is necessary to read the joint velocity
	int res;
	for(int i=0;i<NJoints;i++){
		res = simxGetJointPosition(idclient,joint_handle[i],p,operationMode);
		app_pos[i] = p[0];
	}
	for(int i=0;i<NJoints;i++){
		res = simxGetObjectFloatParameter(idclient,joint_handle[i],parameterID,p,operationMode);
		app_vel[i] = p[0];
	}
	for(int i=0;i<NJoints;i++){
		res = simxGetJointForce(idclient,joint_handle[i],p,operationMode);
		app_tau[i] = p[0];
	}
	//DEBUG
	//std::cout<<"read joints position"<<std::endl;
	//for(unsigned int ii =0;ii<app_pos.size();ii++)
	//	std::cout<<app_pos[ii]<<" ";
	//std::cout<<std::endl;
	//---
	this->ds_ang_pos.push_back(app_pos);
	//DEBUG
	//std::cout<<"address "<<&(ds_ang_pos.back())<<std::endl;
	//----
	this->dl_ang_pos.store( &(ds_ang_pos.back()),boost::memory_order_release);
    //DEBUG
	//std::cout<<"address after writing 1 "<< this->dl_ang_pos.load(boost::memory_order_acquire)<<std::endl;
	//std::cout<<"address after writing 2 "<< this->dl_ang_pos.load(boost::memory_order_acquire)<<std::endl;
    //---
	this->ds_ang_vel.push_back(app_vel);
	this->dl_ang_vel.store(&(ds_ang_vel.back()),boost::memory_order_release);


	this->ds_ang_tau.push_back(app_tau);
	this->dl_ang_tau.store( &(ds_ang_tau.back()),boost::memory_order_release);

	return app_pos;

}
void driverbot::ReadCartesian(State q,int operationMode){
	State p;
	arma::mat R;
	bot->DK(q,p,R);
	this->ds_cart.push_back(p);
	this->dl_cart.store( &(ds_cart.back()),boost::memory_order_release);
}
