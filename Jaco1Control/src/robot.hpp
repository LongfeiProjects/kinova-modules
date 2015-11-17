/*
 * robot.hpp
 *l
 *  Created on: May 31, 2015
 *      Author: vale
 */

#ifndef ROBOT_HPP_
#define ROBOT_HPP_


#include "safetycheck.hpp"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Idevice.hpp"
#include "robotmodel.hpp"

class robot
{
	public:
		boost::shared_ptr<stats>  st;              // interface to the state object that collect data from the robot
		boost::shared_ptr<controller>  contr;      // interface to the controller object that send command to the robot
		safetycheck check;                         // structure used for defining the non safe conditions
		boost::thread* safety_check;               // thread that monitors if the robot is getting into a non safe conditions
		boost::thread* emergency_stop;             // thread that control the emergency stop (when the button c is pressed)
		boost::thread* action_thread;              // i can use action thread to lunch some command that are not instantaneous

		boost::atomic<bool> stop_auxiliary_thread; // this flag control the execution of the auxiliary thread (safety check and emergency stop)
		boost::atomic<bool> stop;                  // this flag is activated when a non safe condition takes place and stop all the action threads

		robot(){};

		robot(stats * _st,controller * _ct,safetycheck _check)
		: st(_st)
		,contr(_ct)
		,stop_auxiliary_thread(false)
		,stop(false)
		{
			this->check = _check;
			this->safety_check = NULL;
			this->emergency_stop = NULL;
			this->action_thread = NULL;
			this->StartAllThread();
		};
		~robot(){this->StopAllThread();};
		// method
		void ReadCurrentState(std::vector<State>& , std::vector<std::string> & type );
		void StartLog(std::vector<std::string>  & type);
		std::vector<Log> StopLog(std::vector<std::string>  & type);
		void ExecuteTrajectory();
		void SendCommand(State cmd,int type);
		void MoveHome();
	private:
		void StartAllThread();
		void StopAllThread();
		void Cheking();
		void EmergencyStop();
};



#endif /* ROBOT_HPP_ */
