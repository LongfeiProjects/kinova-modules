/*
 * kinova.cpp
 *
 *  Created on: May 11, 2015
 *      Author: vale
 */

#include "kinova.hpp"
#include <fstream>
#include <sstream>

using namespace kinenv;



//private function //

POSITION_TYPE kinova::InitPositionType(int value)
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
kinova::kinova() : stats(1)
{
    //DEBUG
	std::cout<<"dentro costruttore kinova"<<std::endl;
    //---
	APIhandle = dlopen("Kinova.API.USBCommandLayerUbuntu.so",RTLD_NOW|RTLD_GLOBAL);
	//DEBUG
	std::cout<<"dentro costruttore kinova"<<std::endl;
	//---

	if(APIhandle != NULL)
	{
		MyInitAPI = (int (*)()) dlsym(APIhandle,"InitAPI");
		MyCloseAPI = (int (*)()) dlsym(APIhandle,"CloseAPI");
		MyStartControlAPI = (int (*)()) dlsym(APIhandle,"StartControlAPI");
		MyStopControlAPI =  (int (*)()) dlsym(APIhandle,"StopControlAPI");
		MySetCartesianControl = (int (*)()) dlsym(APIhandle,"SetCartesianControl");
		MySendAdvanceTrajectory = (int (*)(TrajectoryPoint)) dlsym(APIhandle,"SendAdvanceTrajectory");
		MyMoveHome = (int (*)()) dlsym(APIhandle,"MoveHome");

		//We validate that all the functions were loaded corectly.
		if((MyInitAPI == NULL) || (MyCloseAPI == NULL) ||
		    (MyStartControlAPI == NULL) || (MySendAdvanceTrajectory == NULL) || (MyMoveHome == NULL) )
		{
			std::cout << "Can't load all the functions from the library." << std::endl;
		}
		else
		{
			//Init the API
			bool result = (*MyInitAPI)();

			if(result == SUCCESS)
			{
				result = (*MyStartControlAPI)();
			}

			if(result == SUCCESS)
			{
				bool canExecuteProgram = true;
			}
			else
			{
				std::cout << "Cannot initializes the API." << std::endl;
			}

            //this->stats.LaunchThread();

		}


	}

}

kinova::~kinova(void)
{
	//stats.~kinova_status();
	(*MyStopControlAPI)();
	(*MyCloseAPI)();
}

int ReadFile(std::string namefile,std::vector< std::vector<double> > & value)
{
	std::ifstream infile;
	int traj;

	try
    {
	  infile.open(namefile.c_str(),std::ifstream::in);

	  std::string line;

	  // in the first line is defined the kind of trajectory
	  std::getline(infile, line);
	  std::stringstream ss1(line);
	  if ( !(ss1 >> traj) )
	  {
	     std::cout<<"problem reading the kind of control"<< std::endl;
	  }

	  while (std::getline(infile, line))
	  {
			std::stringstream ss(line);
			std::vector<double> app;
			while( !ss.eof() )
			{
				double ff;
				if ( ss >> ff)
				{
				   app.push_back(ff);
				}

			}
			if(!app.empty())
			{
				value.push_back(app);
			}


	  }
	  infile.close();
    }
    catch (std::ifstream::failure e)
    {
	  std::cerr << "Exception opening/reading/closing file\n";
    }

    return traj;
}

bool kinova::Move2Home()
{
	bool result = (*MyMoveHome)();
	usleep(3000);
	return result;
}


 void  kinova::FeedForward(std::string namefile,bool limitation)
{

	std::vector< std::vector<double> > value;
    int controltype = ReadFile(namefile,value);


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


bool  kinova::Exec()
{
	//DEBUG
    std::cout << "dimension of ff= "<< ff.size() << std::endl;
    //----
	for(unsigned int i=0;i<ff.size();i++)
	{

		//here i can insert a simple pid controller
		(*MySendAdvanceTrajectory)(ff[i]);
		usleep(SLEEP_INTERVAL);
		std::cout << i << std::endl;

	}

	return true;
}



