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
	APIhandle = dlopen("Kinova.API.USBCommandLayerUbuntu.so",RTLD_NOW|RTLD_GLOBAL);
	if(APIhandle != NULL)
	{
		MySetCartesianControl = (int (*)()) dlsym(APIhandle,"SetCartesianControl");
		MySendAdvanceTrajectory = (int (*)(TrajectoryPoint)) dlsym(APIhandle,"SendAdvanceTrajectory");
		MyMoveHome = (int (*)()) dlsym(APIhandle,"MoveHome");
	}

}
kinova_controller::~kinova_controller()
{}


bool kinova_controller::Move2Home()
{
	bool result = (*MyMoveHome)();
	usleep(3000);
	return result;
}


 void  kinova_controller::FeedForward(std::vector< std::vector<double> > & value,int controltype,bool limitation)
{

    std::cout <<"controltype= "<< controltype <<std::endl;
    std::cout <<"value.size()= "<< value.size() <<std::endl;

	for (unsigned int i=0;i<value.size();i++)
	{
		TrajectoryPoint pointToSend;
		pointToSend.InitStruct();
		pointToSend.Position.Type = InitPositionType(controltype);

		pointToSend.LimitationsActive = limitation;

		//DEBUG
		for(unsigned int jj = 0;jj<6;jj++)
		{
			std::cout <<value[i][jj]<<" ";
		}
		std::cout<<std::endl;
		//----

		if(controltype==ANGULAR_POSITION || controltype==ANGULAR_VELOCITY)
		{
			//DEBUG
			std::cout<<"sono nel lato joint"<<std::endl;
			//----
			pointToSend.Position.Actuators.Actuator1 = value[i][0];
			pointToSend.Position.Actuators.Actuator2 = value[i][1];
			pointToSend.Position.Actuators.Actuator3 = value[i][2];
			pointToSend.Position.Actuators.Actuator4 = value[i][3];
			pointToSend.Position.Actuators.Actuator5 = value[i][4];
			pointToSend.Position.Actuators.Actuator6 = value[i][4];


		}
		else
		{

			pointToSend.Position.CartesianPosition.X = value[i][0];
			pointToSend.Position.CartesianPosition.Y = value[i][1];
			pointToSend.Position.CartesianPosition.Z = value[i][2];
			//We set the orientation part of the position (unit is RAD)
			pointToSend.Position.CartesianPosition.ThetaX = value[i][3];
			pointToSend.Position.CartesianPosition.ThetaY = value[i][4];
			pointToSend.Position.CartesianPosition.ThetaZ = value[i][5];

		}
		std::cout<<"1"<<std::endl;
		this->ff.push_back(pointToSend);
		std::cout<<"2"<<std::endl;
	}

}


bool  kinova_controller::ExecController()
{
	//DEBUG
    std::cout << "dimension of ff= "<< ff.size() << std::endl;
    //----
	for(unsigned int i=0;i<ff.size();i++)
	{

		//here i can insert a simple pid controller
		(*MySendAdvanceTrajectory)(ff[i]);
		//usleep(SLEEP_INTERVAL);
		std::cout << i << std::endl;

	}

	return true;
}



