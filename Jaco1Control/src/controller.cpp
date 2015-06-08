#include "controller.hpp"

arma::mat controller::J0(State & q,std::string type)
{

	int njoint = q.size();
	const double *_q = q.memptr();
	double J[njoint][6];
	this->jacob0(J,_q);
	arma::mat Jacob((double *)(&J[0][0]),6,njoint);

	return Jacob;

}



