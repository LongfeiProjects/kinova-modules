#include "kinova_status_openAPI.hpp"

kinova_status_openapi::kinova_status_openapi(model * mdl)
: running(true)
, running_cleaner(true)
, first_write(false)
, ang_pos(0)
, ang_tau(0)
, cart_f(0)
, mot_amp(0)
, comp_t(0)
{
	// move this guys outside the constructor
	int NBLOCKS = 3;
	int NJOINTS = 6;
	int chunk_dim = 500;
	PLFLT x_min = 0;
	PLFLT x_max = 100;
	PLFLT low_thresh = 5;
	PLFLT high_thresh = 5;
	const double d_a[] = {-360,-30,-10};
	std::vector<PLFLT> y_min(d_a,End(d_a));
	const double d_b[] = {360,30,10};
	std::vector<PLFLT>y_max(d_b,End(d_b));
	const char *s_a[] = {"deg","Nm","A"};
	std::vector<std::string> label(s_a,End(s_a));
	const char *s_b[]= {"joints","torques","currents"};
	std::vector<std::string> title(s_b,End(s_b)); //end---
	vis = visualization(NBLOCKS,NJOINTS,chunk_dim,x_min,x_max,low_thresh,high_thresh,y_min,y_max,label,title);
	//thread related
	bot = mdl;
	Max_DS_allowed = 10000;
	reader_stats = NULL;
	log_stats = NULL;
	garbage_collection = NULL;

	KinDrv::init_usb();

	try {
		this->arm=new KinDrv::JacoArm();
	  } catch(KinDrv::KinDrvException &e) {
	    printf("error %i: %s \n", e.error(), e.what());
	  }
	this->arm->start_api_ctrl();

}

kinova_status_openapi::~kinova_status_openapi()
{}

void kinova_status_openapi::Start()
{
	this->reader_stats = new boost::thread(boost::bind(&kinova_status_openapi::Reading,this));
	//this->log_stats = new boost::thread(boost::bind(&kinova_status::Logging,this));
	this->garbage_collection = new boost::thread(boost::bind(&kinova_status_openapi::Cleaning,this));
}

void kinova_status_openapi::Stop()
{

	// maybe add the command to empty the stack of command
	// and to put the velocity at zero
	this->arm->erase_trajectories();
	this->running.store(false,boost::memory_order_release);
	this->running_cleaner.store(false,boost::memory_order_release);
	this->reader_stats->join();
	//this->log_stats->join();
	this->garbage_collection->join();
	// close open api kinova
	this->arm->stop_api_ctrl();
	KinDrv::close_usb();
	std::cout<<"close all thread"<<std::endl;
}

// KINOVA API DEPENDANT // // in reading i update the value for control
void kinova_status_openapi::Reading()
{
    // start the global time for logging
    this->tStart = boost::chrono::high_resolution_clock::now();
	boost::chrono::milliseconds reading_time;
	while(this->running.load(boost::memory_order_acquire))
	{
		boost::chrono::high_resolution_clock::time_point global_begin = boost::chrono::high_resolution_clock::now();
		KinDrv::jaco_position_t position,velocity,force;
        velocity = this->arm->get_ang_vel();
		position = this->arm->get_ang_pos();
		//force = this->arm->get_ang_force();
		//cart_pos = this->arm->get_cart_pos();
		this->ReadTimeStamp();
		this->ReadJoints(position,velocity,force);
		this->ReadCartesian(position);
		//this->ReadCurrents(cur);
		if(!first_write.load(boost::memory_order_acquire))
		{
			std::cout<<"first write"<<std::endl;
			first_write.store(true,boost::memory_order_release);
		}
		reading_time = boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - global_begin);
    	//std::cout << "time spent Reading: " << reading_time.count() << " ms\n";
        int test_time = boost::chrono::round<boost::chrono::milliseconds>(reading_time).count();
        if(test_time < 10)
        {
            boost::this_thread::sleep(boost::posix_time::milliseconds(10-test_time));
        }
	}
	std::cout<<"im out of Reading thread"<<std::endl;
}

