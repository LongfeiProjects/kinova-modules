#include "robot.hpp"


int main()
{
	// STATUS READER
	/*Jaco* mdl = new Jaco();
	kinova_status_openapi * st= new kinova_status_openapi(mdl);
	// CONTROLLER
    Option opt;
    opt.type_of_controller = "CartesianKinematic";
    opt.control_action = 28;
	const double Pid_coef[] = {5,0,0}; // deg
	std::vector<double> Pid(Pid_coef,End(Pid_coef));
	const char * _name2read[] = {"j_pos","j_vel"};
    std::vector<std::string> name2read (_name2read,End(_name2read));
    opt.meas_val = name2read;
	const char * _namefiles[] = {"cart_pos.txt","joint_vel.txt"};
	std::vector<std::string> namefile (_namefiles,End(_namefiles));
	Jaco* md = new Jaco();
    kinova_controller_openapi * ct = new kinova_controller_openapi(namefile,"index_openapi.mat",opt,Pid,md,st->arm);
	// CHECKING MODULE
	// define bounding box
	const double bb_point[] = {-0.6,-0.8,-0.4};
    const double bb_dims[]  = {1.2,1.6,0.8};
    std::vector<double> bb_p(bb_point,End(bb_point)),bb_d(bb_dims,End(bb_dims));
	// define all the limit
	const char *cl[] = {"j_pos","j_tau"};
	std::vector<std::string> chekclist(cl,End(cl));
	const double joint_min[] = {-10000,47,19,-10000,-10000,-10000}; // deg
	const double joint_max[] = {10000,313,341,10000,10000,10000}; // deg
	const double tau_min[] = {-15}; // Nm
	const double tau_max[] = {15};  // nm
	std::vector<double> j_min(joint_min,End(joint_min)),j_max(joint_max,End(joint_max));
	std::vector<double> t_min(tau_min,End(tau_min)),t_max(tau_max,End(tau_max));
	std::vector<std::vector<double> > l_down_left_corner,l_dims,l_min,l_max;
	l_down_left_corner.push_back(bb_p);l_dims.push_back(bb_d);
	l_min.push_back(j_min);l_min.push_back(t_min);
	l_max.push_back(j_max)
	safetycheck checker(l_down_left_corner,l_dims,l_min,l_max,chekclist);
	robot bot(st,ct,checker); */

	// STATUS READER
	// model
	Lwr* mdl = new Lwr();
	//Jaco* mdl = new Jaco();
	bool sync = false;
	std::string joint_base_name = "LBR4p_joint";
	driverbot * vrep = new driverbot(sync,joint_base_name,mdl);
	// CONTROLLER
    Option opt;
    opt.type_of_controller = "DirectCartesian";
    opt.control_action = 2;
   	const double Pid_coef[] = {5,0,0};
   	std::vector<double> Pid(Pid_coef,End(Pid_coef));
	const char * _name2read[] = {"j_pos","cart_pos","j_vel","comp_t"};
    std::vector<std::string> name2read (_name2read,End(_name2read));
    opt.meas_val = name2read;
   	// FF files
    const char * _namefiles[] = {"joint_pos.mat","cart_pos.mat","comp_t.mat"};
    std::vector<std::string> namefile(_namefiles,End(_namefiles));
    // model
	Lwr* md = new Lwr();
	//Jaco* md = new Jaco();
    driverbot_contr * ct = new driverbot_contr(namefile,opt,Pid,md,vrep->idclient,vrep->joint_handle);
    // CHECKING MODULE
	// define bounding box
	const double bb_point[] = {-0.6,-0.8,-0.4};
	const double bb_dims[]  = {1.2,1.6,0.8};
	std::vector<double> bb_p(bb_point,End(bb_point)),bb_d(bb_dims,End(bb_dims));
	// define all the limit
	const char *cl[] = {"j_pos","j_tau"};
	std::vector<std::string> chekclist(cl,End(cl));
	const double joint_min[] = {-2*PI,47*DEG,19*DEG,-2*PI,-2*PI,-2*PI,-2*PI}; // rad
	const double joint_max[] = {2*PI,313*DEG,341*DEG,2*PI,2*PI,2*PI,2*PI}; // rad
	const double tau_min[] = {-25}; // Nm
	const double tau_max[] = {25};  // nm
	std::vector<double> j_min(joint_min,End(joint_min)),j_max(joint_max,End(joint_max));
	std::vector<double> t_min(tau_min,End(tau_min)),t_max(tau_max,End(tau_max));
	// construct range and bb objects
	std::vector<std::vector<double> > l_down_left_corner,l_dims,l_min,l_max;
	l_down_left_corner.push_back(bb_p);l_dims.push_back(bb_d);
	l_min.push_back(j_min);l_min.push_back(t_min);
	l_max.push_back(j_max);l_max.push_back(t_max);
	// create checker
	safetycheck checker(l_down_left_corner,l_dims,l_min,l_max,chekclist);
	// activate checker
	checker.launch_thread = true;
	robot bot(vrep,ct,checker);
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
	State cmd;
	Log checkpoints; // sequence of joints positions
	std::vector<Log> logs;
	double speed = 0.06;
	bool continuos_mov = false;
	bool start_save = true;
	const char * _name2save[] = {"j_pos","cart_pos","comp_t"};
	std::vector<std::string> name2save(_name2save,End(_name2save));
	// check point mechanism
	const char * _name2check[] = {"j_pos"};
		std::vector<std::string> name2check(_name2check,End(_name2check));
	while(true){

		while (window.pollEvent(e)){
			if (e.type == sf::Event::KeyPressed){
				switch (e.key.code){
					case sf::Keyboard::Escape:{
						 window.close();
						 return 0;
					}
						break;
					default:
						break;
				}
			}
			if (e.type == sf::Event::JoystickMoved){
				double x = sf::Joystick::getAxisPosition(0,sf::Joystick::X);
				double y = sf::Joystick::getAxisPosition(0,sf::Joystick::Y);
				double u = sf::Joystick::getAxisPosition(0,sf::Joystick::U);
				std::cout << "X axis: " << x << std::endl;
				std::cout << "Y axis: " << y << std::endl;
				std::cout << "U axis: " << u << std::endl;
				if(y == -100){
					std::cout<<"Up"<<std::endl;
					cmd = convertDirectionToState("Up",speed);
					// check point mechanism
					std::vector<State> check;
					bot.st->GetLastValue(check,name2check);
					bot.st->SaveCheckPoint(name2save); // ---
					if(!start_save)
						checkpoints.push_back(check[0]);
					// end
					continuos_mov = true;
				}else if(y == 100){
					std::cout<<"Down"<<std::endl;
					// check point mechanism
					std::vector<State> check;
					bot.st->GetLastValue(check,name2check);
					bot.st->SaveCheckPoint(name2save); // ---
					if(!start_save)
						checkpoints.push_back(check[0]);
					// compute command
					cmd = convertDirectionToState("Down",speed);
					continuos_mov = true;
				}if(x == 100){
					std::cout<<"Right"<<std::endl;
					// check point mechanism
					std::vector<State> check;
					bot.st->GetLastValue(check,name2check);
					bot.st->SaveCheckPoint(name2save); // ---
					if(!start_save)
						checkpoints.push_back(check[0]);
					// compute command
					cmd = convertDirectionToState("Right",speed);
					continuos_mov = true;
				}else if(x == -100){
					std::cout<<"Left"<<std::endl;
					// check point mechanism
					std::vector<State> check;
					bot.st->GetLastValue(check,name2check);
					bot.st->SaveCheckPoint(name2save); // ---
					if(!start_save)
						checkpoints.push_back(check[0]);
					// compute command
					cmd = convertDirectionToState("Left",speed);
					continuos_mov = true;
				}else if(u == 100){
					std::cout<<"Pull"<<std::endl;
					// check point mechanism
					std::vector<State> check;
					bot.st->GetLastValue(check,name2check);
					bot.st->SaveCheckPoint(name2save); // ---
					if(!start_save)
						checkpoints.push_back(check[0]);
					// compute command
					cmd = convertDirectionToState("Pull",speed);
					continuos_mov = true;
				}else if(u == -100){
					std::cout<<"Push"<<std::endl;
					// check point mechanism
					std::vector<State> check;
					bot.st->GetLastValue(check,name2check);
					bot.st->SaveCheckPoint(name2save); // ---
					if(!start_save)
						checkpoints.push_back(check[0]);
					// compute command
					cmd = convertDirectionToState("Push",speed);
					continuos_mov = true;
				}else if(x == 0 && y == 0 && u==0){
					std::cout<<"continuos_mov = false;"<<std::endl;
					continuos_mov = false;
				}
			}
			if (sf::Joystick::isButtonPressed(0, 0)){//triangle = start/stop logging
				std::cout << "sf::Joystick::isButtonPressed(0, 0)" << std::endl;
				if(start_save){
					bot.st->StartSaving(name2save);
					start_save = false;
				}
				else if(!start_save){
					logs = bot.st->StopSaving(name2save);
					WriteLogFiles(logs,namefile); // i just copy the sequence of joint positions
					start_save = true;
					checkpoints.clear();
				}
			}
			if(sf::Joystick::isButtonPressed(0,1)){//circle = execute last trajectory
				std::cout << "sf::Joystick::isButtonPressed(0,1)" << std::endl;
				bot.ReproduceTrajectory(namefile[0]);
			}
			if (sf::Joystick::isButtonPressed(0, 2)){//X = undo command
				std::cout << "sf::Joystick::isButtonPressed(0, 2)" << std::endl;
				if(checkpoints.size()>0){
					State actualPosition = checkpoints.back();
					checkpoints.pop_back();
					bot.st->DeleteCheckPoint(); // ---
					bot.SendCommand(actualPosition,bot.contr->opt.control_action);

				}
			}
			if (sf::Joystick::isButtonPressed(0, 3)){//square // show scoring
				std::cout << "sf::Joystick::isButtonPressed(0, 3)" << std::endl;
				std::vector<int> score = bot.check.GetScore();
				std::cout<<"current score"<<std::endl;
				for(unsigned int i =0;i<score.size();i++)
					std::cout<<bot.check.checklist[i]<<" = "<< score[i] <<" ";
				std::cout<<std::endl;
			}
			if (sf::Joystick::isButtonPressed(0, 4)){//L2
				std::cout << "sf::Joystick::isButtonPressed(0, 4)" << std::endl;
			}
			if (sf::Joystick::isButtonPressed(0, 5)){//R2
				std::cout << "sf::Joystick::isButtonPressed(0, 5)" << std::endl;
			}
			if (sf::Joystick::isButtonPressed(0, 6)){//L1
				std::cout << "sf::Joystick::isButtonPressed(0, 6)" << std::endl;
			}
			if (sf::Joystick::isButtonPressed(0, 7)){//R1
				std::cout << "sf::Joystick::isButtonPressed(0, 7)" << std::endl;
			}
			if (sf::Joystick::isButtonPressed(0, 8)){//select
				std::cout << "sf::Joystick::isButtonPressed(0, 8)" << std::endl;
			}
			if (sf::Joystick::isButtonPressed(0, 9)){//start
				std::cout << "sf::Joystick::isButtonPressed(0, 9)" << std::endl;
				window.close();
				return 0;
			}
		}
		// auto fire
		if(continuos_mov){
			bot.SendDeltaCartesianCommand(cmd);
		}

	}
	/* Clean up */
	return 1;
}

