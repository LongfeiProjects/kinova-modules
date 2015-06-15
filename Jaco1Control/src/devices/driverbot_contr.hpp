/*
 * driverbot_contr.hpp
 *
 *  Created on: Jun 15, 2015
 *      Author: vale
 */

#ifndef DRIVERBOT_CONTR_HPP_
#define DRIVERBOT_CONTR_HPP_

#include "../controller.hpp"

class   driverbot_contr : public controller
{


public:
	std::vector<State> ff;
	int controltype;
	bool limitation;
	//driverbot_contr(){};
	//driverbot_contr(std::string namefile,std::vector<std::string> list_meas_value,
	//					std::vector<double> Pid,int _controltype,bool _limitation,model* md,void * APIhandle);
	~driverbot_contr(){};

	int Move2Home(){};
	void ConvertControl( State & value){};
	void SendSingleCommand(State cmd){};
	State PID(std::vector<State> ff,std::vector<State> current_state){};
	bool InitController(std::vector<State> initial_state){};
	bool ExecController(std::vector<State> current_state){};

};



#endif /* DRIVERBOT_CONTR_HPP_ */
