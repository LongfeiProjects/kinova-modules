#include "kinova_status_openAPI.hpp"

kinova_status_openapi::kinova_status_openapi(model * mdl)
: running(true)
, running_cleaner(true)
, first_write(false)
, ang_pos(0)
, ang_tau(0)
, cart_f(0)
, mot_amp(0)
, comp_t(0){
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
	already_saving = false;
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
    // start the global time for logging
    this->tStart = boost::chrono::high_resolution_clock::now();
}

kinova_status_openapi::~kinova_status_openapi(){
    // close open api kinova
    this->arm->stop_api_ctrl();
    KinDrv::close_usb();
}

void kinova_status_openapi::Start(){
    this->running.store(true,boost::memory_order_release);
    this->running_cleaner.store(true,boost::memory_order_release);
    this->reader_stats = new boost::thread(boost::bind(&kinova_status_openapi::Reading,this));
	//this->log_stats = new boost::thread(boost::bind(&kinova_status::Logging,this));
    this->garbage_collection = new boost::thread(boost::bind(&kinova_status_openapi::Cleaning,this));
    std::cout<<"start all threads"<<std::endl;
}

void kinova_status_openapi::Stop(){
	// maybe add the command to empty the stack of command
	// and to put the velocity at zero
	this->arm->erase_trajectories();
	this->running.store(false,boost::memory_order_release);
	this->running_cleaner.store(false,boost::memory_order_release);
    this->reader_stats->join();
	//this->log_stats->join();
    this->garbage_collection->join();
	std::cout<<"close all thread"<<std::endl;
}

// KINOVA API DEPENDANT // // in reading i update the value for control
void kinova_status_openapi::Reading(){
    std::cout<<"starting reading thread"<< std::endl;
	boost::chrono::milliseconds reading_time;
	while(this->running.load(boost::memory_order_acquire)){
		boost::chrono::high_resolution_clock::time_point global_begin = boost::chrono::high_resolution_clock::now();
        KinDrv::jaco_position_t position,velocity,force;
        velocity = this->arm->get_ang_vel();
		position = this->arm->get_ang_pos();
        force = this->arm->get_ang_force();
        this->ReadTimeStamp();
        this->ReadJoints(position,velocity,force);
        this->ReadCartesian(position);
		if(!first_write.load(boost::memory_order_acquire)){
			std::cout<<"first write"<<std::endl;
			first_write.store(true,boost::memory_order_release);
		}
        //DEBUG
        /*KinDrv::jaco_retract_mode_t mode = arm->get_status();
        switch( mode ) {
            case KinDrv::MODE_READY_TO_RETRACT:
                std::cout<<"MODE_READY_TO_RETRACT"<<std::endl;
            break;
            case KinDrv::MODE_READY_STANDBY:
                std::cout<<"MODE_READY_STANDBY"<<std::endl;
            break;
            case KinDrv::MODE_RETRACT_TO_READY:
                std::cout<<"MODE_RETRACT_TO_READY"<<std::endl;
            break;
            case KinDrv::MODE_NORMAL_TO_READY:
                std::cout<<"MODE_NORMAL_TO_READY"<<std::endl;
            break;
            case KinDrv::MODE_NORMAL:
                std::cout<<"MODE_NORMAL"<<std::endl;
            break;
            case KinDrv::MODE_NOINIT:
                std::cout<<"MODE_NOINIT"<<std::endl;
            break;
            case KinDrv::MODE_ERROR:
                std::cout<<"MODE_ERROR"<<std::endl;
            break;
            case KinDrv::MODE_RETRACT_STANDBY:
                std::cout<<"MODE_RETRACT_STANDBY"<<std::endl;
            break;
        }*/
        //-----
		reading_time = boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - global_begin);
    	//std::cout << "time spent Reading: " << reading_time.count() << " ms\n";
        int test_time = boost::chrono::round<boost::chrono::milliseconds>(reading_time).count();
        if(test_time < 10){
            boost::this_thread::sleep(boost::posix_time::milliseconds(10-test_time));
        }
	}
	std::cout<<"im out of Reading thread"<<std::endl;
}

