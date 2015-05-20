/*
 * kinova_status.hpp
 *
 *  Created on: May 19, 2015
 *      Author: vale
 */

#ifndef KINOVA_STATUS_HPP_
#define KINOVA_STATUS_HPP_

#include <iostream>
#include <dlfcn.h>
#include "KinovaTypes.h"
#include "Kinova.API.UsbCommandLayerUbuntu.h"
#include <armadillo>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>


namespace kinenv
{
	#define SLEEP_INTERVAL 10000    // ms    // with this value i control the sleep interval beetween
	#define TORQUE_LIMIT 20        // Nm
	#define EXT_FORCE_LIM  10      // Nm
	#define VELOCITY_LIM   25      // percentage reduce

	class kinova_status
	{

		private:

		    // joint space
			//int (*MyGetPositionCurrentActuators)(std::vector<float> &data);
			//int (*MyGetAngularVelocity)(AngularPosition &);
			/*int (*MyGetForcesInfo)(ForcesInfo &Response);


			// cartesian space
			int (*MyGetCartesianPosition)(CartesianPosition &);
			int (*MyGetCartesianForce)(CartesianPosition &);


			// others
			int (*MyGetAngularCommand)(AngularPosition &);
			int (*MyGetCartesianCommand)(CartesianPosition &);
			int (*MyGetSensorsInfo)(SensorsInfo &);
			int (*MyGetActuatorAcceleration)(AngularAcceleration &);*/

            // global info
			int (*MyGetGeneralInformations)(GeneralInformations &);
			int (*MyGetGlobalTrajectoryInfo)(TrajectoryFIFO &);


		public:
			boost::thread* reader_stats;
			std::vector<arma::vec > ang_position;
			std::vector<arma::vec > ang_torque;
			std::vector<arma::vec > cart_position;
			std::vector<arma::vec > cart_force;
			std::vector<arma::vec > motor_ampere;
			std::vector<double> robot_time;
			std::vector<boost::posix_time::ptime> computer_time;
			sf::Window window;
			sf::Text text;
			bool running;

			kinova_status();
            kinova_status(int i);
			~kinova_status();

		    void Reading();

		    void ReadTimeStamp(GeneralInformations & info);
		    void ReadJoints(GeneralInformations & info);
		    void ReadCartesian(GeneralInformations & info);
		    void ReadCurrents(GeneralInformations & info);

		    void PrintStats();

		    void StopRun();


	};
}


#endif /* KINOVA_STATUS_HPP_ */
