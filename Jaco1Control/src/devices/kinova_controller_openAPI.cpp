/*
 * kinova_conotroller.hpp
 *
 *  Created on: May 11, 2015
 *      Author: vale
 */

#include "kinova_controller_openAPI.hpp"
//private function //

KinDrv::jaco_position_type_t kinova_controller_openapi::InitPositionType(int value)
{
	KinDrv::jaco_position_type_t p;
	if(value == 1)
		p = KinDrv::POSITION_CARTESIAN;
	else if(value == 2)
		p = KinDrv::POSITION_ANGULAR;
	else if(value == 7)
		p = KinDrv::SPEED_CARTESIAN;
	else if(value == 8)
	{
		p = KinDrv::SPEED_ANGULAR;
		std::cout<<"speed_angular"<<std::endl;
	}

	return p;
}
// public function//
kinova_controller_openapi::kinova_controller_openapi()
{}
kinova_controller_openapi::kinova_controller_openapi(std::vector<std::string> namefile,std::vector<std::string> list_meas_value,
										  std::vector<double> Pid,int _controltype,bool _limitation,model* mdl,KinDrv::JacoArm *arm_)
{
	arm = arm_;
	this->P = Pid[0];
	this->I = Pid[1];
	this->D = Pid[2];
	index = -2; // for accessing inizialization procedure
	this->time_interval = 0.01; // second TO CHANGE
	this->measured_value = list_meas_value;
	controltype = this->InitPositionType(_controltype);
	limitation = _limitation;
	bot=mdl;
	for(unsigned int i =0;i<namefile.size();i++)
	{
		std::vector<State> app;
		this->ReadFile(namefile[i],app);
		ff.push_back(app);
	}


}
kinova_controller_openapi::~kinova_controller_openapi()
{}
int kinova_controller_openapi::Move2Home()
{
	//boost::recursive_mutex::scoped_lock scoped_lock(api_mutex);
	  // going to HOME position is possible from all positions. Only problem is,
	  // if there is some kinfo of error
	  arm->stop_api_ctrl();
	  arm->start_api_ctrl();
	  KinDrv::jaco_retract_mode_t mode = arm->get_status();
	  switch( mode ) {
	    case KinDrv::MODE_RETRACT_TO_READY:
	      // is currently on the way to HOME. Need 2 button presses,
	      // 1st moves towards RETRACT, 2nd brings it back to its way to HOME
	      arm->push_joystick_button(2);
	      arm->release_joystick();
	      arm->push_joystick_button(2);
	      break;

	    /*case KinDrv::MODE_NORMAL_TO_READY:
	    case KinDrv::MODE_READY_TO_RETRACT:
	    case KinDrv::MODE_RETRACT_STANDBY:
	    case KinDrv::MODE_NORMAL:*/
	    case KinDrv::MODE_NOINIT:
	      // just 1 button press needed
	      arm->push_joystick_button(2);
	      break;

	    case KinDrv::MODE_ERROR:
	      printf("some error?! \n");
	      return 0;
	      break;

	    case KinDrv::MODE_READY_STANDBY:
	      printf("nothing to do here \n");
	      return 1;
	      break;
	  }

	  while( mode != KinDrv::MODE_READY_STANDBY ) {
	    mode = arm->get_status();
	    switch( mode )
	    {
	        case KinDrv::MODE_NORMAL_TO_READY:
	          std::cout<<"MODE_NORMAL_TO_READY"<<std::endl;
	          break;
	        case KinDrv::MODE_READY_TO_RETRACT:
	           std::cout<<"MODE_READY_TO_RETRACT"<<std::endl;
	          break;
	        case KinDrv::MODE_RETRACT_STANDBY:
	           std::cout<<"MODE_RETRACT_STANDBY"<<std::endl;
	          break;
	        case KinDrv::MODE_NORMAL:
	           std::cout<<"MODE_NORMAL"<<std::endl;
	          break;
	        case KinDrv::MODE_NOINIT:
	           std::cout<<"MODE_NOINIT"<<std::endl;
	          break;
	          // just 1 button press needed
	          arm->push_joystick_button(2);
	          break;
	         case KinDrv::MODE_ERROR:
	          printf("some error?! \n");
	          return 0;
	          break;
	        }


	    if(mode == KinDrv::MODE_READY_TO_RETRACT  || mode == KinDrv::MODE_NORMAL || mode == KinDrv::MODE_RETRACT_STANDBY ) {
	      arm->release_joystick();
	      arm->push_joystick_button(2);
	    }
	  }
	  arm->release_joystick();

	  return 0;
}