// in logging i update the value for logging and visualization
void kinova_status_openapi::Logging()
{
	while(this->running.load(boost::memory_order_acquire))
	{
		std::vector<State_ptr> visvec;
		int readckeck=this->Read4Vis(visvec);
		if(readckeck)
		{
			vis.Update(visvec);
			vis.Plot();
		}
	}
	std::cout<<"im out of Logging thread"<<std::endl;
}

void kinova_status_openapi::Cleaning()
{
    std::cout<<"start cleaning thread"<<std::endl;
	while(this->running_cleaner.load(boost::memory_order_acquire))
	{
		if(this->ds_ang_pos.size() > (unsigned int)(this->Max_DS_allowed) ){
			this->ds_ang_pos.pop_front();
        }
        if(this->ds_ang_vel.size() > (unsigned int)(this->Max_DS_allowed)){
            this->ds_ang_vel.pop_front();
        }
        if(this->ds_ang_tau.size() > (unsigned int)(this->Max_DS_allowed)){
           this->ds_ang_tau.pop_front();
        }
        if(this->ds_cart_f.size() > (unsigned int)(this->Max_DS_allowed)){
           this->ds_cart_f.pop_front();
        }
        if(this->ds_cart_pos.size() > (unsigned int)(this->Max_DS_allowed)){
           this->ds_cart_pos.pop_front();
        }
        if(this->ds_comp_t.size() > (unsigned int)(this->Max_DS_allowed)){
           this->ds_comp_t.pop_front();
        }
        if(this->ds_mot_amp.size() > (unsigned int)(this->Max_DS_allowed)){
           this->ds_mot_amp.pop_front();
        }
        if(this->ds_robot_t.size() > (unsigned int)(this->Max_DS_allowed)){
           this->ds_robot_t.pop_front();
        }

    }

	std::cout<<"im out of Cleaning thread"<<std::endl;

}

void kinova_status_openapi::StartSaving(std::vector<std::string>  & type)
{
	// first i have to stop the garbage collector process
	this->running_cleaner.store(false,boost::memory_order_release);
	this->garbage_collection->join();
    // start the global time for logging
    this->tStart = boost::chrono::high_resolution_clock::now();

	if(first_write.load(boost::memory_order_acquire))
    {
		for(unsigned int i =0;i<type.size();i++)
		{
			DataStoreIt app;
			if(type[i].compare("comp_t")==0)
			{
                app = this->ds_comp_t.end();
                app--;
			}
			if(type[i].compare("j_pos") == 0)
			{
                app = this->ds_ang_pos.end();
				app--;
			}
			else if(type[i].compare("j_vel") == 0)
			{
				app = this->ds_ang_vel.end();
				app--;
			}
			else if(type[i].compare("j_tau") == 0)
			{
				app = this->ds_ang_tau.end();
				app--;
			}
			else if(type[i].compare("cart_f") == 0)
			{
				app = this->ds_cart_f.end();
				app--;
			}
			else if(type[i].compare("cart_pos") == 0)
			{
                std::cout << "cart_pos if" << std::endl;
				app = this->ds_cart_pos.end();
				app--;
			}
			this->bookmarks.push_back(app);
		}
	 }
}


