/*
 * driverbot.cpp
 *
 *  Created on: Jun 9, 2015
 *      Author: vale
 */
#include "driverbot.hpp"
#include "../Interface/VREP/extApi.h"
#include "../Interface/VREP/VrepDefinition.h"

driverbot::driverbot(bool _sync)
{
	char * connectionAddress= "127.0.0.1";
	simxInt connectionPort = 19996;
	simxInt timeOutInMs = 2000;
	simxInt commThreadCycleInMs = 5;
	//  if different from zero, then the function blocks until connected (or timed out).
	simxUChar waitUntilConnected = "a";
	// if different from zero, then the communication thread will not attempt a second connection if a connection was lost.
	simxUChar doNotReconnectOnceDisconnected = "a";
	//open connection
 	idclient = simxStart(connectionAddress,connectionPort,waitUntilConnected,doNotReconnectOnceDisconnected,timeOutInMs,commThreadCycleInMs);
 	sync = _sync;
 	if(sync)
 	{
 		simxUChar en="enable";
 		simxSynchronous(this->idclient,en);
 	}

 	reader_stats = NULL;
 	garbage_collection = NULL;

}

void driverbot::Start()
{
	this->reader_stats = new boost::thread(boost::bind(&kinova_status::Reading,this));
	this->garbage_collection = new boost::thread(boost::bind(&kinova_status::Cleaning,this));
	simxStartSimulation(this->idclient,simx_opmode_oneshot);
}


void driverbot::Reading()
{
	while(this->running.load(boost::memory_order_acquire))
	{
		this->ReadTimeStamp();
		this->ReadJoints();
		this->ReadCartesian();
		if(!first_write.load(boost::memory_order_acquire))
		{
			first_write.store(true,boost::memory_order_release);
		}
	}

}


void driverbot::ReadTimeStamp()
{

}
void driverbot::ReadJoints()
{

}
void driverbot::ReadCartesian()
{


}