// in logging i update the value for logging and visualization
void kinova_status_openapi::Logging(){
	while(this->running.load(boost::memory_order_acquire)){
		std::vector<State_ptr> visvec;
		int readckeck=this->Read4Vis(visvec);
		if(readckeck){
			vis.Update(visvec);
			vis.Plot();
		}
	}
	std::cout<<"im out of Logging thread"<<std::endl;
}

void kinova_status_openapi::Cleaning(){
    std::cout<<"start cleaning thread "<<std::endl;
	while(this->running_cleaner.load(boost::memory_order_acquire)){
        if(this->ds_ang_pos.size() > (unsigned int)(this->Max_DS_allowed) ){
           this->ds_ang_pos.pop_front();
        }
        if(this->ds_hand_pos.size() > (unsigned int)(this->Max_DS_allowed) ){
           this->ds_hand_pos.pop_front();
        }
        if(this->ds_ang_vel.size() > (unsigned int)(this->Max_DS_allowed)){
           this->ds_ang_vel.pop_front();
        }
        if(this->ds_hand_vel.size() > (unsigned int)(this->Max_DS_allowed)){
           this->ds_hand_vel.pop_front();
        }
        if(this->ds_ang_tau.size() > (unsigned int)(this->Max_DS_allowed)){
           this->ds_ang_tau.pop_front();
        }
        if(this->ds_cart_pos.size() > (unsigned int)(this->Max_DS_allowed)){
           this->ds_cart_pos.pop_front();
        }
        if(this->ds_comp_t.size() > (unsigned int)(this->Max_DS_allowed)){
           this->ds_comp_t.pop_front();
        }
    }
	std::cout<<"im out of Cleaning thread"<<std::endl;
}

void kinova_status_openapi::StartSaving(std::vector<std::string>  & type){
	// first i have to stop the garbage collector process
	this->running_cleaner.store(false,boost::memory_order_release);
    this->garbage_collection->join();
    // start the global time for logging
    //this->tStart = boost::chrono::high_resolution_clock::now();
    for(unsigned int ii =0;ii<type.size();ii++){
		std::vector<DataStoreIt> init;
		this->bookmarks.push_back(init);
	}
    std::cout<<"starting saving"<<std::endl;
	if(first_write.load(boost::memory_order_acquire)){
		for(unsigned int i =0;i<type.size();i++){
            DataStoreIt app;
			if(type[i].compare("comp_t")==0){
                app = this->ds_comp_t.end();
                app--;
			}else if(type[i].compare("j_pos") == 0){
                app = this->ds_ang_pos.end();
				app--;
			}else if(type[i].compare("hand_pos") == 0){
                app = this->ds_hand_pos.end();
                app--;
            }else if(type[i].compare("j_vel") == 0){
				app = this->ds_ang_vel.end();
				app--;
			}else if(type[i].compare("hand_vel") == 0){
                app = this->ds_hand_vel.end();
                app--;
            }else if(type[i].compare("j_tau") == 0){
				app = this->ds_ang_tau.end();
				app--;
			}else if(type[i].compare("cart_f") == 0){
				app = this->ds_cart_f.end();
				app--;
			}else if(type[i].compare("cart_pos") == 0){
				app = this->ds_cart_pos.end();
				app--;
			}
			this->bookmarks[i].push_back(app);
		}
		this->active_bookmarks.push_back(1);
		this->already_saving = true;
	 }
}
void kinova_status_openapi::SaveCheckPoint(std::vector<std::string>  & type){
	if(already_saving){
		for(unsigned int i =0;i<type.size();i++){
			DataStoreIt app;
			if(type[i].compare("comp_t")==0){
				app = this->ds_comp_t.end();
				app--;
			}else if(type[i].compare("j_pos") == 0){
				app = this->ds_ang_pos.end();
				app--;
			}else if(type[i].compare("hand_pos") == 0){
				app = this->ds_hand_pos.end();
				app--;
			}else if(type[i].compare("j_vel") == 0){
				app = this->ds_ang_vel.end();
				app--;
			}else if(type[i].compare("hand_vel") == 0){
				app = this->ds_hand_vel.end();
				app--;
			}else if(type[i].compare("j_tau") == 0){
				app = this->ds_ang_tau.end();
				app--;
			}else if(type[i].compare("cart_f") == 0){
				app = this->ds_cart_f.end();
				app--;
			}else if(type[i].compare("cart_pos") == 0){
				app = this->ds_cart_pos.end();
				app--;
			}
			this->bookmarks[i].push_back(app);
		}
		this->active_bookmarks.push_back(1);
	}
}

