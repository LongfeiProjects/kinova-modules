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
	int controltype;
	int clientID;
    std::vector<int> joint_handle;
    driverbot_contr(){};
    driverbot_contr(std::vector<std::string> namefile,Option options,std::vector<double> Pid,model* md,int _clientID,std::vector<int> _joint_handle);
	~driverbot_contr(){};

	int Move2Home();
	void SendSingleCommand(State cmd,int type);
	bool InitController(std::vector<State> initial_state);
	bool ExecController(std::vector<State> current_state, int type);

};



#endif /* DRIVERBOT_CONTR_HPP_ */
