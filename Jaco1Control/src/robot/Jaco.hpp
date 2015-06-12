#include "../model.hpp"

class Jaco : public model
{

public:
	void T0_n(double T[][4],const double* input1);
	void jacob0(double J0[][6], const double* input1);
	~Jaco();
};
