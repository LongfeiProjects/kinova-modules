#include "common.hpp"

// global variables
boost::recursive_mutex api_mutex;
boost::atomic<bool>  _first(true);
boost::atomic<bool>  _second(false);
double DEG = arma::datum::pi/180;

