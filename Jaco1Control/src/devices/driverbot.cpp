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
, first_write(false)
{
	std::string connectionAddress= "127.0.0.1";
	simxInt connectionPort = 19996;
	simxInt timeOutInMs = 2000;
	simxInt commThreadCycleInMs = 5;
	//  if different from zero, then the function blocks until connected (or timed out).
	simxUChar waitUntilConnected = 'a';
	// if different from zero, then the communication thread will not attempt a second connection if a connection was lost.
	simxUChar doNotReconnectOnceDisconnected = 'a';
	//open connection
 	idclient = simxStart(connectionAddress.c_str(),connectionPort,waitUntilConnected,doNotReconnectOnceDisconnected,timeOutInMs,commThreadCycleInMs);
 	sync = _sync;
 	if(sync)
 	{
 		simxUChar en='enable'; // to check
 		simxSynchronous(this->idclient,en);
 	}
 	// read joint handle
 	bool more_joint = true;
 	simxInt* handle = NULL;
 	NJoints =0;
 	while(more_joint)
 	{
 		std::string current_joint_name(joint_base_name);
 		std::string s = SSTR(NJoints);
 		s = current_joint_name + s;
 		int res = simxGetObjectHandle(this->idclient,s.c_str(),handle,simx_opmode_oneshot);
 		if(res != 0)
 			more_joint = false;
 		NJoints++;
 	}
 	bot = _bot;
 	reader_stats = NULL;
 	garbage_collection = NULL;
 	Max_DS_allowed = 10000;

}

driverbot::~driverbot(){}

bool driverbot::GetLastValue(State& res, std::string type)
{
	if(first_write.load(boost::memory_order_acquire))
	{
		if(type.compare("j_pos") == 0)
		{
			res = *(this->dl_ang_pos.load(boost::memory_order_acquire));
		}
		else if(type.compare("j_vel") == 0)
		{
			res = *(this->dl_ang_vel.load(boost::memory_order_acquire));
		}
		else if(type.compare("j_tau") == 0)
		{
		   res = *(this->dl_ang_tau.load(boost::memory_order_acquire));
		}
		return true;
	}
	return false;

}

void driverbot::Start()
{
	this->reader_stats = new boost::thread(boost::bind(&driverbot::ReadingMutex,this));
	this->garbage_collection = new boost::thread(boost::bind(&driverbot::Cleaning,this));
	simxStartSimulation(this->idclient,simx_opmode_oneshot);
}

void driverbot::Stop()
{
	this->running.store(false,boost::memory_order_release);
	this->reader_stats->join();
	this->garbage_collection->join();
	// close simulator
	simxFinish(this->idclient);
	std::cout<<"close all thread"<<std::endl;

}
void driverbot::Reading()
{
	while(this->running.load(boost::memory_order_acquire))
	{
		this->ReadTimeStamp();
		State pos=this->ReadJoints();
		this->ReadCartesian(pos);
		if(!first_write.load(boost::memory_order_acquire))
		{
			first_write.store(true,boost::memory_order_release);
		}
	}
}
void driverbot::Cleaning()
{
	while(this->running.load(boost::memory_order_acquire))
	{
		if(this->ds_ang_pos.size() > (unsigned int)(this->Max_DS_allowed) )
		{
			this->ds_ang_pos.pop_front();
			this->ds_ang_vel.pop_front();
			this->ds_ang_tau.pop_front();
			this->ds_cart.pop_front();
			this->ds_t.pop_front();
		}
	}
	std::cout<<"im out of Cleaning thread"<<std::endl;
}

void driverbot::ReadTimeStamp()
{
	State t_cur(1);
	simxFloat* _time = NULL;
	simxCustomGetTime(this->idclient,_time,simx_opmode_oneshot);
	t_cur[0] = *(_time);
	this->ds_t.push_back(t_cur);
	this->dl_t.store( &(ds_t.back()),boost::memory_order_release);
}
State driverbot::ReadJoints()
{
	State app_pos(this->NJoints),app_vel(this->NJoints),app_tau(this->NJoints);
	simxFloat* p = NULL;
	simxInt parameterID = 2012; // this parameter is necessary to read the joint velocity
	for(int i=0;i<NJoints;i++)
	{
		simxGetJointPosition(idclient,joint_handle[i],p,simx_opmode_oneshot);
		app_pos[i] = (*p);
	}
	for(int i=0;i<NJoints;i++)
	{
		simxGetObjectFloatParameter(idclient,joint_handle[i],parameterID,p,simx_opmode_oneshot);
		app_vel[i] = (*p);
	}
	for(int i=0;i<NJoints;i++)
	{
		simxGetJointForce(idclient,joint_handle[i],p,simx_opmode_oneshot);
		app_tau[i] = (*p);
	}

	this->ds_ang_pos.push_back(app_pos);
	this->dl_ang_pos.store( &(ds_ang_pos.back()),boost::memory_order_release);


	this->ds_ang_vel.push_back(app_vel);
	this->dl_ang_vel.store(&(ds_ang_vel.back()),boost::memory_order_release);


	this->ds_ang_tau.push_back(app_tau);
	this->dl_ang_tau.store( &(ds_ang_tau.back()),boost::memory_order_release);

	return app_pos;

}
void driverbot::ReadCartesian(State q)
{
	State p;
	arma::mat R;
	bot->DK(q,p,R);

	this->ds_cart.push_back(p);
	this->dl_cart.store( &(ds_cart.back()),boost::memory_order_release);

}

