/*
 * driverbot.hpp
 *
 *  Created on: May 31, 2015
 *      Author: vale
 */

#ifndef DRIVERBOT_HPP_
#define DRIVERBOT_HPP_

#include "../stats.hpp"

class driverbot : public stats
{	private:
		int Max_DS_allowed;
	public:
		int idclient;
		int NJoints;
		std::vector<int> joint_handle;
		bool sync;
		std::vector<std::string> list_value_to_read;
		boost::atomic<bool> running;
		boost::atomic<bool> running_cleaner;
		boost::atomic<bool> first_write;
		boost::thread* reader_stats;
		boost::thread* garbage_collection;
		float * tStart;
		DataStore ds_ang_pos;	DataLast dl_ang_pos;
		DataStore ds_ang_vel;	DataLast dl_ang_vel;
		DataStore ds_ang_tau;	DataLast dl_ang_tau;
		DataStore ds_cart;	    DataLast dl_cart;
		DataStore ds_t;    		DataLast dl_t;
		bool already_saving; // this flag trigger all the things that can take place during logging //---
		std::vector<std::vector<DataStoreIt> > bookmarks; //---
		std::vector<int>       active_bookmarks; //---
		// constructor
		driverbot(bool sync,std::string joint_base_name,model * bot);
		~driverbot();
		std::vector<State> FirstRead(std::vector<std::string>);
		bool GetLastValue(std::vector<State>& , std::vector<std::string> & type );
		void Start();
		void Stop();
		void Reading();
		void StartSaving(std::vector<std::string>  & type);
		void SaveCheckPoint(std::vector<std::string>  & type); //---
		void DeleteCheckPoint(); //---
		std::vector<Log> StopSaving(std::vector<std::string>  & type);
		void ClearCommands(){};
		void Cleaning();

		void ReadTimeStamp(int operationMode);
		State ReadJoints(int operationMode);
		void ReadCartesian(State q,int operationMode);

};




#endif /* DRIVERBOT_HPP_ */
