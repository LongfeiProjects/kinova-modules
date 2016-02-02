#include "robot.hpp"

void robot::ReadCurrentState(std::vector<State>& stat, std::vector<std::string> & type ){
	this->st->GetLastValue(stat,type);
}

void robot::StartLog(std::vector<std::string>  & type){
  this->st->StartSaving(type);
}
std::vector<Log> robot::StopLog(std::vector<std::string>  & type){
	std::vector<Log> result = this->st->StopSaving(type);
	return result;
}

void robot::UpdateFF(std::vector< std::vector<State> > new_ff){
    this->contr->SetNewFF(new_ff);
}

void robot::ExecuteTrajectory(std::vector<State> timestamps, State starting_cartesian_position){
    boost::chrono::high_resolution_clock::time_point time_reference;
    boost::chrono::milliseconds cur_time,control_time;
    boost::chrono::high_resolution_clock::time_point begin;
    int internal_clock = 0;
    // i have to grant access to the execution of the task
    this->stop.store(false,boost::memory_order_release);
    // i create the time map for each trajectory that i want to reproduce
    this->contr->ComputeTimeMap(timestamps);
    try{
        contr->index = -1; // in this way i define the initialization of the controller contr->index = -1
        bool read_data = false;
        while( !this->stop.load(boost::memory_order_acquire) && internal_clock < this->contr->time_map.size()){
            std::vector<State> cur_val;
            begin = boost::chrono::high_resolution_clock::now();
            read_data =st->GetLastValue(cur_val,this->contr->opt.meas_val);
            // control block
            if(read_data){
                if(contr->index == -1){
                    //DEBUG
                    std::cout<<"before move2startingposition"<<std::endl;
                    //---
                    SendAndWait(starting_cartesian_position);
                    std::vector<State> start;
                    start = st->FirstRead(this->contr->opt.meas_val);
                    //DEBUG
                    std::cout<<"after move2startingposition"<<std::endl;
                    //---
                    // Initialization of current value after after move2home
                    cur_val = start;
                    contr->index = 0;
                    time_reference = boost::chrono::high_resolution_clock::now();
                }
                else if(contr->index >= 0){
                	 contr->ExecController(cur_val,this->contr->opt.control_action);
                    control_time = boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - begin);
                    std::cout << "time spent in main cycle: " << control_time.count()  << " ms\n";
                    int test_time = boost::chrono::round<boost::chrono::milliseconds>(control_time).count();
                    if(test_time < 10){
                        boost::this_thread::sleep(boost::posix_time::milliseconds(10-test_time));
                    }
                    cur_time = boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - time_reference);
                    internal_clock = boost::chrono::round<boost::chrono::milliseconds>(cur_time).count();
                    std::cout << "internal clock " << internal_clock << std::endl;
                    std::cout << "index " << contr->index << std::endl;
                    if(internal_clock < this->contr->time_map.size()){
                        contr->index = this->contr->time_map[internal_clock];
                    }
                }
            }
        }
        std::cout<< "im out the main cycle"<<std::endl;
    }
    catch(const std::exception &e){
        std::cout<< "something gone wrong"<<std::endl;
        return;
    }
}

void robot::ExecuteUpdatedTrajectory(std::vector<State> timestamps, State starting_cartesian_position, std::vector<std::vector<State> > new_ff){
    this->UpdateFF(new_ff);
    this->ExecuteTrajectory(timestamps,starting_cartesian_position);
}

void robot::ExecuteTrajectoryFile(State starting_cartesian_position){
    std::vector<State> timestamps;
    if(!this->contr->timestamp_file.empty()){
        this->contr->ReadFile(this->contr->timestamp_file,timestamps);
    }
    for(unsigned int i =0;i<this->contr->ff_files.size();i++){
        std::vector<State> app;
        this->contr->ReadFile(this->contr->ff_files[i],app);
        this->contr->ff.push_back(app);
    }
    this->ExecuteTrajectory(timestamps, starting_cartesian_position);
}

void robot::SendCommand(State & cmd,int type){
	//std::cout << "before SendSingleCommand" << std::endl;
	this->contr->SendSingleCommand(cmd,type);
}

