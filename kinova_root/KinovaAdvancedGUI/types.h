#ifndef TYPES
#define TYPES

#include <string.h>
#include <vector>

using namespace std;
typedef struct{
    int id;
    float pos_x;
    float pos_y;
    float pos_z;
    float finger1;
    float finger2;
    float finger3;
    float theta_x;
    float theta_y;
    float theta_z;

    float angvel_j1;
    float angvel_j2;
    float angvel_j3;
    float angvel_j4;
    float angvel_j5;
    float angvel_j6;

}RecordedCartesianInfo;

typedef struct{
    int id;
    string name;
    string description;
    vector<RecordedCartesianInfo> trajectoryInfo;
}Trajectory;


#endif // TYPES

