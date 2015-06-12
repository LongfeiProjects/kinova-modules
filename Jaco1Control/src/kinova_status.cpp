#include "kinova_status.hpp"
#include "robot/dummybot.hpp"


kinova_status::kinova_status()
: running(true)
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
	std::vector<std::string> title(s_b,End(s_b));
	vis = visualization(NBLOCKS,NJOINTS,chunk_dim,x_min,x_max,low_thresh,high_thresh,y_min,y_max,label,title);
	// robot
	dummybot *test = new dummybot();
	//this->bot(test);
	//thread related
	Max_DS_allowed = 10000;
	reader_stats = NULL;
	log_stats = NULL;
	garbage_collection = NULL;
	// kinova api
	APIhandle = dlopen("Kinova.API.USBCommandLayerUbuntu.so",RTLD_NOW|RTLD_GLOBAL);
	MyInitAPI = (int (*)()) dlsym(APIhandle,"InitAPI");
	MyCloseAPI = (int (*)()) dlsym(APIhandle,"CloseAPI");
	MyStartControlAPI = (int (*)()) dlsym(APIhandle,"StartControlAPI");
	MyStopControlAPI =  (int (*)()) dlsym(APIhandle,"StopControlAPI");
	MyGetGeneralInformations = (int (*)(GeneralInformations &)) dlsym(APIhandle,"GetGeneralInformations");
	MyGetGlobalTrajectoryInfo = (int (*)(TrajectoryFIFO &)) dlsym(APIhandle,"GetGlobalTrajectoryInfo");
	MyGetAngularVelocity = (int (*)(AngularPosition &)) dlsym(APIhandle,"GetAngularVelocity");
	//We test if that all the functions were loaded corectly.
	if((MyInitAPI == NULL) || (MyCloseAPI == NULL) || (MyStartControlAPI == NULL) )
	{
		std::cout << "Can't load the inizialization functions from the library." << std::endl;
	}
	else
	{
		//Init the API
		bool result = (*MyInitAPI)();

		if(result == SUCCESS)
		{
			result = (*MyStartControlAPI)();
		}

		if(result == SUCCESS)
		{
			std::cout<<"can execute program kinova status"<<std::endl;
		}
		else
		{
			std::cout << "Cannot initializes the API." << std::endl;
		}
	}
}

kinova_status::~kinova_status()
{}

void kinova_status::Start()
{
	this->reader_stats = new boost::thread(boost::bind(&kinova_status::Reading,this));
	this->log_stats = new boost::thread(boost::bind(&kinova_status::Logging,this));
	this->garbage_collection = new boost::thread(boost::bind(&kinova_status::Cleaning,this));
}

void kinova_status::Stop()
{
	this->running.store(false,boost::memory_order_release);
	this->reader_stats->join();
	this->log_stats->join();
	this->garbage_collection->join();
	// close api kinova
	(*MyStopControlAPI)();
	(*MyCloseAPI)();
	std::cout<<"close all thread"<<std::endl;
}

// KINOVA API DEPENDANT // // in reading i update the value for control
void kinova_status::Reading()
{
	this->tStart = clock();
	while(this->running.load(boost::memory_order_acquire))
	{
		GeneralInformations cur;
		AngularPosition     av;
		(*MyGetGeneralInformations)(cur);
		(*MyGetAngularVelocity)(av);
		this->ReadTimeStamp(cur);
		this->ReadJoints(cur,av);
		this->ReadCartesian(cur);
		this->ReadCurrents(cur);
		if(!first_write.load(boost::memory_order_acquire))
		{
			first_write.store(true,boost::memory_order_release);
		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(10000));
	}
	std::cout<<"im out of Reading thread"<<std::endl;
}

// in logging i update the value for logging and visualization
void kinova_status::Logging()
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

void kinova_status::Cleaning()
{
	while(this->running.load(boost::memory_order_acquire))
	{
		if(this->ds_ang_pos.size() > (unsigned int)(this->Max_DS_allowed) )
		{
			this->ds_ang_pos.pop_front();
			this->ds_ang_vel.pop_front();
			this->ds_ang_tau.pop_front();
			this->ds_cart_f.pop_front();
			this->ds_cart_pos.pop_front();
			this->ds_comp_t.pop_front();
			this->ds_mot_amp.pop_front();
			this->ds_robot_t.pop_front();
		}
	}
	std::cout<<"im out of Cleaning thread"<<std::endl;

}


