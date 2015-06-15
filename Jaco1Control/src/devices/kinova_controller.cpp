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
{
	/*APIhandle = dlopen("Kinova.API.USBCommandLayerUbuntu.so",RTLD_NOW);
	if(APIhandle != NULL)
	{
		MySetCartesianControl = (int (*)()) dlsym(APIhandle,"SetCartesianControl");
		MySendAdvanceTrajectory = (int (*)(TrajectoryPoint)) dlsym(APIhandle,"SendAdvanceTrajectory");
		MyMoveHome = (int (*)()) dlsym(APIhandle,"MoveHome");
	}*/
}
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
		//Jaco *modl =new Jaco();
		//this->bot(modl);
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
// last_current_values[0] = last joint position (desidered)
// last_current_values[1] = last joint velocity  // not used
// last_current_values[2] = integral of joint position error
// ff[0] = desired cartesian position
// current_state[0] = actual joint position
// current state[1] = actual joint velocity
State kinova_controller::PID(std::vector<State> ff,std::vector<State> current_state)
{
	State result;
	double lambda = 0.001; // bring outside
	//DEBUG
	std::cout<<"2.1"<<std::endl;
	//----
	arma::mat J = bot->J0(current_state[0],"trasl");
	arma::mat I=arma::eye(J.n_rows,J.n_rows);
	arma::mat J_brack = arma::inv(J*J.t() + I*lambda);
	arma::mat J_damp = J.t()*(J_brack);
	State qd_des = J_damp*ff[0];
	//DEBUG
	std::cout<<"2.2"<<std::endl;
	//----
	 // euler integration of velocity(ode1)
	last_current_values[0] = last_current_values[0] + (this->time_interval*qd_des);
	// euler integration of the joint error position
	last_current_values[2] = last_current_values[2] +( (last_current_values[0]-current_state[0])*this->time_interval);
	// computation of PI
	result = P*(qd_des - current_state[1]) + this->I*last_current_values[1] + qd_des;
	//DEBUG
	std::cout<<"2.3"<<std::endl;
	//----
	return result;
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
	 index = 0;
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
	//this->SendSingleCommand(result);
    // using this if statement i will keep the last value when i will reach the end of this->ff vector
	if(index<(int)feedforward.size())
		index = index + 1;
	return true;
}

