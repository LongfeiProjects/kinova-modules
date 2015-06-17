#include "controller.hpp"

// last_current_values[0] = last joint position (desidered)
// last_current_values[1] = last joint velocity  // not used (desired)
// last_current_values[2] = integral of joint position error
// ff[0] = desired cartesian position
// current_state[0] = actual joint position
// current state[1] = actual joint velocity
State controller::PID(std::vector<State> ff,std::vector<State> current_state)
{
	State result;
	double lambda = 0.001; // bring outside
	//DEBUG
	//std::cout<<"2.1"<<std::endl;
	//----
	arma::mat J = bot->J0(last_current_values[0],"trasl");
	//DEBUG
	//std::cout<<"2.1.1"<<std::endl;
	//----
	arma::mat I=arma::eye(J.n_rows,J.n_rows);
	//DEBUG
	//std::cout<<"2.1.2"<<std::endl;
	//----
	arma::mat J_brack = arma::inv(J*J.t() + I*lambda);
	//DEBUG
	//std::cout<<"2.1.3"<<std::endl;
	//----
	arma::mat J_damp = J.t()*(J_brack);
	//DEBUG
	//std::cout<<"2.1.4"<<std::endl;
	//std::cout<<"ff[0].size() = "<<ff[0].size()<<std::endl;
	//for(unsigned int i =0;i<ff[0].size();i++)
	//	std::cout<< ff[0][i] <<" ";
	//std::cout << std::endl;
	//----
	State qd_des = J_damp*ff[0];

	//DEBUG
	std::cout<< "desired joint velocity"<<std::endl;
	for(unsigned int ik =0;ik<qd_des.size();ik++)
			std::cout<<qd_des[ik]<<" ";
	std::cout<<std::endl;
	//---

	//DEBUG
	//std::cout<<"2.2"<<std::endl;
	//std::cout<<"last_current_values[0].size()"<<last_current_values[0].size()<<std::endl;
	//std::cout<<"last_current_values[2].size()"<<last_current_values[2].size()<<std::endl;
	//std::cout<<"current_state[0].size()"<<current_state[0].size()<<std::endl;
	//std::cout<<"current_state[1].size()"<<current_state[1].size()<<std::endl;
	//std::cout<<"qd_des.size()"<<qd_des.size()<<std::endl;
	//----

	//DEBUG
	State Time_qd_des = this->time_interval*qd_des;
	std::cout<< "time interval x qd des"<<std::endl;
	for(unsigned int ik =0;ik<Time_qd_des.size();ik++)
			std::cout<<Time_qd_des[ik]<<" ";
	std::cout<<std::endl;
	//---
	 // euler integration of velocity(ode1)
	last_current_values[0] = last_current_values[0] + (this->time_interval*qd_des);
	//DEBUG
	//std::cout<<"point 1"<<std::endl;
	//---
	//DEBUG
	std::cout<< "joint desired position"<<std::endl;
	for(unsigned int ik =0;ik<last_current_values[0].size();ik++)
			std::cout<<last_current_values[0][ik]<<" ";
	std::cout<<std::endl;

	std::cout<< "joint current position"<<std::endl;
		for(unsigned int ik =0;ik<current_state[0].size();ik++)
				std::cout<<current_state[0][ik]<<" ";
		std::cout<<std::endl;

	std::cout<< "joint current velocity"<<std::endl;
			for(unsigned int ik =0;ik<current_state[1].size();ik++)
					std::cout<<current_state[1][ik]<<" ";
			std::cout<<std::endl;

	//---

	// euler integration of the joint error position
	last_current_values[2] = last_current_values[2] +( (last_current_values[0]-current_state[0])*this->time_interval);
	//DEBUG
	//std::cout<<"point 2"<<std::endl;
	//---
	//DEBUG
	std::cout<< "integral error"<<std::endl;
	for(unsigned int ik =0;ik<last_current_values[2].size();ik++)
			std::cout<<last_current_values[2][ik]<<" ";
	std::cout<<std::endl;

	State velocity_error= qd_des - current_state[1];
    std::cout<< "velocity_error"<<std::endl;
	for(unsigned int ik =0;ik<velocity_error.size();ik++)
			std::cout<<velocity_error[ik]<<" ";
	std::cout<<std::endl;
	//---

	// computation of PI
	result = P*(qd_des - current_state[1]) + this->I*last_current_values[2] + qd_des;
	//result = P*(qd_des - current_state[1])  + qd_des;
	//DEBUG
	std::cout<< "result"<<std::endl;
	for(unsigned int ik =0;ik<result.size();ik++)
		std::cout<<result[ik]<<" ";
	std::cout<<std::endl;
	//std::cout<<"2.3"<<std::endl;
	//----
	return result;
}





