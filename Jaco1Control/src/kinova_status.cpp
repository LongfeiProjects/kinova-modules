#include "kinova_status.hpp"


using namespace kinenv;

kinova_status::kinova_status()
{


}

kinova_status::kinova_status(int i)
: running(true)
, ang_pos(0)
, ang_tau(0)
, cart_f(0)
, mot_amp(0)
, comp_t(0)
{
	int NBLOCKS = 3;
	int NJOINTS = 6;
	int chunk_dim = 500;
	PLFLT x_min = 0;
	PLFLT x_max = 10;
	PLFLT low_thresh = 2;
	PLFLT high_thresh = 2;
	static const double d_a[] = {-180,-30,-10};
	std::vector<PLFLT> y_min(d_a, end(d_a));
	static const double d_b[] = {180,30,10};
	std::vector<PLFLT>y_max(d_b, end(d_b));
	const char *s_a[] = {"deg/s","Nm","A"};
	std::vector<std::string> label(s_a, end(s_a));
	const char *s_b[] = {"joints","torques","currents"};
	std::vector<std::string> title(s_b, end(s_b));
	vis = visualization(NBLOCKS,NJOINTS,chunk_dim,x_min,x_max,low_thresh,high_thresh,y_min,y_max,label,title);
	void * APIhandle = dlopen("Kinova.API.USBCommandLayerUbuntu.so",RTLD_NOW|RTLD_GLOBAL);
	MyGetGeneralInformations = (int (*)(GeneralInformations &)) dlsym(APIhandle,"GetGeneralInformations");
	MyGetGlobalTrajectoryInfo = (int (*)(TrajectoryFIFO &)) dlsym(APIhandle,"GetGlobalTrajectoryInfo");
	reader_stats = NULL;
	log_stats = NULL;
}

// copy constructor. necessary with atomic variable
/*kinova_status::kinova_status(kinova_status & sts)
{
	this->MyGetGeneralInformations = sts.MyGetGeneralInformations;
	this->MyGetGlobalTrajectoryInfo = sts.MyGetGlobalTrajectoryInfo;
	this->reader_stats = sts.reader_stats;
	this->running.;

}*/

void kinova_status::LaunchThread()
{
	this->reader_stats = new boost::thread(boost::bind(&kinova_status::Reading,this));
	this->log_stats = new boost::thread(boost::bind(&kinova_status::Logging,this));
}

void kinova_status::CloseThread()
{
	this->running.store(false,boost::memory_order_release);
	this->reader_stats->join();
	this->log_stats->join();
}

// KINOVA API DEPENDANT // // in reading i update the value for control
void kinova_status::Reading()
{
	this->tStart = clock();
	while(this->running.load(boost::memory_order_acquire))
	{
		GeneralInformations cur;
		(*MyGetGeneralInformations)(cur);
		this->ReadTimeStamp(cur);
		this->ReadJoints(cur);
		this->ReadCartesian(cur);
		//this->ReadCurrents(cur);
		//DEBUG
		//---
	}
	std::cout<<"im out thread"<<std::endl;
}

// in logging i update the value for logging and visualization
void kinova_status::Logging()
{
	while(this->running.load(boost::memory_order_acquire))
	{
		std::vector<std::vector<double>* > visvec;
		this->Read4Vis(visvec);
		vis.Update(visvec);
		vis.Plot();

	}

}

// FROM THIS POINT FUNCTIONS KINOVA API DEPENDANT //

void kinova_status::ReadTimeStamp(GeneralInformations & info)
{

    std::vector<double> t_rob(1),t_cur(1);

    t_cur[0] = (double)(clock() - tStart)/CLOCKS_PER_SEC;
    t_rob[0] = info.TimeFromStartup;

	this->ds_comp_t.push_back(t_cur);
	// i can write for the vis less often then the other op
	this->comp_t.push( &(ds_comp_t.back()) );

	this->ds_robot_t.push_back(t_rob);

}

void kinova_status::ReadJoints(GeneralInformations & info)
{
	std::vector<double> app(6);

	app[0]=info.Position.Actuators.Actuator1;
	app[1]=info.Position.Actuators.Actuator2;
	app[2]=info.Position.Actuators.Actuator3;
	app[3]=info.Position.Actuators.Actuator4;
	app[4]=info.Position.Actuators.Actuator5;
	app[5]=info.Position.Actuators.Actuator6;
	this->ds_ang_pos.push_back(app);
	// i can write for the vis less often then the other op
	this->ang_pos.push( &(ds_ang_pos.back()) );

	app[0]=info.Force.Actuators.Actuator1;
	app[1]=info.Force.Actuators.Actuator2;
	app[2]=info.Force.Actuators.Actuator3;
	app[3]=info.Force.Actuators.Actuator4;
	app[4]=info.Force.Actuators.Actuator5;
	app[5]=info.Force.Actuators.Actuator6;
	this->ds_ang_tau.push_back(app);
	// i can write for the vis less often then the other op
	this->ang_tau.push( &(ds_ang_tau.back()) );

}

void kinova_status::ReadCartesian(GeneralInformations & info)
{
	std::vector<double> app(6);

	app[0]=info.Position.CartesianPosition.X;
	app[1]=info.Position.CartesianPosition.Y;
	app[2]=info.Position.CartesianPosition.Z;
	app[3]=info.Position.CartesianPosition.ThetaX;
	app[4]=info.Position.CartesianPosition.ThetaY;
	app[5]=info.Position.CartesianPosition.ThetaZ;
	this->ds_cart_pos.push_back(app);


	app[0]=info.Force.CartesianPosition.X;
	app[1]=info.Force.CartesianPosition.Y;
	app[2]=info.Force.CartesianPosition.Z;
	app[3]=info.Force.CartesianPosition.ThetaX;
	app[4]=info.Force.CartesianPosition.ThetaY;
	app[5]=info.Force.CartesianPosition.ThetaZ;
	this->ds_cart_f.push_back(app);
}

void kinova_status::ReadCurrents(GeneralInformations & info)
{
	std::vector<double> app(6);
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

void kinova_status::Read4Vis(std::vector<std::vector<double>* > & lastval)
{
	std::vector<double>* app;
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



