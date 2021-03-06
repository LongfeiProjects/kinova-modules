/*
 * driverbot_contr.cpp
 *
 *  Created on: Jun 16, 2015
 *      Author: vale
 */
#include "driverbot_contr.hpp"
#include "../Interface/VREP/extApi.hpp"
#include "../Interface/VREP/extApiCustom.hpp"


driverbot_contr::driverbot_contr(std::vector<std::string> namefile,std::vector<std::string> list_meas_value,std::vector<double> Pid,
			    					 int _controltype,model* md,simxInt _clientID,std::vector<int> _joint_handle)
{
	this->P = Pid[0];
	this->I = Pid[1];
	this->D = Pid[2];
	this->time_interval = 0.01; // i give a value that im going to update with the real one
	this->measured_value = list_meas_value;
	controltype = _controltype;
	bot=md;
	clientID = _clientID;
	joint_handle = _joint_handle;

    for(unsigned int i = 0;i<namefile.size();i++)
    {
    	std::vector<State> app;
    	this->ReadFile(namefile[i],app);
        ff.push_back(app);
    }
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
	if(type == -1)
	{
		if(controltype == 0) // position
		{
			for(unsigned int i =0;i<cmd.size();i++)
				simxSetJointTargetPosition(this->clientID,this->joint_handle[i],cmd[i],simx_opmode_oneshot);
		}
		else if(controltype == 1) // velocity
		{
			//DEBUG
			//std::cout<<"velocity control"<<std::endl;
			//---
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
	else
	{
		if(type == 0) // position
		{
			for(unsigned int i =0;i<cmd.size();i++)
				simxSetJointTargetPosition(this->clientID,this->joint_handle[i],cmd[i],simx_opmode_oneshot);
		}
		else if(type == 1) // velocity
		{
			//DEBUG
			//std::cout<<"velocity control"<<std::endl;
			//---
			for(unsigned int i =0;i<cmd.size();i++)
						simxSetJointTargetVelocity(this->clientID,this->joint_handle[i],cmd[i],simx_opmode_oneshot);
		}
		else if(type == 2) // torque
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

}


bool driverbot_contr::InitController(std::vector<State> initial_state)
{
	//DEBUG
	std::cout<<"0.1"<<std::endl;
	//----
	 State zero = arma::zeros<arma::vec>(6);
	//DEBUG
	std::cout<<"0.2"<<std::endl;
	//----
	this->InitCartesianKinematicController(initial_state);
	 //DEBUG
	 //std::cout<< "starting joint position"<<std::endl;
	 //	for(unsigned int ik =0;ik<desired_values[0].size();ik++)
	 //			std::cout<<desired_values[0][ik]<<" ";
	 //

	 simxFloat delta[1];
	 simxCustomGetDelta(clientID,delta,simx_opmode_oneshot_wait);
	 this->time_interval = delta[0];
	 //DEBUG
	std::cout<< "time_interval" <<time_interval<<std::endl;
	std::cout<<"0.3"<<std::endl;
	//----
	 return true;

}
bool driverbot_contr::ExecController(std::vector<State> current_state,int type)
{
	//DEBUG
	//std::cout<<"1"<<std::endl;
	//----
	// build the vector of vector of value that represent the reference to the control module
	//std::vector<State> feedforward;
	//feedforward.push_back(ff[index]);
	//DEBUG
	//std::cout<<"2"<<std::endl;
	//----
	State result = this->CartesianKinematicController(current_state);
	//DEBUG
	//std::cout<<"3"<<std::endl;
	//----
	//DEBUG
	//for(unsigned int i =0;i<result.size();i++)
	//	std::cout<<result[i]<<" ";
	//std::cout<<std::endl;
	//---
	this->SendSingleCommand(result,type);
	boost::this_thread::sleep(boost::posix_time::milliseconds(10));

	return true;

}
