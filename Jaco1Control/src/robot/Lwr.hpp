/*
 * Lwr.hpp
 *
 *  Created on: Jun 22, 2015
 *      Author: vale
 */

#ifndef LWR_HPP_
#define LWR_HPP_
#include "../model.hpp"

class Lwr : public model
{

public:
	void T0_n(double T[][4],const double* input1);
	void jacob0(double J0[][6], const double* input1);
	~Lwr(){};
};
#endif /* LWR_HPP_ */
