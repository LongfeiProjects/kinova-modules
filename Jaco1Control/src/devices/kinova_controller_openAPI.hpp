/*
 1* kinova_controller.hpp
 *
 *  Created on: May 11, 2015
 *      Author: vale
 */

#ifndef KINOVACONTROLLER_OPENAPI_HPP_
#define KINOVACONTROLLER_OPENAPI_HPP_

#include "../controller.hpp"
#include <dlfcn.h>
#include "../Interface/Kinova_Driver/kindrv.h"

class kinova_controller_openapi : public controller
{
	KinDrv::JacoArm *arm;

public:
	KinDrv::jaco_position_type_t controltype;
	bool limitation;
	kinova_controller_openapi();
	kinova_controller_openapi(std::vector<std::string> namefile,std::vector<std::string> list_meas_value,
						std::vector<double> Pid,int _controltype,bool _limitation,model* md,KinDrv::JacoArm *arm_);
	~kinova_controller_openapi();

	KinDrv::jaco_position_type_t InitPositionType(int value);
	int Move2Home();
	KinDrv::jaco_basic_traj_point_t  ConvertControl(State & value,int type);
	void SendSingleCommand(State cmd,int type);
	bool InitController(std::vector<State> initial_state);
	bool ExecController(std::vector<State> current_state, int type);

};


#endif /* KINOVACONTROLLER_OPENAPI_HPP_ */
