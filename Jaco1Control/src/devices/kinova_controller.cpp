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
kinova_controller::kinova_controller(std::vector<std::string> namefile,Option options,std::vector<double> Pid,bool _limitation,model* mdl,void * _APIhandle)
{
	APIhandle = _APIhandle;
	if(APIhandle != NULL)
	{
		MySetCartesianControl = (int (*)()) dlsym(APIhandle,"SetCartesianControl");
		MySendBasicTrajectory = (int (*)(TrajectoryPoint)) dlsym(APIhandle,"SendBasicTrajectory");
		MySendAdvanceTrajectory = (int (*)(TrajectoryPoint)) dlsym(APIhandle,"SendAdvanceTrajectory");
		MyMoveHome = (int (*)()) dlsym(APIhandle,"MoveHome");
	}
	if((MySendAdvanceTrajectory == NULL) || (MySetCartesianControl == NULL) || (MyMoveHome == NULL) || (MySendBasicTrajectory == NULL))
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
		limitation = _limitation;
		bot=mdl;
		for(unsigned int i =0;i<namefile.size();i++)
		{
			std::vector<State> app;
			this->ReadFile(namefile[i],app);
			ff.push_back(app);
		}
        this->opt = options;
        this->InitController();

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
TrajectoryPoint  kinova_controller::ConvertControl(State & value, int type)
{
	TrajectoryPoint pointToSend;
	pointToSend.InitStruct();
	if(type == -1)
	{
		pointToSend.Position.Type = this->controltype;
	}
	else
	{
		pointToSend.Position.Type = this->InitPositionType(type);
	}
	pointToSend.Position.HandMode = HAND_NOMOVEMENT;
	if(controltype==ANGULAR_POSITION || controltype==ANGULAR_VELOCITY)
	{
		value=value/DEG;
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
void kinova_controller::SendSingleCommand(State cmd,int type)
{
     //boost::recursive_mutex::scoped_lock scoped_lock(api_mutex);
	 TrajectoryPoint p;

	 boost::chrono::high_resolution_clock::time_point begin = boost::chrono::high_resolution_clock::now();
	 p = this->ConvertControl(cmd,type);
	 std::cout << "time spent ConvertControl: " << boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - begin).count() << " ms\n";

	 begin = boost::chrono::high_resolution_clock::now();
	(*MySendBasicTrajectory)(p);
	std::cout << "time spent MySendAdvanceTrajectory: " << boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - begin).count() << " ms\n";

}

// in this case i have to everwrite the ExecController provided by the controller class because i need to
// serialize the reading thread and the control thread
bool kinova_controller::ExecController(std::vector<State> current_state,int type)
{
	if(_second.load(boost::memory_order_acquire))
	{

		//DEBUG
		//std::cout<<"executing controller"<<std::endl;
		//---
		// inhibit the repetition fo this action
		boost::chrono::high_resolution_clock::time_point global_begin = boost::chrono::high_resolution_clock::now();

		_second.store(false,boost::memory_order_release);

		boost::chrono::high_resolution_clock::time_point begin = boost::chrono::high_resolution_clock::now();;
		State result = this->CartesianKinematicController(current_state);
		std::cout << "time spent CartesianKinematicController: " << boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - begin).count() << " ms\n";

		begin = boost::chrono::high_resolution_clock::now();;
		this->SendSingleCommand(result,type);
		std::cout << "time spent SendSingleCommand: " << boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - begin).count() << " ms\n";

		// this sleep control the frequency of the serialized threads (read and send)
		//boost::this_thread::sleep(boost::posix_time::milliseconds(10));
		// after ten millisecond i update my
		_first.store(true,boost::memory_order_release);
		std::cout << "time spent Controlling: " << boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - global_begin).count() << " ms\n";
		return true;
	}
	return false;
}