void robot::SendAndWait(State starting_joint_position){
	std::cout << "entering sendAndWait" << std::endl;
    // GO TO initial position
    double sos;
    std::vector<State> cur_val_1;
    std::string app = "j_vel";
    std::vector<std::string> read;
    read.push_back(app);
    std::cout << "before send command" << std::endl;
    this->SendCommand(starting_joint_position,21);
    std::cout << "after send command" << std::endl;
    usleep(1000*300);
    st->GetLastValue(cur_val_1,read);
    sos =arma::dot(cur_val_1[0],cur_val_1[0]);
    //std::cout<< "curval" << cur_val_1[0] << std::endl;
    std::cout<< sos << std::endl;
   while(sos > 0.000001){
        usleep(10*1000);
        std::vector<State> cur_val_1;
        st->GetLastValue(cur_val_1,read);
        sos =arma::dot(cur_val_1[0],cur_val_1[0]);
        //std::cout<< "curval" << cur_val_1[0] << std::endl;
        //std::cout<< sos << std::endl;
   }
   // DEBUG
   std::string app2 = "cart_pos";
   std::vector<std::string> read2;
   read2.push_back(app2);
   std::vector<State> cur_val_2;
   st->GetLastValue(cur_val_2,read2);
   for(unsigned int i=0;i<cur_val_2.size();i++)
       std::cout<< cur_val_2[i] << std::endl;
   //----

}
// this functions are provisionary and has to be intregrated inside send command for the vrep driver.
// send and wait;
void robot::ReproduceTrajectory(std::string namefile){
	std::vector<State> value;
	ReadFile(namefile,value);
	for(unsigned int i =1 ;i<value.size();i++){
		std::cout << i << std::endl;
		this->SendCommand(value[i],contr->opt.control_action);
	}
}
// send and wait
/*void robot::SendCartesianPositionCommand(State & cmd){
	std::vector<State> cur_val;
	State diff,control_action;
	double sos = 1;
	this->SendCommand(cmd,contr->opt.control_action);
	while(sos > 0.00001){
		st->GetLastValue(cur_val,contr->opt.meas_val);
		sos =arma::dot(cur_val[2],cur_val[2]);
		std::cout<< cur_val[2] <<std::endl;
		std::cout << sos << std::endl;
	}
}*/
void robot::SendDeltaCartesianCommand(State & cmd){
	//std::cout << "inizio controller"<< std::endl;
	//bool read_data = false;
	std::vector<State> cur_val;
	State control_action;
	st->GetLastValue(cur_val,this->contr->opt.meas_val);
	// DEBUG
	//std::cout<<"joint "<<cur_val[0]<<std::endl;
	//std::cout<<"cartesian position"<<cur_val[1]<<std::endl;
	//std::cout<<"cmd 2 "<<cmd<<std::endl;
	//----
	control_action = this->contr->DirectDifferentialKinematicControl(cur_val,cmd,"CartPosDelta");
	this->SendCommand(control_action,contr->opt.control_action);
	//std::cout << "fine controller"<< std::endl;
}
void robot::MoveHome(){
	this->st->Stop();
    this->st->RestartAPI();
	this->contr->Move2Home();
    this->st->Start();
}

void robot::Stop(){
	this->st->ClearCommands();
    this->stop.store(true,boost::memory_order_release);
}

void robot::StartAllThread(){
	// start the thread in the object robot_status object
	st->Start();
    this->emergency_stop = new boost::thread(boost::bind(&robot::EmergencyStop,this));
	// start the thread in the robot object
    if(check.launch_thread){
    	std::cout<<"start checking thread"<<std::endl;
        this->safety_check = new boost::thread(boost::bind(&robot::Cheking,this));
    }
}
void robot::StopAllThread(){
	st->Stop();
	this->stop_auxiliary_thread.store(true,boost::memory_order_release);
    emergency_stop->join();
    if(check.launch_thread)
        safety_check->join();
}
void robot::Cheking(){
    std::cout<< "starting cheking thread"<<std::endl;
	try{
		while( !this->stop_auxiliary_thread.load(boost::memory_order_acquire) ){
			std::vector<State> cur_val;
			bool read_data = false;
			read_data = st->GetLastValue(cur_val,check.checklist);
			// if the thread that publish data start to write data i will start to check them
			if(read_data){
				this->check.VerifyViolation(cur_val);
			}
             boost::this_thread::sleep(boost::posix_time::milliseconds(5));
		}
		std::cout<< "im out the the Cheking thread"<<std::endl;
	}
	catch(const std::exception &e){
		std::cout<< "error in the checking thread"<<std::endl;
	}
}
void robot::EmergencyStop(){
	// cycle of control to keep the robot in action
	sf::RenderWindow window(sf::VideoMode(800, 600, 32), "Joystick Use", sf::Style::Default);
	sf::Event e;
	// window to control robot
	sf::RectangleShape square;
	square.setFillColor(sf::Color(255, 0, 0, 255));
	square.setPosition(window.getSize().x / 2, window.getSize().y / 2);
	square.setOutlineColor(sf::Color(0, 0, 0, 255));
	square.setSize(sf::Vector2f(50.f, 50.f));
	// joystick
	//query joystick for settings if it's plugged in...
	if (sf::Joystick::isConnected(0)){
		// check how many buttons joystick number 0 has
		unsigned int buttonCount = sf::Joystick::getButtonCount(0);
		std::cout << "Button count: " << buttonCount << std::endl;
	}
	while( !this->stop_auxiliary_thread.load(boost::memory_order_acquire) ){
		while (window.pollEvent(e)){
			if (sf::Joystick::isButtonPressed(0, 2)){//X = undo command
				std::cout<< "---------------------------------------------------------"<<std::endl;
				this->Stop();
			}
		}
	}
	window.close();
}
