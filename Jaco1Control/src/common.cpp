#include "common.hpp"

// global variables
boost::recursive_mutex api_mutex;
boost::atomic<bool>  _first(true);
boost::atomic<bool>  _second(false);
double DEG = arma::datum::pi/180;


// Mathematical routine
arma::vec Mat2RPY(arma::mat m)
{
    double sr,cr;
    arma::vec rpy(3);

    //XYZ order
    if (abs(m.at(2,2)) < arma::datum::eps && abs(m.at(1,2)) < arma::datum::eps)
    {
        //singularity
        rpy[0] = 0;  // roll is zero
        rpy[1] = atan2(m.at(0,2), m.at(2,2));  // pitch
        rpy[2] = atan2(m.at(1,0), m.at(1,1));  // yaw is sum of roll+yaw
    }
    else
    {
        rpy[0] = atan2(-m.at(1,2), m.at(2,2));        // roll
        // compute sin/cos of roll angle
        sr = sin(rpy[0]);
        cr = cos(rpy[0]);
        rpy[1] = atan2(m.at(0,2), cr * m.at(2,2) - sr * m.at(1,2));  // pitch
        rpy[2] = atan2(-m.at(0,1), m.at(0,0));        // yaw
    }
}