KinDrv::jaco_basic_traj_point_t  kinova_controller_openapi::ConvertControl(State & value,int type)
{
	KinDrv::jaco_basic_traj_point_t pointToSend;
    KinDrv::jaco_position_type_t ty;
    // set the hand control type
    if(type > 10 && type < 20)
    {
        std::cout<<"MODE_POSITION" <<  std::endl;
		type = type - 10;
		pointToSend.hand_mode = KinDrv::MODE_POSITION;
	}
	else if(type>20)
	{
        std::cout<<"MODE_SPEED" <<  std::endl;
		type = type - 20;
		pointToSend.hand_mode = KinDrv::MODE_SPEED;
	}
	else
	{
        std::cout<<"NO_MOVEMENT" <<  std::endl;
		pointToSend.hand_mode = KinDrv::NO_MOVEMENT;
	}
    // set the robot control type
	if(type == -1)
	{
		pointToSend.pos_type = this->controltype;
        ty = this->controltype;
	}
	else
	{
		pointToSend.pos_type = this->InitPositionType(type);
        ty = this->InitPositionType(type);
	}
	pointToSend.time_delay = 0;




    if(ty==KinDrv::POSITION_ANGULAR || ty==KinDrv::SPEED_ANGULAR)
	{
        value=value/DEG;
		pointToSend.target.joints[0] = (float)value[0];
		pointToSend.target.joints[1] = (float)value[1];
		pointToSend.target.joints[2] = (float)value[2];
		pointToSend.target.joints[3] = (float)value[3];
		pointToSend.target.joints[4] = (float)value[4];
		pointToSend.target.joints[5] = (float)value[5];
        pointToSend.target.finger_position[0] = (float)value[6]*DEG;
        pointToSend.target.finger_position[1] = (float)value[7]*DEG;
        pointToSend.target.finger_position[2] = (float)value[8]*DEG;
	}
	else if(ty==KinDrv::POSITION_CARTESIAN || ty==KinDrv::SPEED_CARTESIAN)
	{
        std::cout << value << std::endl;
		pointToSend.target.position[0] = (float)value[0];
		pointToSend.target.position[1] = (float)value[1];
		pointToSend.target.position[2] = (float)value[2];
		//We set the orientation part of the position (unit is RAD)
		pointToSend.target.rotation[0]= (float)value[3];
		pointToSend.target.rotation[1] = (float)value[4];
		pointToSend.target.rotation[2] = (float)value[5];
		pointToSend.target.finger_position[0] = (float)value[6];
		pointToSend.target.finger_position[1] = (float)value[7];
		pointToSend.target.finger_position[2] = (float)value[8];
	}
	return pointToSend;
}
void kinova_controller_openapi::SendSingleCommand(State cmd,int type)
{

    //DEBUG
	boost::chrono::high_resolution_clock::time_point begin = boost::chrono::high_resolution_clock::now();
	//---
	KinDrv::jaco_basic_traj_point_t  p = this->ConvertControl(cmd,type);
	//DEBUG
	std::cout << "time spent ConvertControl: " << boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - begin).count() << " ms\n";
	begin = boost::chrono::high_resolution_clock::now();
	//---
	this->arm->erase_trajectories();
	this->arm->set_target(p);
	//DEBUG
	std::cout << "time spent MySendAdvanceTrajectory: " << boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - begin).count() << " ms\n";
	//----
}
bool kinova_controller_openapi::InitController(std::vector<State> initial_state)
{
	 //DEBUG
	std::cout<<"0.1"<<std::endl;
	//----
	//DEBUG
	std::cout<<"0.2"<<std::endl;
	//----
     this->InitCartesianKinematicController(initial_state);

	 //DEBUG
	std::cout<<"0.3"<<std::endl;
	//----
	 // this is really important! because in this way i can exit from initialization and start the execution of controller
	 return true;
}
bool kinova_controller_openapi::ExecController(std::vector<State> current_state,int type)
{

	//DEBUG
	//std::cout<<"executing controller"<<std::endl;
	//---
	// inibhit the repetition of this action
	boost::chrono::high_resolution_clock::time_point global_begin = boost::chrono::high_resolution_clock::now();


	boost::chrono::high_resolution_clock::time_point begin = boost::chrono::high_resolution_clock::now();;
	State result = this->CartesianKinematicController(current_state);
	std::cout << "time spent CartesianKinematicController: " << boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - begin).count() << " ms\n";

	begin = boost::chrono::high_resolution_clock::now();;
	this->SendSingleCommand(result,type);
	std::cout << "time spent SendSingleCommand: " << boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - begin).count() << " ms\n";


	std::cout << "time spent overall Controlling: " << boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - global_begin).count() << " ms\n";
	return true;

}

