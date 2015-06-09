#include "controller.hpp"

arma::mat controller::J0(State & q,std::string type)
{

	int njoint = q.size();
	const double *_q = q.memptr();
	double J[njoint][6];
	this->jacob0(J,_q);
	arma::mat Jacob((double *)(&J[0][0]),6,njoint);

	if(std::strcmp(type.c_str(),"trasl") ==0)
	{
		arma::mat Jtrasl = Jacob.submat(0,0,2,njoint);
		return Jtrasl;
	}
	else if(std::strcmp(type.c_str(),"rot") ==0)
	{
		arma::mat Jrot = Jacob.submat(3,0,6,njoint);
		return Jrot;
	}

	return Jacob;

}