std::vector<Log> kinova_status_openapi::StopSaving(std::vector<std::string>  & type)
{
	std::vector<Log> result;
	for(unsigned int i =0;i<type.size();i++)
	{
		if(type[i].compare("comp_t")==0)
		{
			Log app(this->bookmarks[i],this->ds_comp_t.end());

           std::cout<< "app size = " << app.size() << std::endl;
			for(unsigned int i =0;i<app.size();i++)
			{
				app[i]=app[i]-app[0];
			}
		    result.push_back(app);

		}
		if(type[i].compare("j_pos") == 0)
		{   // here i construct the log by assigning to the vec of state Log
			// the sublist
			Log app(this->bookmarks[i],this->ds_ang_pos.end());
			result.push_back(app);
		}
		else if(type[i].compare("j_vel") == 0)
		{
			Log app(this->bookmarks[i],this->ds_ang_vel.end());
			result.push_back(app);
		}
		else if(type[i].compare("j_tau") == 0)
		{
			Log app(this->bookmarks[i],this->ds_ang_tau.end());
			result.push_back(app);
		}
		else if(type[i].compare("cart_f") == 0)
		{
			Log app(this->bookmarks[i],this->ds_cart_f.end());
			result.push_back(app);
		}
		else if(type[i].compare("cart_pos") == 0)
		{
			Log app(this->bookmarks[i],this->ds_cart_pos.end());
			result.push_back(app);
		}
	}
	// reactivate the cleaner tasks
	this->running_cleaner.store(true,boost::memory_order_release);
	this->garbage_collection = new boost::thread(boost::bind(&kinova_status_openapi::Cleaning,this));
	// clean bookmarks
	this->bookmarks.clear();

	return result;
}

// FROM THIS POINT ALL THE FUNCTIONS are KINOVA API DEPENDANT //

void kinova_status_openapi::ClearCommands()
{
	this->arm->erase_trajectories();
}

void kinova_status_openapi::RestartAPI(){
    this->running.store(false,boost::memory_order_release);
    this->running_cleaner.store(false,boost::memory_order_release);
    arm->stop_api_ctrl();
    arm->start_api_ctrl();
    this->running.store(true,boost::memory_order_release);
    this->running_cleaner.store(true,boost::memory_order_release);
}

void kinova_status_openapi::ReadTimeStamp()
{
    State t_cur(1);
    boost::chrono::milliseconds reading_time;
    reading_time = boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - this->tStart);
    t_cur[0] =  boost::chrono::round<boost::chrono::milliseconds>(reading_time).count();

	this->ds_comp_t.push_back(t_cur);
	// i can write for the vis less often then the other op
	this->comp_t.push( &(ds_comp_t.back()) );
	//this->ds_robot_t.push_back(t_rob);

}

void kinova_status_openapi::ReadJoints(KinDrv::jaco_position_t &position,KinDrv::jaco_position_t & velocity,KinDrv::jaco_position_t & force)
{
	// joint position
	State app(9);
	app[0]=position.joints[0];
	app[1]=position.joints[1];
	app[2]=position.joints[2];
	app[3]=position.joints[3];
	app[4]=position.joints[4];
	app[5]=position.joints[5];
	// convert angle from deg to rad
	app=app*DEG;
	app[6]=position.finger_position[1];
	app[7]=position.finger_position[2];
	app[8]=position.finger_position[3];
	this->ds_ang_pos.push_back(app);
	this->dl_ang_pos.store( &(ds_ang_pos.back()),boost::memory_order_release);
	// i can write for the vis less often then the other op
	this->ang_pos.push( &(ds_ang_pos.back()) );
	// joint velocity
    State app_short(6);
    app_short[0]=velocity.joints[0];
    app_short[1]=velocity.joints[1];
    app_short[2]=velocity.joints[2];
    app_short[3]=velocity.joints[3];
    app_short[4]=velocity.joints[4];
    app_short[5]=velocity.joints[5];
    //std::cout<<app_short << std::endl;
    this->ds_ang_vel.push_back(app_short);
	this->dl_ang_vel.store(&(ds_ang_vel.back()),boost::memory_order_release);
	// joint torques
	/*app[0]=force.joints[0];
	app[1]=force.joints[1];
	app[2]=force.joints[2];
	app[3]=force.joints[3];
	app[4]=force.joints[4];
	app[5]=force.joints[5];
	this->ds_ang_tau.push_back(app);
	this->dl_ang_tau.store( &(ds_ang_tau.back()),boost::memory_order_release);
	// i can write for the vis less often then the other op
	this->ang_tau.push( &(ds_ang_tau.back()) );*/
}

