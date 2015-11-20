/*
 * model.cpp
 *
 *  Created on: Jun 12, 2015
 *      Author: vale
 */
#include "model.hpp"

arma::mat model::J0(State & q,std::string type)
{

	int njoint = q.size();
	const double *_q = q.memptr();
	double J[njoint][6];
	this->jacob0(J,_q);
	arma::mat Jacob((double *)(&J[0][0]),6,njoint);

	if(std::strcmp(type.c_str(),"trasl") == 0)
	{
		//DEBUG
		//std::cout<<"prima di costruire J trasl"<<std::endl;
		//std::cout<<"Jacob.n_rows = "<<Jacob.n_rows<<std::endl;
		//std::cout<<"Jacob.n_cols = "<<Jacob.n_cols<<std::endl;
		//std::cout<<"njoint-1 = "<<njoint-1<<std::endl;
		//---

		arma::mat Jtrasl = Jacob.submat(0,0,2,njoint-1);

		//DEBUG
		//std::cout<<"Jtrasl.n_rows = "<<Jtrasl.n_rows<<std::endl;
		//std::cout<<"Jtrasl.n_cols = "<<Jtrasl.n_cols<<std::endl;
		//---

		return Jtrasl;
	}
	else if(std::strcmp(type.c_str(),"rot") ==0)
	{
		arma::mat Jrot = Jacob.submat(3,0,5,njoint-1);
		return Jrot;
	}

	return Jacob;

}



void model::DK(State & q,State & position,arma::mat & R)
{
	const double *_q = q.memptr();
	double T[4][4];
	this->T0_n(T,_q);
	arma::mat rototrasl((double *)(&T[0][0]),4,4);
	R = rototrasl.submat(0,0,2,2);
	position = rototrasl.submat(0,3,2,3);
}


model::~model()
{}