void kinova_status_openapi::DeleteCheckPoint(){
	if(already_saving){
		for(unsigned int i = (active_bookmarks.size()-1);i>0;i--){
			if(active_bookmarks[i] == 1 && i>= 0){
				active_bookmarks[i]=0;
				break;
			}
		}
	}
}

std::vector<Log> kinova_status_openapi::StopSaving(std::vector<std::string>  & type){
	std::vector<Log> result;
	std::vector<std::pair<int,int> > seq;
    // stop the reading thread
    this->running.store(false,boost::memory_order_release);
    this->reader_stats->join();
    // save the last positions
    this->SaveCheckPoint(type);
    // analisys active_bookmarks
    seq = ContSeq(this->active_bookmarks);
    // DEBUG
		for(unsigned int i =0;i<this->bookmarks.size();i++){
			std::cout << type[i] << "  = ";
			DataStoreIt first_index_of_seq = bookmarks[i][0];
			int index = 0;
			for(unsigned int j = 1;j<this->bookmarks[i].size();j++){
				DataStoreIt it = bookmarks[i][j];
				while(first_index_of_seq != it){
					index++;
					first_index_of_seq++;
				}
				std::cout<< index << " ";
			}
			std::cout<<std::endl;
		}
		std::cout<<" this->active_bookmarks "<<std::endl;
		for(unsigned int i =0;i<this->active_bookmarks.size();i++)
			std::cout<< this->active_bookmarks[i];
		std::cout<<std::endl;
		std::cout<<"seq"<<std::endl;
		for(unsigned int i =0;i<seq.size();i++)
			std::cout<< seq[i].first << " " << seq[i].second<< std::endl;
    //---

	for(unsigned int i =0;i<type.size();i++){
		if(type[i].compare("comp_t")==0){
			Log app_tot;
			State time_displacement(1);
			time_displacement[0]=0;
			for(unsigned int j = 0;j<seq.size();j++){
				Log app(this->bookmarks[i][seq[j].first],this->bookmarks[i][seq[j].second]);
				State correction = app[0];
				std::cout<< correction << std::endl;
				std::cout<<time_displacement << std::endl;
				//for(unsigned int i=0;i<app.size();i++)
				//	std::cout << app[i] << std::endl;
				for(unsigned int i =0;i<app.size();i++){
					app[i]=(app[i]-correction) + time_displacement;
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
		}else if(type[i].compare("hand_pos") == 0){
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
		}else if(type[i].compare("hand_vel") == 0){
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
		}else if(type[i].compare("cart_f") == 0){
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
    this->reader_stats = new boost::thread(boost::bind(&kinova_status_openapi::Reading,this));
	// reactivate the cleaner tasks
	this->running_cleaner.store(true,boost::memory_order_release);
    this->garbage_collection = new boost::thread(boost::bind(&kinova_status_openapi::Cleaning,this));
    // clean bookmarks and active_bookmarks
	this->bookmarks.clear();
	this->active_bookmarks.clear();
    this->already_saving = false;
    std::cout << "before return stop" << std::endl;
	return result;
}

// FROM THIS POINT ALL THE FUNCTIONS are KINOVA API DEPENDANT //

void kinova_status_openapi::ClearCommands()
{
	this->arm->erase_trajectories();
}

void kinova_status_openapi::RestartAPI(){
    arm->stop_api_ctrl();
    arm->start_api_ctrl();
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
}

void kinova_status_openapi::ReadJoints(KinDrv::jaco_position_t &position,KinDrv::jaco_position_t & velocity,KinDrv::jaco_position_t & force)
{
	// joint position
    State app(6);
	app[0]=position.joints[0];
	app[1]=position.joints[1];
	app[2]=position.joints[2];
	app[3]=position.joints[3];
	app[4]=position.joints[4];
	app[5]=position.joints[5];
	// convert angle from deg to rad
	app=app*DEG;
	this->ds_ang_pos.push_back(app);
	this->dl_ang_pos.store( &(ds_ang_pos.back()),boost::memory_order_release);
	// i can write for the vis less often then the other op
	this->ang_pos.push( &(ds_ang_pos.back()) );
    // finger position
    State app_fing(3);
    app_fing[0]=position.finger_position[0];
    app_fing[1]=position.finger_position[1];
    app_fing[2]=position.finger_position[2];
    //std::cout<<app_fing<<std::endl;
    this->ds_hand_pos.push_back(app_fing);
    this->dl_hand_pos.store(&(ds_hand_pos.back()),boost::memory_order_release);
    // joint velocity
    State app_short(6);
    app_short[0]=velocity.joints[0];
    app_short[1]=velocity.joints[1];
    app_short[2]=velocity.joints[2];
    app_short[3]=velocity.joints[3];
    app_short[4]=velocity.joints[4];
    app_short[5]=velocity.joints[5];
    // convert angle from deg to rad
    app_short=app_short*DEG;
    //std::cout<<app_short << std::endl;
    this->ds_ang_vel.push_back(app_short);
	this->dl_ang_vel.store(&(ds_ang_vel.back()),boost::memory_order_release);
    // finger velocity. For the finger velocity because there is a difference between the
    // measured and the commanded value we can only rely on the sign of the misure.
    State app_short_fing(3);
    for(int i=0;i<3;i++)
    {
         if(velocity.finger_position[i]>0.00001)
            app_short_fing[i]= 30.0;
         else if(velocity.finger_position[i]<-0.00001)
            app_short_fing[i]= -30.0;
         else
            app_short_fing[i]= 0.0;
    }
    //std::cout<<app_short_fing<<std::endl;
    this->ds_hand_vel.push_back(app_short_fing);
    this->dl_hand_vel.store(&(ds_hand_vel.back()),boost::memory_order_release);
	// joint torques
    app[0]=force.joints[0];
	app[1]=force.joints[1];
	app[2]=force.joints[2];
	app[3]=force.joints[3];
	app[4]=force.joints[4];
	app[5]=force.joints[5];
	std::cout<<app<<std::endl;
	this->ds_ang_tau.push_back(app);
	this->dl_ang_tau.store( &(ds_ang_tau.back()),boost::memory_order_release);
	// i can write for the vis less often then the other op
    this->ang_tau.push( &(ds_ang_tau.back()) );
}

// TO DO: SPLIT CARTESIAN POSITION FROM ORIENTATION INTO DIFFERENT VARIABLE
void kinova_status_openapi::ReadCartesian(KinDrv::jaco_position_t & position)
{
	// cartesian position
    State q(6),cart_pos(6);
    State pos,rpy(3);
    arma::mat R;
    q[0]=position.joints[0];
    q[1]=position.joints[1];
    q[2]=position.joints[2];
    q[3]=position.joints[3];
    q[4]=position.joints[4];
    q[5]=position.joints[5];

	// convert angle from deg to rad
	q=q*DEG;
    bot->DK(q,pos,R);
    Mat2RPY(R,rpy);

    cart_pos[0]=pos[0];
    cart_pos[1]=pos[1];
    cart_pos[2]=pos[2];
    cart_pos[3]=rpy[0];
    cart_pos[4]=rpy[1];
    cart_pos[5]=rpy[2];

    this->ds_cart_pos.push_back(cart_pos);
    this->dl_cart_pos.store( &(ds_cart_pos.back()),boost::memory_order_release);
}


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
    //std::cout<<"FirstRead result"<<result<<std::endl;
	//---
	return res;

}


bool kinova_status_openapi::GetLastValue(std::vector<State>& res, std::vector<std::string>  & type)
{
	if(first_write.load(boost::memory_order_acquire))
	{
		for(unsigned int i =0;i<type.size();i++)
		{
            //DEBUG
            //std::cout<<"reading succed"<<std::endl;
            //--
			State app;
			if(type[i].compare("j_pos") == 0)
            {
				app = *(this->dl_ang_pos.load(boost::memory_order_acquire));
			}
            else if(type[i].compare("hand_pos") == 0)
            {
                app = *(this->dl_hand_pos.load(boost::memory_order_acquire));
            }
			else if(type[i].compare("j_vel") == 0)
			{
				app = *(this->dl_ang_vel.load(boost::memory_order_acquire));
			}
            else if(type[i].compare("hand_vel") == 0)
            {
                app = *(this->dl_hand_vel.load(boost::memory_order_acquire));
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

