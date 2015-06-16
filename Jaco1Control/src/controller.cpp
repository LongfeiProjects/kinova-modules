#include "controller.hpp"

// last_current_values[0] = last joint position (desidered)
// last_current_values[1] = last joint velocity  // not used
// last_current_values[2] = integral of joint position error
// ff[0] = desired cartesian position
// current_state[0] = actual joint position
// current state[1] = actual joint velocity
State controller::PID(std::vector<State> ff,std::vector<State> current_state)
{
	State result;
	double lambda = 0.001; // bring outside
	//DEBUG
	std::cout<<"2.1"<<std::endl;
	//----
	arma::mat J = bot->J0(current_state[0],"trasl");
	arma::mat I=arma::eye(J.n_rows,J.n_rows);
	arma::mat J_brack = arma::inv(J*J.t() + I*lambda);
	arma::mat J_damp = J.t()*(J_brack);
	State qd_des = J_damp*ff[0];
	//DEBUG
	std::cout<<"2.2"<<std::endl;
	//----
	 // euler integration of velocity(ode1)
	last_current_values[0] = last_current_values[0] + (this->time_interval*qd_des);
	// euler integration of the joint error position
	last_current_values[2] = last_current_values[2] +( (last_current_values[0]-current_state[0])*this->time_interval);
	// computation of PI
	result = P*(qd_des - current_state[1]) + this->I*last_current_values[1] + qd_des;
	//DEBUG
	std::cout<<"2.3"<<std::endl;
	//----
	return result;
}





