/*
 * driverbot.hpp
 *
 *  Created on: May 31, 2015
 *      Author: vale
 */

#ifndef DRIVERBOT_HPP_
#define DRIVERBOT_HPP_

#include "../stats.hpp"

class driverbot : stats
{	private:
		int Max_DS_allowed;
	public:
		int idclient;
		int NJoints;
		std::vector<int> joint_handle;
		bool sync;
		boost::atomic<bool> running;
		boost::atomic<bool> first_write;
		boost::thread* reader_stats;
		boost::thread* garbage_collection;
		DataStore ds_ang_pos;	DataLast dl_ang_pos;
		DataStore ds_ang_vel;	DataLast dl_ang_vel;
		DataStore ds_ang_tau;	DataLast dl_ang_tau;
		DataStore ds_cart;	    DataLast dl_cart;
		DataStore ds_t;    		DataLast dl_t;
		// constructor
		driverbot(bool sync,std::string joint_base_name,model * bot);
		~driverbot();
		bool GetLastValue(State& , std::string type );
		void Start();
		void Stop();
		void Reading();
		void Cleaning();

		void ReadTimeStamp();
		State ReadJoints();
		void ReadCartesian(State q);

};




#endif /* DRIVERBOT_HPP_ */
