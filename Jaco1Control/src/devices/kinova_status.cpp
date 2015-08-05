#include "kinova_status.hpp"

kinova_status::kinova_status(model * mdl)
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
	std::vector<std::string> title(s_b,End(s_b)); //end---
	vis = visualization(NBLOCKS,NJOINTS,chunk_dim,x_min,x_max,low_thresh,high_thresh,y_min,y_max,label,title);
	//thread related
	bot = mdl;
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
	MyGetAngularPosition = (int (*)(AngularPosition &)) dlsym(APIhandle,"GetAngularPosition");
	MyGetAngularVelocity = (int (*)(AngularPosition &)) dlsym(APIhandle,"GetAngularVelocity");
	MyGetAngularForce    = (int (*)(AngularPosition &)) dlsym(APIhandle,"GetAngularForce");
	MyGetCartesianPosition = (int (*)(CartesianPosition &))dlsym(APIhandle,"GetCartesianPosition");
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
	//this->log_stats = new boost::thread(boost::bind(&kinova_status::Logging,this));
	this->garbage_collection = new boost::thread(boost::bind(&kinova_status::Cleaning,this));
}

void kinova_status::Stop()
{

	// maybe add the command to empty the stack of command
	// and to put the velocity at zero
	this->running.store(false,boost::memory_order_release);
	this->reader_stats->join();
	//this->log_stats->join();
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
		if(_first.load(boost::memory_order_acquire))
		{
			boost::chrono::high_resolution_clock::time_point global_begin = boost::chrono::high_resolution_clock::now();
			//DEBUG
		    //std::cout<<"reading new value"<<std::endl;
			//---
			_first.store(false,boost::memory_order_release);
			GeneralInformations cur;
			CartesianPosition   cart_pos;
			AngularPosition     position,velocity,force;

			//(*MyGetGeneralInformations)(cur);
			(*MyGetAngularPosition)(position);
			//(*MyGetAngularVelocity)(velocity);
			//(*MyGetAngularForce)(force);
			//MyGetCartesianPosition(cart_pos);


			this->ReadTimeStamp();
			this->ReadJoints(position,velocity,force);
			this->ReadCartesian(position);
			//this->ReadCurrents(cur);
			if(!first_write.load(boost::memory_order_acquire))
			{
				std::cout<<"first write"<<std::endl;
				first_write.store(true,boost::memory_order_release);
			}
			_second.store(true,boost::memory_order_release);
		    std::cout << "time spent Reading: " << boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - global_begin).count() << " ms\n";
		}
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

void kinova_status::ReadTimeStamp()
{
    State t_rob(1),t_cur(1);

    t_cur[0] = (double)((clock() - tStart)/CLOCKS_PER_SEC);
    //t_rob[0] = info.TimeFromStartup;

	this->ds_comp_t.push_back(t_cur);
	// i can write for the vis less often then the other op
	this->comp_t.push( &(ds_comp_t.back()) );
	//this->ds_robot_t.push_back(t_rob);

}

void kinova_status::ReadJoints(AngularPosition & position,AngularPosition & velocity,AngularPosition & force)
{
	// joint position
	State app(6);
	app[0]=position.Actuators.Actuator1;
	app[1]=position.Actuators.Actuator2;
	app[2]=position.Actuators.Actuator3;
	app[3]=position.Actuators.Actuator4;
	app[4]=position.Actuators.Actuator5;
	app[5]=position.Actuators.Actuator6;


	// convert angle from deg to rad
	app=app*DEG;

	//DEBUG
	//for(int i=0;i<6;i++)
	//	std::cout<<app[i]<<" ";
	//std::cout<<std::endl;
	//---

	this->ds_ang_pos.push_back(app);
	this->dl_ang_pos.store( &(ds_ang_pos.back()),boost::memory_order_release);
	// i can write for the vis less often then the other op
	this->ang_pos.push( &(ds_ang_pos.back()) );

	// joint velocity
	/*app[0]=velocity.Actuators.Actuator1;
	app[1]=velocity.Actuators.Actuator2;
	app[2]=velocity.Actuators.Actuator3;
	app[3]=velocity.Actuators.Actuator4;
	app[4]=velocity.Actuators.Actuator5;
	app[5]=velocity.Actuators.Actuator6;

	//DEBUG
	std::cout<<"velocity"<<std::endl;
	for(int i=0;i<6;i++)
		std::cout<<app[i]<<" ";
	std::cout<<std::endl;
	//---

	this->ds_ang_vel.push_back(app);
	this->dl_ang_vel.store(&(ds_ang_vel.back()),boost::memory_order_release);*/

	// joint torques
	app[0]=force.Actuators.Actuator1;
	app[1]=force.Actuators.Actuator2;
	app[2]=force.Actuators.Actuator3;
	app[3]=force.Actuators.Actuator4;
	app[4]=force.Actuators.Actuator5;
	app[5]=force.Actuators.Actuator6;

	this->ds_ang_tau.push_back(app);
	this->dl_ang_tau.store( &(ds_ang_tau.back()),boost::memory_order_release);
	// i can write for the vis less often then the other op
	this->ang_tau.push( &(ds_ang_tau.back()) );

}

// TO DO: SPLIT CARTESIAN POSITION FROM ORIENTATION INTO DIFFERENT VARIABLE
void kinova_status::ReadCartesian(AngularPosition & position)
{
	// cartesian position
	State q(6);
	State cart_pos;
	arma::mat R;
	q[0]=position.Actuators.Actuator1;
	q[1]=position.Actuators.Actuator2;
	q[2]=position.Actuators.Actuator3;
	q[3]=position.Actuators.Actuator4;
	q[4]=position.Actuators.Actuator5;
	q[5]=position.Actuators.Actuator6;

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
std::vector<State> kinova_status::FirstRead(std::vector<std::string> type)
{
	std::vector<State> res;
	boost::this_thread::sleep(boost::posix_time::milliseconds(10)); // this sleep is necessary because at the begining i read a lot of nasty value
	GetLastValue(res,type);
	// DEBUG
	//std::cout<<" FirstRead result  "<<result<<std::endl;
	//---
	return res;

}


bool kinova_status::GetLastValue(std::vector<State>& res, std::vector<std::string>  & type)
{
	if(first_write.load(boost::memory_order_acquire))
	{
		for(unsigned int i =0;i<type.size();i++)
		{
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