// TO DO: SPLIT CARTESIAN POSITION FROM ORIENTATION INTO DIFFERENT VARIABLE
void kinova_status_openapi::ReadCartesian(KinDrv::jaco_position_t & position)
{
	// cartesian position
	State q(6);
	State cart_pos;
	arma::mat R;
    q[0]=position.joints[0];
    q[1]=position.joints[1];
    q[2]=position.joints[2];
    q[3]=position.joints[3];
    q[4]=position.joints[4];
    q[5]=position.joints[5];

	// convert angle from deg to rad
	q=q*DEG;


    bot->DK(q,cart_pos,R);
	this->ds_cart_pos.push_back(cart_pos);
	this->dl_cart_pos.store( &(ds_cart_pos.back()),boost::memory_order_release);

	// cartesian forces
	/*State force(6);
	force[0]=info.Force.CartesianPosition.X;
	force[1]=info.Force.CartesianPosition.Y;
	force[2]=info.Force.CartesianPosition.Z;
	force[3]=info.Force.CartesianPosition.ThetaX;
	force[4]=info.Force.CartesianPosition.ThetaY;
	force[5]=info.Force.CartesianPosition.ThetaZ;
	this->ds_cart_f.push_back(app);
	this->dl_cart_f.store( &(ds_cart_f.back()),boost::memory_order_release);*/
}

/*void kinova_status::ReadCurrents(GeneralInformations & info)
{
	State app(6);
	app[0]=info.Current.Actuators.Actuator1;
	app[1]=info.Current.Actuators.Actuator2;
	app[2]=info.Current.Actuators.Actuator3;
	app[3]=info.Current.Actuators.Actuator4;
	app[4]=info.Current.Actuators.Actuator5;
	app[5]=info.Current.Actuators.Actuator6;
	this->ds_mot_amp.push_back(app);
	// i can write for the vis less often then the other op
	this->mot_amp.push( &(ds_mot_amp.back()) );
}*/

int kinova_status_openapi::Read4Vis(std::vector<State_ptr> & lastval)
{
	State_ptr app;

	if( !(this->comp_t.empty()) && !(this->ang_pos.empty()) && !(this->ang_tau.empty()) && !(this->mot_amp.empty()) )
	{
		//copy last time
		this->comp_t.pop(app);
		lastval.push_back(app);
		//copy joint
		this->ang_pos.pop(app);
		lastval.push_back(app);
		// copy tau
		this->ang_tau.pop(app);
		lastval.push_back(app);
		// copy mot ampere
		this->mot_amp.pop(app);
		lastval.push_back(app);
	}
	else
	{
		return 0;
	}

	return 1;
}
std::vector<State> kinova_status_openapi::FirstRead(std::vector<std::string> type)
{
	std::vector<State> res;
	boost::this_thread::sleep(boost::posix_time::milliseconds(10)); // this sleep is necessary because at the begining i read a lot of nasty value
	GetLastValue(res,type);
	// DEBUG
	//std::cout<<" FirstRead result  "<<result<<std::endl;
	//---
	return res;

}


bool kinova_status_openapi::GetLastValue(std::vector<State>& res, std::vector<std::string>  & type)
{
	if(first_write.load(boost::memory_order_acquire))
	{
		for(unsigned int i =0;i<type.size();i++)
		{
            std::cout<<"reading succed"<<std::endl;
			State app;
			if(type[i].compare("j_pos") == 0)
			{	//DEBUG
                //std::cout<<"reading last joints"<<std::endl;
				//---
				app = *(this->dl_ang_pos.load(boost::memory_order_acquire));
			}
			else if(type[i].compare("j_vel") == 0)
			{
				app = *(this->dl_ang_vel.load(boost::memory_order_acquire));
			}
			else if(type[i].compare("j_tau") == 0)
			{
				app = *(this->dl_ang_tau.load(boost::memory_order_acquire));
			}
			else if(type[i].compare("cart_f") == 0)
			{
				app = *(this->dl_cart_f.load(boost::memory_order_acquire));
			}
			else if(type[i].compare("cart_pos") == 0)
			{
				app = *(this->dl_cart_pos.load(boost::memory_order_acquire));
			}
			res.push_back(app);
		}

		return true;
	}
	return false;
}