// FROM THIS POINT FUNCTIONS are KINOVA API DEPENDANT //

void kinova_status::ReadTimeStamp(GeneralInformations & info)
{
    State t_rob(1),t_cur(1);

    t_cur[0] = (double)((clock() - tStart)/CLOCKS_PER_SEC);
    t_rob[0] = info.TimeFromStartup;

	this->ds_comp_t.push_back(t_cur);
	// i can write for the vis less often then the other op
	this->comp_t.push( &(ds_comp_t.back()) );
	this->ds_robot_t.push_back(t_rob);

}

void kinova_status::ReadJoints(GeneralInformations & info,AngularPosition & av)
{
	State app(6);
	app[0]=info.Position.Actuators.Actuator1;
	app[1]=info.Position.Actuators.Actuator2;
	app[2]=info.Position.Actuators.Actuator3;
	app[3]=info.Position.Actuators.Actuator4;
	app[4]=info.Position.Actuators.Actuator5;
	app[5]=info.Position.Actuators.Actuator6;

	//DEBUG
	for(int i=0;i<6;i++)
		std::cout<<app[i]<<" ";
	std::cout<<std::endl;
	//---

	this->ds_ang_pos.push_back(app);
	this->dl_ang_pos.store( &(ds_ang_pos.back()),boost::memory_order_release);
	// i can write for the vis less often then the other op
	this->ang_pos.push( &(ds_ang_pos.back()) );

	app[0]=av.Actuators.Actuator1;
	app[1]=av.Actuators.Actuator2;
	app[2]=av.Actuators.Actuator3;
	app[3]=av.Actuators.Actuator4;
	app[4]=av.Actuators.Actuator5;
	app[5]=av.Actuators.Actuator6;

	this->ds_ang_vel.push_back(app);
	this->dl_ang_vel.store(&(ds_ang_vel.back()),boost::memory_order_release);

	app[0]=info.Force.Actuators.Actuator1;
	app[1]=info.Force.Actuators.Actuator2;
	app[2]=info.Force.Actuators.Actuator3;
	app[3]=info.Force.Actuators.Actuator4;
	app[4]=info.Force.Actuators.Actuator5;
	app[5]=info.Force.Actuators.Actuator6;

	this->ds_ang_tau.push_back(app);
	this->dl_ang_tau.store( &(ds_ang_tau.back()),boost::memory_order_release);
	// i can write for the vis less often then the other op
	this->ang_tau.push( &(ds_ang_tau.back()) );

}

void kinova_status::ReadCartesian(GeneralInformations & info)
{
	State app(6);

	app[0]=info.Position.CartesianPosition.X;
	app[1]=info.Position.CartesianPosition.Y;
	app[2]=info.Position.CartesianPosition.Z;
	app[3]=info.Position.CartesianPosition.ThetaX;
	app[4]=info.Position.CartesianPosition.ThetaY;
	app[5]=info.Position.CartesianPosition.ThetaZ;
	this->ds_cart_pos.push_back(app);
	this->dl_cart_pos.store( &(ds_cart_pos.back()),boost::memory_order_release);


	app[0]=info.Force.CartesianPosition.X;
	app[1]=info.Force.CartesianPosition.Y;
	app[2]=info.Force.CartesianPosition.Z;
	app[3]=info.Force.CartesianPosition.ThetaX;
	app[4]=info.Force.CartesianPosition.ThetaY;
	app[5]=info.Force.CartesianPosition.ThetaZ;
	this->ds_cart_f.push_back(app);
	this->dl_cart_f.store( &(ds_cart_f.back()),boost::memory_order_release);
}

void kinova_status::ReadCurrents(GeneralInformations & info)
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
}

int kinova_status::Read4Vis(std::vector<State_ptr> & lastval)
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

bool kinova_status::GetLastValue(State& res, std::string type)
{
	if(first_write.load(boost::memory_order_acquire))
	{
		if(type.compare("j_pos") == 0)
		{	//DEBUG
			//std::cout<<"reading last joints"<<std::endl;
			//---
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
		else if(type.compare("cart_f") == 0)
		{
			res = *(this->dl_cart_f.load(boost::memory_order_acquire));
		}
		else if(type.compare("cart_pos") == 0)
		{
			res = *(this->dl_cart_pos.load(boost::memory_order_acquire));
		}


		return true;
	}
	return false;
}

