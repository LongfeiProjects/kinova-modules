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

	desired_values[0] = ff[0][index];
	desired_values[1] = ff[1][index];
	State result;
	double lambda = 0; // bring outside

	boost::chrono::high_resolution_clock::time_point begin = boost::chrono::high_resolution_clock::now();
	arma::mat J = bot->J0(current_state[0],"trasl");
	std::cout << "time spent J0: " << boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - begin).count() << " ms\n";

	arma::mat I=arma::eye(J.n_rows,J.n_rows);

	begin = boost::chrono::high_resolution_clock::now();
	arma::mat J_brack = arma::inv(J*J.t() + I*lambda);
	arma::mat J_damp = J.t()*(J_brack);
	std::cout << "time spent pseudo invers: " << boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - begin).count() << " ms\n";



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

		//DEBUG

		/*
		arma::mat J_inv = arma::inv(J);
		State a(6);
		a.fill(0.0);
		for(int i =0;i<3;i++)
		{
			a[i] = desired_values[0][i];
		}
		State b(6);
		b.fill(0.0);
		for(int i =0;i<3;i++)
		{
			b[i] = current_state[1][i];
		}
		State c(6);
		c.fill(0.0);
		for(int i =0;i<3;i++)
		{
			c[i] = desired_values[1][i];
		}

		result = J_inv*(this->P*(a - b) + c);*/
		//----
	begin = boost::chrono::high_resolution_clock::now();
	result = this->P*(desired_values[0] - current_state[1]);
	std::cout << "result 1: " << boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - begin).count() << " ms\n";
	begin = boost::chrono::high_resolution_clock::now();
	result = result +  desired_values[1];
	std::cout << "result 2: " << boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - begin).count() << " ms\n";
	begin = boost::chrono::high_resolution_clock::now();
	result = J_damp*result;
	std::cout << "result 3: " << boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - begin).count() << " ms\n";


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





