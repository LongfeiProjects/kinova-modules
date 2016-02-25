#ifndef KINOVALIBTYPES
#define KINOVALIBTYPES
#include <kinovalib.h>
#include <string>

using namespace std;

/*For high level control functionality*/
enum MOVE_DIRECTION
{

    Up = 1,


    Right = 2,


    Down = 3,


    Left = 4,

    Push = 5,

    Pull = 6,

    Open = 7,

    Close = 8
};


struct HandPosition
{
    string  name;
    FingersPosition fingers;
};

#endif // KINOVALIBTYPES

