/*
 * kinova.hpp
 *
 *  Created on: May 11, 2015
 *      Author: vale
 */

#ifndef KINOVA_HPP_
#define KINOVA_HPP_


#include "kinova_status.hpp"

namespace kinenv
{

	class kinova
	{
		void * APIhandle;
		int (*MyInitAPI)();
		int (*MyCloseAPI)();
		int (*MyStartControlAPI)();
		int (*MyStopControlAPI)();
		int (*MySetCartesianControl)();
		int (*MySendAdvanceTrajectory)(TrajectoryPoint command);
		int (*MyMoveHome)();

		public:
		std::vector<TrajectoryPoint> ff;
		kinova_status stats;

		POSITION_TYPE InitPositionType(int value);
		kinova();
		~kinova();

		bool Move2Home();
		void FeedForward(std::string namefile,bool limitation);
		// each of these function contain a simple low level control (not for position)
		void GetGlobalStatus();
		bool Exec();

	};

}


#endif /* KINOVA_HPP_ */
