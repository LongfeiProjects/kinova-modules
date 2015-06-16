/*
 * driverbot_contr.cpp
 *
 *  Created on: Jun 16, 2015
 *      Author: vale
 */
#include "driverbot_contr.hpp"
#include "../Interface/VREP/extApi.hpp"
#include "../Interface/VREP/extApiCustom.hpp"


driverbot_contr::driverbot_contr(std::string namefile,std::vector<std::string> list_meas_value,std::vector<double> Pid,
			    					 int _controltype,model* md,simxInt _clientID,std::vector<int> _joint_handle)
{


	this->P = Pid[0];
	this->I = Pid[1];
	this->D = Pid[2];
	index = -1; // for accessing inizialization procedure
	this->time_interval = 0.01; // second TO CHANGE
	this->measured_value = list_meas_value;
	controltype = _controltype;
	bot=md;
	clientID = _clientID;
	joint_handle = _joint_handle;

	simxFloat delta[1];
	simxCustomGetDelta(clientID,delta,simx_opmode_oneshot_wait);
	this->time_interval = delta[0];

	this->ReadFile(namefile,this->ff);
}



int driverbot_contr::Move2Home()
{
	//std::vector<simxFloat>targetPosition;
	//for(unsigned int i= 0;i<joint_handle.size();i++)
	//	simxSetJointTargetPosition(clientID,joint_handle[i],targetPosition[i],simx_opmode_oneshot);
}


void driverbot_contr::SendSingleCommand(State cmd)
{
	if(controltype == 0) // position
	{
		for(unsigned int i =0;i<cmd.size();i++)
			simxSetJointTargetPosition(this->clientID,this->joint_handle[i],cmd[i],simx_opmode_oneshot);
	}
	else if(controltype == 1) // velocity
	{
		for(unsigned int i =0;i<cmd.size();i++)
					simxSetJointTargetVelocity(this->clientID,this->joint_handle[i],cmd[i],simx_opmode_oneshot);
	}
	else if(controltype == 2) // torque
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

}

