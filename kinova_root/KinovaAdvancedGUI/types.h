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
    float vel_x;
    float vel_y;
    float vel_z;
}RecordedCartesianInfo;

typedef struct{
    int id;
    string name;
    string description;
    vector<RecordedCartesianInfo> trajectoryInfo;
}Trajectory;


#endif // TYPES

