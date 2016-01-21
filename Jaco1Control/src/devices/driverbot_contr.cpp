/*
 * driverbot_contr.cpp
 *
 *  Created on: Jun 16, 2015
 *      Author: vale
 */
#include "driverbot_contr.hpp"
#include "../Interface/VREP/extApi.hpp"
#include "../Interface/VREP/extApiCustom.hpp"


driverbot_contr::driverbot_contr(std::vector<std::string> namefile,Option options,std::vector<double> Pid,model* md,int _clientID,std::vector<int> _joint_handle)
{
	this->P = Pid[0];
	this->I = Pid[1];
	this->D = Pid[2];
	bot=md;
	clientID = _clientID;
	joint_handle = _joint_handle;
    for(unsigned int i = 0;i<namefile.size();i++)
    {
    	std::vector<State> app;
    	this->ReadFile(namefile[i],app);
        ff.push_back(app);
    }
    this->opt = options;
    this->InitController();
    // get the time step of the simulator
    simxFloat delta[1];
    simxCustomGetDelta(clientID,delta,simx_opmode_oneshot_wait);
    this->time_interval = delta[0];
    //DEBUG
   std::cout<< "time_interval" <<time_interval<<std::endl;
   //----

}

int driverbot_contr::Move2Home()
{
	//std::vector<simxFloat>targetPosition;
	//for(unsigned int i= 0;i<joint_handle.size();i++)
	//	simxSetJointTargetPosition(clientID,joint_handle[i],targetPosition[i],simx_opmode_oneshot);
	return 1;
}


void driverbot_contr::SendSingleCommand(State cmd,int type)
{
		if(type == 1)       // cartesian position
		{
			// to implement later once the communication problem with vrep are solved
		}
		else if(type == 2) // joint position
		{
			for(unsigned int i =0;i<cmd.size();i++)
				simxSetJointTargetPosition(this->clientID,this->joint_handle[i],cmd[i],simx_opmode_oneshot);
		}
		else if(type == 7) // cartesian velocity
		{
			// to implement later once the communication problem with vrep are solved
		}
		else if(type == 8) // joint velocity
		{
			//DEBUG
			//std::cout<<"velocity control"<<std::endl;
			std::cout << cmd << std::endl;
			//---
			for(unsigned int i =0;i<cmd.size();i++)
						simxSetJointTargetVelocity(this->clientID,this->joint_handle[i],cmd[i],simx_opmode_oneshot);
		}
		else if(type == 9) //  joint torque
		{
			for(unsigned int i =0;i<cmd.size();i++)
			{
				if(cmd[i]>=0)
					simxSetJointTargetVelocity(this->clientID,this->joint_handle[i],9000,simx_opmode_oneshot);
				else
					simxSetJointTargetVelocity(this->clientID,this->joint_handle[i],-9000,simx_opmode_oneshot);

				simxSetJointForce(this->clientID,this->joint_handle[i],abs(cmd[i]),simx_opmode_oneshot);
			}
		}


		/*if(type == 0) // joint position
		{
			for(unsigned int i =0;i<cmd.size();i++)
				simxSetJointTargetPosition(this->clientID,this->joint_handle[i],cmd[i],simx_opmode_oneshot);
		}
		else if(type == 1) //  joint velocity
		{
			//DEBUG
			//std::cout<<"velocity control"<<std::endl;
			//---
			for(unsigned int i =0;i<cmd.size();i++)
						simxSetJointTargetVelocity(this->clientID,this->joint_handle[i],cmd[i],simx_opmode_oneshot);
		}
		else if(type == 2) // joint torque
		{
			for(unsigned int i =0;i<cmd.size();i++)
			{
				if(cmd[i]>=0)
					simxSetJointTargetVelocity(this->clientID,this->joint_handle[i],9000,simx_opmode_oneshot);
				else
					simxSetJointTargetVelocity(this->clientID,this->joint_handle[i],-9000,simx_opmode_oneshot);

				simxSetJointForce(this->clientID,this->joint_handle[i],abs(cmd[i]),simx_opmode_oneshot);
			}
		}*/


}




