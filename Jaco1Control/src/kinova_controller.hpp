/*
 1* kinova_controller.hpp
 *
 *  Created on: May 11, 2015
 *      Author: vale
 */

#ifndef KINOVACONTROLLER_HPP_
#define KINOVACONTROLLER_HPP_

#include "controller.hpp"
#include <dlfcn.h>
#include <KinovaTypes.h>
#include <Kinova.API.UsbCommandLayerUbuntu.h>

class kinova_controller : public controller
{
	void * APIhandle;
	int (*MySetCartesianControl)();
	int (*MySendAdvanceTrajectory)(TrajectoryPoint command);
	int (*MyMoveHome)();

	public:
	std::vector<TrajectoryPoint> ff;

	kinova_controller();
	~kinova_controller();

	POSITION_TYPE InitPositionType(int value);
	bool Move2Home();
	void FeedForward(std::vector< std::vector<double> > & value,int controltype,bool limitation);
	bool ExecController();

};


#endif /* KINOVACONTROLLER_HPP_ */
