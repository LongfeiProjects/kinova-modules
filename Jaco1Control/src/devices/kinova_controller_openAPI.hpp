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
	kinova_controller_openapi();
    kinova_controller_openapi(std::vector<std::string> namefile,std::string timestamp_namefile,Option options,std::vector<double> Pid,model* md,KinDrv::JacoArm *arm_);
	~kinova_controller_openapi();

	KinDrv::jaco_position_type_t InitPositionType(int value);
	int Move2Home();
    KinDrv::jaco_basic_traj_point_t  ConvertControl(State value,int type);
    void SetNewFF(std::vector< std::vector<State> > new_ff);
    void SendSingleCommand(State cmd,int type);
};


#endif /* KINOVACONTROLLER_OPENAPI_HPP_ */
