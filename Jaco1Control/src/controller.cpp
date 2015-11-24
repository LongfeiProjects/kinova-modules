#include "controller.hpp"

// in this function I initialize the value of each controller in the struct this->opt that are "controller dependant"
bool controller::InitController()
{
    if(this->opt.type_of_controller.compare("CartesianKinematic")==0)
    {
        const char * _meas_val[] ={"j_pos","cart_pos"};
        std::vector<std::string> meas_val(_meas_val,End(_meas_val));
        this->opt.meas_val = meas_val;
    }
}

bool controller::ExecController(std::vector<State> current_state,int type)
{
    State result;
    //boost::chrono::high_resolution_clock::time_point begin = boost::chrono::high_resolution_clock::now();
    if(this->opt.type_of_controller.compare("CartesianKinematic")==0)
    {
        result = this->CartesianKinematicController(current_state);
        //std::cout << "time spent CartesianKinematicController: " << boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - begin).count() << " ms\n";
    }
    //begin = boost::chrono::high_resolution_clock::now();
    this->SendSingleCommand(result,type);
    //std::cout << "time spresultent SendSingleCommand: " << boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - begin).count() << " ms\n";
    //std::cout << "time spent overall Controlling: " << boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - begin).count() << " ms\n";
    return true;
}
// structure of the vector of CartesianKinematicController
// ff[0] = desired cartesian position
// ff[1] = desired joint velocity
// ff[2] = desired hand finger velocity (only if active)
// current_state[0] = actual joint position
// current state[1] = actual joint velocity
// in the controller i control the construction of the result vector given the parameters in this->opt
State controller::CartesianKinematicController(std::vector<State> current_state)
{   
    State result,part_result,des_pos(3),cur_pos(3);
	double lambda = 0.001; // bring outside

    // reduce the position vector to the right dimension
    des_pos(0)= ff[0][index](0);
    des_pos(1)= ff[0][index](1);
    des_pos(2)= ff[0][index](2);
    cur_pos(0)= current_state[1](0);
    cur_pos(1)= current_state[1](1);
    cur_pos(2)= current_state[1](2);
    // joint velocity control
    arma::mat J = bot->J0(current_state[0],"trasl");
	arma::mat I=arma::eye(J.n_rows,J.n_rows);
	arma::mat J_brack = arma::inv(J*J.t() + I*lambda);
	arma::mat J_damp = J.t()*(J_brack);
    part_result = J_damp*(P*(des_pos - cur_pos));
    part_result[0] = -part_result[0]; // TOFIX (has to be removed to generalize for every robot
    part_result = part_result + ff[1][index];
    // adapting the action according to the specific driver in use
    if(this->opt.control_action>10)
    {
        std::cout << "this->opt.control_action>10"<<std::endl;
        result.set_size(9);
        result[0] = part_result[0]; result[1] = part_result[1]; result[2] = part_result[2];
        result[3] = part_result[3]; result[4] = part_result[4]; result[5] = part_result[5];
        result[6] = ff[2][index][0];
        result[7] = ff[2][index][1];
        result[8] = ff[2][index][2];
    }
    else
    {
       result.zeros(9);
       result[0] = part_result[0]; result[1] = part_result[1]; result[2] = part_result[2];
       result[3] = part_result[3]; result[4] = part_result[4]; result[5] = part_result[5];
    }
	return result;
}





