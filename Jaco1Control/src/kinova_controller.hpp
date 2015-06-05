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
	std::vector<std::vector<double> > ff;
	int controltype;
	bool limitation;

	kinova_controller();
	kinova_controller(std::vector<double> Pid,std::string namefile,int _controltype,bool _limitation);
	~kinova_controller();

	POSITION_TYPE InitPositionType(int value);
	bool Move2Home();
	TrajectoryPoint ConvertControl( std::vector<double> & value);
	void SendSingleCommand(std::vector<double> cmd);
	std::vector<double> PID(std::vector<std::vector<double> > ff,std::vector<std::vector<double> > current_state);
	bool InitController(std::vector<std::vector<double> > initial_state);
	bool ExecController(std::vector<std::vector<double> > current_state);

};


#endif /* KINOVACONTROLLER_HPP_ */
