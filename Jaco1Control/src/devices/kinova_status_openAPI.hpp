/*
 * kinova_status.hpp
 *
 *  Created on: May 19, 2015
 *      Author: vale
 */

#ifndef KINOVA_STATUS_OPENAPI_HPP_
#define KINOVA_STATUS_OPENAPI_HPP_

#include "../stats.hpp"

#include <dlfcn.h>
#include "../visualization.hpp"

#include "../Interface/Kinova_Driver/kindrv.h"


//#define SLEEP_INTERVAL 10000    // ms    // with this value i control the sleep interval beetween
//#define TORQUE_LIMIT 20         // Nm
//#define EXT_FORCE_LIM  10       // Nm
//#define VELOCITY_LIM   25       // percentage reduce

class kinova_status_openapi : public stats
{

	private:
		visualization vis;
		int Max_DS_allowed;
	public:
		KinDrv::JacoArm *arm;
		boost::atomic<bool> running;
		boost::atomic<bool> running_cleaner;
		boost::atomic<bool> first_write;
		boost::thread* reader_stats;
		boost::thread* log_stats;
		boost::thread* garbage_collection;
		boost::chrono::high_resolution_clock::time_point tStart;
		// for logging          for visualizing      for control
		DataStore ds_ang_pos;	DataFlow ang_pos;    DataLast dl_ang_pos;
		DataStore ds_ang_vel;					     DataLast dl_ang_vel;
		DataStore ds_ang_tau;	DataFlow ang_tau;    DataLast dl_ang_tau;
		DataStore ds_cart_f;	DataFlow cart_f;	 DataLast dl_cart_f;
		DataStore ds_mot_amp;	DataFlow mot_amp;
		DataStore ds_comp_t;	DataFlow comp_t;
		DataStore ds_robot_t;
		DataStore ds_cart_pos;              		 DataLast dl_cart_pos;
		std::vector<DataStoreIt> bookmarks;

		kinova_status_openapi(model * p);
		~kinova_status_openapi();

		void Start();
		void Stop();
		void Reading();
		void StartSaving(std::vector<std::string>  & type);
		std::vector<Log> StopSaving(std::vector<std::string>  & type);
		void Logging();
		void Cleaning();
        // kinova specific functions
		void ReadTimeStamp();
		void ReadJoints(KinDrv::jaco_position_t &position,KinDrv::jaco_position_t & velocity,KinDrv::jaco_position_t & force);
		void ReadCartesian(KinDrv::jaco_position_t & position);
		//FIXME exist this in the open driver? void ReadCurrents(GeneralInformations & info);
		int Read4Vis(std::vector<State_ptr > & lastval);
		std::vector<State> FirstRead(std::vector<std::string>);
		bool GetLastValue(std::vector<State>& , std::vector<std::string>  & type );

};

#endif /* KINOVA_STATUS_OPENAPI_HPP_ */
