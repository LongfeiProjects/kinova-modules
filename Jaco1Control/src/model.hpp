/*
 * model.hpp
 *
 *  Created on: Jun 12, 2015
 *      Author: vale
 */

#ifndef MODEL_HPP_
#define MODEL_HPP_


#include "common.hpp"


class model
{
   public:
	int n; // dimension of the state space (number of joints)
      // pure virtual function
	virtual void T0_n(double T[][4],const double* input1)=0 ;
	virtual void jacob0(double J0[][6], const double* input1) =0;
	void DK(State & q,State & position,arma::mat & R);
	arma::mat J0(State & q,std::string type);

    virtual ~model();
};


#endif /* MODEL_HPP_ */
