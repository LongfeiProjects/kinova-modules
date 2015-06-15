/*
 1* kinova_controller.hpp
 *
 *  Created on: May 11, 2015
 *      Author: vale
 */

#ifndef KINOVACONTROLLER_HPP_
#define KINOVACONTROLLER_HPP_

#include "../controller.hpp"
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
	std::vector<State> ff;
	int controltype;
	bool limitation;
	kinova_controller();
	kinova_controller(std::string namefile,std::vector<std::string> list_meas_value,
						std::vector<double> Pid,int _controltype,bool _limitation,model* md,void * APIhandle);
	~kinova_controller();

	POSITION_TYPE InitPositionType(int value);
	int Move2Home();
	TrajectoryPoint ConvertControl( State & value);
	void SendSingleCommand(State cmd);
	State PID(std::vector<State> ff,std::vector<State> current_state);
	bool InitController(std::vector<State> initial_state);
	bool ExecController(std::vector<State> current_state);

};


#endif /* KINOVACONTROLLER_HPP_ */
