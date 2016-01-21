#include "common.hpp"

// global variables
boost::recursive_mutex api_mutex;
boost::atomic<bool>  _first(true);
boost::atomic<bool>  _second(false);
double DEG = arma::datum::pi/180;


// Mathematical routine
void Mat2RPY(arma::mat m,arma::vec & rpy)
{
    double sr,cr;
    //XYZ order
    if (fabs(m.at(2,2)) < arma::datum::eps && fabs(m.at(1,2)) < arma::datum::eps)
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


// Robot interaction
State convertDirectionToState( std::string direction, float speed){
    State res(3);
    res.zeros(3);
    if(direction.compare( "Right") == 0)
    {
        res[0] = -speed;
    }
    else if(direction.compare("Down") == 0)
    {
        res[2] = -speed;
    }
    else if(direction.compare("Left") == 0)
    {
        res[0] = speed;
    }
    else if(direction.compare("Up") == 0 )
    {
        res[2] = speed;
    }
    else if(direction.compare("Push")== 0 )
    {
        res[1] = -speed;
    }
    else if(direction.compare("Pull")== 0 )
    {
        res[1] = speed;
    }
    else if(direction.compare("Open") == 0)
    {
        res[6] = -30;
        res[7] = -30;
        res[8] = -30;
    }
    else if(direction.compare("Close") == 0)
    {
        res[6] = 30;
        res[7] = 30;
        res[8] = 30;
    }
    return res;
}

