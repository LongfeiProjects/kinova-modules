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

	 //desired_values.push_back(initial_state[0]);
     //desired_values.push_back(initial_state[1]);

}
State controller::CartesianKinematicController(std::vector<State> current_state)
{

	//desired_values[0] = ff[0][index];
	//desired_values[1] = ff[1][index];
	State result;
	double lambda = 0.001; // bring outside

	// controllo nei giunti (velocita)
	arma::mat J = bot->J0(current_state[0],"trasl");
	arma::mat I=arma::eye(J.n_rows,J.n_rows);
	arma::mat J_brack = arma::inv(J*J.t() + I*lambda);
	arma::mat J_damp = J.t()*(J_brack);
	result = J_damp*(P*(ff[0][index] - current_state[1]));// + desired_values[1]);
	result[0] = -result[0];
	//arma::mat J_sub_inv = arma::pinv(J_sub);
	//result = J_sub_inv*(P*(desired_values[0] - cart) + desired_values[1]);

	result = result + ff[1][index];

	std::cout << "result[0] = " << result[0]<< std::endl;
	std::cout << "result[1] = " << result[1]<< std::endl;
	std::cout << "result[2] = " << result[2]<< std::endl;
	std::cout << "result[3] = " << result[3]<< std::endl;
	std::cout << "result[4] = " << result[4]<< std::endl;
	std::cout << "result[5] = " << result[5]<< std::endl;



	 //DEBUG
	 /*State position_error = desired_values[0] - current_state[1];
	 std::cout<< "position_error"<<std::endl;
			for(unsigned int ik =0;ik<position_error.size();ik++)
					std::cout<<position_error[ik]<<" ";
			std::cout<<std::endl;

	std::cout<< "control"<<std::endl;
				for(unsigned int ik =0;ik<result.size();ik++)
						std::cout<<result[ik]<<" ";
				std::cout<<std::endl;*/

	//---

	//DEBUG
	//std::cout<< "desired joint velocity"<<std::endl;
	//for(unsigned int ik =0;ik<qd_des.size();ik++)
	//		std::cout<<qd_des[ik]<<" ";
	//std::cout<<std::endl;
	//---

	/*State position_error= last_current_values[0] - current_state[0];
	    std::cout<< "position_error"<<std::endl;
		for(unsigned int ik =0;ik<position_error.size();ik++)
				std::cout<<position_error[ik]<<" ";
		std::cout<<std::endl;*/


	return result;
}





