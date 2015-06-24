/*
 * kinova_status.hpp
 *
 *  Created on: May 19, 2015
 *      Author: vale
 */

#ifndef KINOVA_STATUS_HPP_
#define KINOVA_STATUS_HPP_

#include "../stats.hpp"

#include <dlfcn.h>
#include <KinovaTypes.h>
#include <Kinova.API.UsbCommandLayerUbuntu.h>
#include "../visualization.hpp"



//#define SLEEP_INTERVAL 10000    // ms    // with this value i control the sleep interval beetween
//#define TORQUE_LIMIT 20         // Nm
//#define EXT_FORCE_LIM  10       // Nm
//#define VELOCITY_LIM   25       // percentage reduce

class kinova_status : public stats
{

	private:

		// joint space
		//int (*MyGetPositionCurrentActuators)(std::vector<float> &data);
		//int (*MyGetAngularVelocity)(AngularPosition &);
		/*int (*MyGetForcesInfo)(ForcesInfo &Response);
		// cartesian space
		int (*MyGetCartesianForce)(CartesianPosition &);
		// others
		int (*MyGetAngularCommand)(AngularPosition &);
		int (*MyGetCartesianCommand)(CartesianPosition &);
		int (*MyGetSensorsInfo)(SensorsInfo &);
		int (*MyGetActuatorAcceleration)(AngularAcceleration &);*/


		int (*MyInitAPI)();
		int (*MyCloseAPI)();
		int (*MyStartControlAPI)();
		int (*MyStopControlAPI)();
		int (*MyGetAngularPosition)(AngularPosition &);
		int (*MyGetAngularVelocity)(AngularPosition &);
		int (*MyGetAngularForce)(AngularPosition &);
		int (*MyGetCartesianPosition)(CartesianPosition &);
		int (*MyGetGeneralInformations)(GeneralInformations &);
		int (*MyGetGlobalTrajectoryInfo)(TrajectoryFIFO &);
		visualization vis;
		int Max_DS_allowed;
	public:
		void * APIhandle;
		boost::atomic<bool> running;
		boost::atomic<bool> first_write;
		boost::thread* reader_stats;
		boost::thread* log_stats;
		boost::thread* garbage_collection;
		clock_t tStart;
		// for logging          for visualizing      for control
		DataStore ds_ang_pos;	DataFlow ang_pos;    DataLast dl_ang_pos;
		DataStore ds_ang_vel;					     DataLast dl_ang_vel;
		DataStore ds_ang_tau;	DataFlow ang_tau;    DataLast dl_ang_tau;
		DataStore ds_cart_f;	DataFlow cart_f;	 DataLast dl_cart_f;
		DataStore ds_mot_amp;	DataFlow mot_amp;
		DataStore ds_comp_t;	DataFlow comp_t;
		DataStore ds_robot_t;
		DataStore ds_cart_pos;              		 DataLast dl_cart_pos;

		kinova_status(model * p);
		~kinova_status();

		void Start();
		void Stop();
		void Reading();
		void Logging();
		void Cleaning();

		void ReadTimeStamp();
		void ReadJoints(AngularPosition & position,AngularPosition & velocity,AngularPosition & force);
		void ReadCartesian(AngularPosition & position);
		void ReadCurrents(GeneralInformations & info);
		int Read4Vis(std::vector<State_ptr > & lastval);
		std::vector<State> FirstRead(std::vector<std::string>);
		bool GetLastValue(std::vector<State>& , std::vector<std::string>  & type );

};

#endif /* KINOVA_STATUS_HPP_ */
