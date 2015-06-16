/*
 * kinova_conotroller.hpp
 *
 *  Created on: May 11, 2015
 *      Author: vale
 */

#include "kinova_controller.hpp"
//private function //

POSITION_TYPE kinova_controller::InitPositionType(int value)
{
	POSITION_TYPE p;
	if(value == 1)
		p = CARTESIAN_POSITION;
	else if(value == 2)
		p = ANGULAR_POSITION;
	else if(value == 7)
		p = CARTESIAN_VELOCITY;
	else if(value == 8)
		p = ANGULAR_VELOCITY;

	return p;
}
// public function//
kinova_controller::kinova_controller()
{}
kinova_controller::kinova_controller(std::string namefile,std::vector<std::string> list_meas_value,
										  std::vector<double> Pid,int _controltype,bool _limitation,model* mdl,void * _APIhandle)
{
	APIhandle = _APIhandle;
	if(APIhandle != NULL)
	{
		MySetCartesianControl = (int (*)()) dlsym(APIhandle,"SetCartesianControl");
		MySendAdvanceTrajectory = (int (*)(TrajectoryPoint)) dlsym(APIhandle,"SendAdvanceTrajectory");
		MyMoveHome = (int (*)()) dlsym(APIhandle,"MoveHome");
	}
	if((MySendAdvanceTrajectory == NULL) || (MySetCartesianControl == NULL) || (MyMoveHome == NULL))
	{
		perror( "Unable to initialize the command layer.");
	}
	else
	{
		this->P = Pid[0];
		this->I = Pid[1];
		this->D = Pid[2];
		index = -1; // for accessing inizialization procedure
		this->time_interval = 0.01; // second TO CHANGE
		this->measured_value = list_meas_value;
		controltype = _controltype;
		limitation = _limitation;
		bot=mdl;

		this->ReadFile(namefile,this->ff);

	}
}
kinova_controller::~kinova_controller()
{}
int kinova_controller::Move2Home()
{
	int result;
	result = (*MyMoveHome)();
	usleep(3000);
	return result;
}
TrajectoryPoint  kinova_controller::ConvertControl(State & value)
{
	TrajectoryPoint pointToSend;
	pointToSend.InitStruct();
	pointToSend.Position.Type = InitPositionType(controltype);
	pointToSend.LimitationsActive = limitation;

	//DEFINE LIMITATIONS HERE
	pointToSend.Limitations.speedParameter1 = 0.5f;//We limit the translation velocity to 8 cm per second.
	pointToSend.Limitations.speedParameter2 = 0.5f; //We limit the orientation velocity to 0.6 RAD per second



	if(controltype==ANGULAR_POSITION || controltype==ANGULAR_VELOCITY)
	{
		pointToSend.Position.Actuators.Actuator1 = value[0];
		pointToSend.Position.Actuators.Actuator2 = value[1];
		pointToSend.Position.Actuators.Actuator3 = value[2];
		pointToSend.Position.Actuators.Actuator4 = value[3];
		pointToSend.Position.Actuators.Actuator5 = value[4];
		pointToSend.Position.Actuators.Actuator6 = value[5];
	}
	else
	{
		pointToSend.Position.CartesianPosition.X = value[0];
		pointToSend.Position.CartesianPosition.Y = value[1];
		pointToSend.Position.CartesianPosition.Z = value[2];
		//We set the orientation part of the position (unit is RAD)
		pointToSend.Position.CartesianPosition.ThetaX = value[3];
		pointToSend.Position.CartesianPosition.ThetaY = value[4];
		pointToSend.Position.CartesianPosition.ThetaZ = value[5];
	}
	return pointToSend;
}
void kinova_controller::SendSingleCommand(State cmd)
{
	 TrajectoryPoint p;
	 p = this->ConvertControl(cmd);
	 (*MySendAdvanceTrajectory)(p);
	 //usleep(3000); // TO VERIFY
}
bool kinova_controller::InitController(std::vector<State> initial_state)
{
	 //DEBUG
	std::cout<<"0.1"<<std::endl;
	//----
	 State zero(6,0);
	//DEBUG
	std::cout<<"0.2"<<std::endl;
	//----
	 last_current_values.push_back(initial_state[0]);
	 last_current_values.push_back(zero);
	 last_current_values.push_back(zero);
	 //DEBUG
	std::cout<<"0.3"<<std::endl;
	//----
	 // this is really important! because in this way i can exit from initialization and start the execution of controller
	 return true;
}
bool  kinova_controller::ExecController(std::vector<State> current_state)
{
	//DEBUG
	std::cout<<"1"<<std::endl;
	//----
	// build the vector of vector of value that represent the reference to the control module
	std::vector<State> feedforward;
    feedforward.push_back(ff[index]);
    //DEBUG
	std::cout<<"2"<<std::endl;
	//----
	State result = this->PID(feedforward,current_state);
	//DEBUG
	std::cout<<"3"<<std::endl;
	//----
	//DEBUG
	for(unsigned int i =0;i<result.size();i++)
		std::cout<<result[i]<<" ";
	std::cout<<std::endl;
	//---
	this->SendSingleCommand(result);

	return true;
}

