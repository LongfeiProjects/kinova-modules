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
kinova_controller::kinova_controller(std::vector<std::string> namefile,std::vector<std::string> list_meas_value,
										  std::vector<double> Pid,int _controltype,bool _limitation,model* mdl,void * _APIhandle)
{
	APIhandle = _APIhandle;
	if(APIhandle != NULL)
	{
		MySetCartesianControl = (int (*)()) dlsym(APIhandle,"SetCartesianControl");
		MySendAdvanceTrajectory = (int (*)(TrajectoryPoint)) dlsym(APIhandle,"SendAdvanceTrajectory");
		MySendBasicTrajectory = (int (*)(TrajectoryPoint)) dlsym(APIhandle,"SendBasicTrajectory");
		MyMoveHome = (int (*)()) dlsym(APIhandle,"MoveHome");
	}
	if((MySendAdvanceTrajectory == NULL) || (MySetCartesianControl == NULL) || (MyMoveHome == NULL) || (MySendBasicTrajectory = NULL))
	{
		perror( "Unable to initialize the command layer.");
	}
	else
	{
		this->P = Pid[0];
		this->I = Pid[1];
		this->D = Pid[2];
		index = -2; // for accessing inizialization procedure
		this->time_interval = 0.01; // second TO CHANGE
		this->measured_value = list_meas_value;
		controltype = _controltype;
		limitation = _limitation;
		bot=mdl;
		for(unsigned int i =0;i<namefile.size();i++)
		{
			std::vector<State> app;
			this->ReadFile(namefile[i],app);
			ff.push_back(app);
		}

	}
}
kinova_controller::~kinova_controller()
{}
int kinova_controller::Move2Home()
{
	//boost::recursive_mutex::scoped_lock scoped_lock(api_mutex);
	int result = (*MyMoveHome)();
	return result;
}
TrajectoryPoint  kinova_controller::ConvertControl(State & value)
{
	TrajectoryPoint pointToSend;
	pointToSend.InitStruct();
	pointToSend.Position.Type = ANGULAR_VELOCITY;
	pointToSend.Position.HandMode = HAND_NOMOVEMENT;
	pointToSend.LimitationsActive = limitation;

	//DEFINE LIMITATIONS HERE
	//pointToSend.Limitations.speedParameter1 = 100.f;//We limit the translation velocity to 8 cm per second.
	//pointToSend.Limitations.speedParameter2 = 100.f; //We limit the orientation velocity to 0.6 RAD per second
	// conversion from rad/s to deg/s
	//value = value*(DEG);
	//DEBUG
	//std::cout<< "control"<<std::endl;
	//for(unsigned int ik =0;ik<value.size();ik++)
	//		std::cout<<value[ik]<<" ";
	//std::cout<<std::endl;
	//---
	//DEBUG
	for(unsigned int ik =0;ik<value.size();ik++)
	{
		if(ik == 5)
		value[ik] = 5;
		else
			value[ik] = 0.0;
	}
	//---

	if(controltype==ANGULAR_POSITION || controltype==ANGULAR_VELOCITY)
	{
		//DEBUG
		//std::cout<< "position velocity control"<<std::endl;
		//
		pointToSend.Position.Actuators.Actuator1 = (float)value[0];
		pointToSend.Position.Actuators.Actuator2 = (float)value[1];
		pointToSend.Position.Actuators.Actuator3 = (float)value[2];
		pointToSend.Position.Actuators.Actuator4 = (float)value[3];
		pointToSend.Position.Actuators.Actuator5 = (float)value[4];
		pointToSend.Position.Actuators.Actuator6 = (float)value[5];
	}
	else
	{
		pointToSend.Position.CartesianPosition.X = (float)value[0];
		pointToSend.Position.CartesianPosition.Y = (float)value[1];
		pointToSend.Position.CartesianPosition.Z = (float)value[2];
		//We set the orientation part of the position (unit is RAD)
		pointToSend.Position.CartesianPosition.ThetaX = (float)value[3];
		pointToSend.Position.CartesianPosition.ThetaY = (float)value[4];
		pointToSend.Position.CartesianPosition.ThetaZ = (float)value[5];
	}
	return pointToSend;
}
void kinova_controller::SendSingleCommand(State cmd)
{
     //boost::recursive_mutex::scoped_lock scoped_lock(api_mutex);
	 TrajectoryPoint p;
	 p = this->ConvertControl(cmd);
	 (*MySendAdvanceTrajectory)(p);
}
bool kinova_controller::InitController(std::vector<State> initial_state)
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
bool kinova_controller::ExecController(std::vector<State> current_state)
{
	if(_second.load(boost::memory_order_acquire))
	{
		clock_t begin_global = clock();
		//DEBUG
		//std::cout<<"executing controller"<<std::endl;
		//---
		// inhibit the repetition fo this action
		_second.store(false,boost::memory_order_release);

		clock_t begin = clock();
		State result = this->CartesianKinematicController(current_state);
		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		std::cout<< "time spent CartesianKinematicController =" << elapsed_secs<<std::endl;
		begin = clock();
		this->SendSingleCommand(result);
		end = clock();
		elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		std::cout<< "time spent SendSingleCommand =" << elapsed_secs<<std::endl;
		// this sleep control the frequency of the serialized threads (read and send)
		//boost::this_thread::sleep(boost::posix_time::milliseconds(10));
		// after ten millisecond i update my
		_first.store(true,boost::memory_order_release);
		clock_t global_end = clock();
		double global_elapsed_secs = double(global_end - begin_global) / CLOCKS_PER_SEC;
		std::cout<< "time spent Controlling =" << global_elapsed_secs<<std::endl;
		return true;
	}
	return false;
}
