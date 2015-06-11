/*
 * driverbot.hpp
 *
 *  Created on: May 31, 2015
 *      Author: vale
 */

#ifndef DRIVERBOT_HPP_
#define DRIVERBOT_HPP_

#include "stats.hpp"

class driverbot : stats
{
	public:
	    int idclient;
	    bool sync;
		boost::atomic<bool> running;
		boost::atomic<bool> first_write;
		boost::thread* reader_stats;
		boost::thread* garbage_collection;
		DataStore ds_ang_pos;	DataLast dl_ang_pos;
		DataStore ds_ang_vel;	DataLast dl_ang_vel;
		DataStore ds_ang_tau;	DataLast dl_ang_tau;
		DataStore ds_cart_f;	DataLast dl_cart_f;
		DataStore ds_mot_amp;
		DataStore ds_comp_t;
		// constructor
		driverbot(bool sync);
	    bool GetLastValue(State& , std::string type );
	    void Start();
	    void Stop();
	    void Reading();
		void Logging();
		void Cleaning();

		void ReadTimeStamp();
		void ReadJoints();
		void ReadCartesian();

};




#endif /* DRIVERBOT_HPP_ */
