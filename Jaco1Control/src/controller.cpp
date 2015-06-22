#include "controller.hpp"
// structure of the vector of CartesianKinematicController
// desired_values[0] = cartesian position
// desired_values[1] = cartesian velocity
//
// ff[0] = desired cartesian position
// current_state[0] = actual joint position
// current state[1] = actual joint velocity
void controller::InitCartesianKinematicController(std::vector<State> initial_state)
{
	// initialize the structure of desired vaule

	 desired_values.push_back(initial_state[0]);
     desired_values.push_back(initial_state[1]);

}
State controller::CartesianKinematicController(std::vector<State> current_state)
{
	//DEBUG
	//std::cout<< "input position"<<std::endl;
	//for(unsigned int ik =0;ik<desired_value[0].size();ik++)
	//		std::cout<<desired_value[0][ik]<<" ";
	//std::cout<<std::endl;
	//---
	desired_values[0] = ff[0][index];
	desired_values[1] = ff[1][index];
	State result;
	double lambda = 0.001; // bring outside
	//DEBUG
	//std::cout<<"2.1"<<std::endl;
	//----
	arma::mat J = bot->J0(current_state[0],"trasl");
	//DEBUG
	//std::cout<<"J.n_rows "<<J.n_rows<<std::endl;
	//std::cout<<"J.n_cols "<<J.n_cols<<std::endl;
	//std::cout<<"2.1.1"<<std::endl;
	//----
	arma::mat I=arma::eye(J.n_rows,J.n_rows);
	//DEBUG
	//std::cout<<"2.1.2"<<std::endl;
	//----
	arma::mat J_brack = arma::inv(J*J.t() + I*lambda);
	//DEBUG
	//std::cout<<"J_brack.n_rows "<<J_brack.n_rows<<std::endl;
	//std::cout<<"J_brack.n_cols "<<J_brack.n_cols<<std::endl;
	//std::cout<<"2.1.3"<<std::endl;
	//----
	arma::mat J_damp = J.t()*(J_brack);
	//DEBUG
	//std::cout<<"2.1.4"<<std::endl;
	//std::cout<<"desired_values[0].n_elem "<<desired_values[0].n_elem<<std::endl;
	//std::cout<<"desired_values[1].n_elem "<<desired_values[1].n_elem<<std::endl;
	//std::cout<<"current_state[0].n_elem "<<current_state[0].n_elem<<std::endl;
	//std::cout<<"current_state[1].n_elem "<<current_state[1].n_elem<<std::endl;
	//std::cout<<"ff[0].size() = "<<ff[0].size()<<std::endl;
	//for(unsigned int i =0;i<ff[0].size();i++)
	//	std::cout<< ff[0][i] <<" ";
	//std::cout << std::endl;
	//----

	 result = J_damp*(this->P*(desired_values[0] - current_state[1]) + desired_values[1]);

	//DEBUG
	//std::cout<< "desired joint velocity"<<std::endl;
	/*for(unsigned int ik =0;ik<qd_des.size();ik++)
			std::cout<<qd_des[ik]<<" ";
	std::cout<<std::endl;*/
	//---

	/*State position_error= last_current_values[0] - current_state[0];
	    std::cout<< "position_error"<<std::endl;
		for(unsigned int ik =0;ik<position_error.size();ik++)
				std::cout<<position_error[ik]<<" ";
		std::cout<<std::endl;*/


	return result;
}





