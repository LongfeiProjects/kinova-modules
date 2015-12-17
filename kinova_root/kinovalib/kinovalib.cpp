#include "kinovalib.h"
#include <iostream>
#include <dlfcn.h>
#include <vector>
#include <stdio.h>
#include <unistd.h>

using namespace std;


Kinovalib::Kinovalib()
{
    this->isKinovaInit=false;
    this->initResult=-1;
}

int Kinovalib::kinovaInit(){
    int result = -1;
    //We load the library
    this->commandLayer_handle = dlopen("Kinova.API.USBCommandLayerUbuntu.so",RTLD_NOW|RTLD_GLOBAL);

    //We load the functions from the library (Under Windows, use GetProcAddress)
    this->MyInitAPI = (int (*)()) dlsym(commandLayer_handle,"InitAPI");
    this->MyCloseAPI = (int (*)()) dlsym(commandLayer_handle,"CloseAPI");
    this->MyMoveHome = (int (*)()) dlsym(commandLayer_handle,"MoveHome");
    this->MyInitFingers = (int (*)()) dlsym(commandLayer_handle,"InitFingers");
    this->MySendBasicTrajectory = (int (*)(TrajectoryPoint)) dlsym(commandLayer_handle,"SendBasicTrajectory");
    this->MyGetQuickStatus = (int (*)(QuickStatus &)) dlsym(commandLayer_handle,"GetQuickStatus");
    this->MyGetCartesianCommand = (int (*)(CartesianPosition &)) dlsym(commandLayer_handle,"GetCartesianCommand");
    this->MyStartControlAPI = (int (*)()) dlsym(commandLayer_handle,"StartControlAPI");
    this->MySetCartesianControl = (int (*)()) dlsym(commandLayer_handle,"SetCartesianControl");
    this->MySendAdvanceTrajectory = (int (*)(TrajectoryPoint)) dlsym(commandLayer_handle,"SendAdvanceTrajectory");
    this->MyGetActualTrajectoryInfo = (int (*)(TrajectoryPoint &)) dlsym(commandLayer_handle,"GetActualTrajectoryInfo");
    this->MyGetAngularVelocity = (int (*)(AngularPosition &)) dlsym(commandLayer_handle,"GetAngularVelocity");

    if((this->MyInitAPI == NULL) || (this->MyCloseAPI == NULL) ||
       (this->MyGetQuickStatus == NULL) || (this->MySendBasicTrajectory == NULL) ||
       (this->MySendBasicTrajectory == NULL) || (this->MyMoveHome == NULL) || (this->MyInitFingers == NULL) ||
       (this->MyGetCartesianCommand == NULL))
    {
        cout << "* * *  E R R O R   D U R I N G   I N I T I A L I Z A T I O N  * * *" << endl;
    }
    else
    {
        cout << "I N I T I A L I Z A T I O N   C O M P L E T E D" << endl << endl;


        result = (*(this->MyInitAPI))();

        cout << "Initialization's result :" << result << endl;
        if(result == SUCCESS){
            result = (*(this->MyStartControlAPI))();
        }

        if(result == SUCCESS){
            cout << "Send the robot to HOME position" << endl;
            MyMoveHome();

            cout << "Initializing the fingers" << endl;
            MyInitFingers();

            this->isKinovaInit=true;
        }
        this->initResult=result;


    }
    return result;
}



int Kinovalib::initAPI(){
    return (*(this->MyInitAPI))();
}

int Kinovalib::closeAPI(){
    return (*(this->MyCloseAPI))();
}

int Kinovalib::sendBasicTrajectory(TrajectoryPoint command){
    return (*(this->MySendBasicTrajectory))(command);
}

int Kinovalib::sendAdvancedTrajectory(TrajectoryPoint command){
    return (*(this->MySendBasicTrajectory))(command);
}

int Kinovalib::moveHome(){
    initIfnotYetInitialized();
    int res = -1;
    if(this->isKinovaInit){
        res= (*(this->MyMoveHome))();
    }else{
        cout << "Could not initialize kinova" << endl;
    }
    return res;
}

int Kinovalib::initFingers(){
    return (*(this->MyInitFingers))();
}

int Kinovalib::getQuickStatus(QuickStatus &){
   //TODO How to do the wrapper?
    // return (*(this->MyGetQuickStatus))(QuickStatus);
    return -1;
}

int Kinovalib::getCartesianCommand(CartesianPosition &){
   //TODO How to do the wrapper?
    //return (*(this->getCartesianCommand))(CartesianPosition);
    return -1;
}

void Kinovalib::initIfnotYetInitialized(){
    if(!this->isKinovaInit){
        this->kinovaInit();
    }
}

int Kinovalib::getActualCartesianPosition(CartesianPosition &position){
    int res =-1;
    initIfnotYetInitialized();
    if(this->isKinovaInit){
        QuickStatus status;
        (*MyGetQuickStatus)(status);

        res = (*MyGetCartesianCommand)(position);
        cout<< "GetCartesianCommand result: " << res << endl;
    }
    return res;
}

int Kinovalib::getAngularVelocity(AngularPosition &angPos){
    int res =-1;
    initIfnotYetInitialized();
    if(this->isKinovaInit){
       res = (*MyGetAngularVelocity)(angPos);
    }
    return res;
}

void Kinovalib::getTrajectoryInfo(TrajectoryPoint &point){
     int res =-1;
     initIfnotYetInitialized();
     if(this->isKinovaInit){
        res = (*MyGetActualTrajectoryInfo)(point);
     }
 }

void Kinovalib::sendCartesianPosition(bool fingerCommand, bool armCommand, TrajectoryPoint pointToSend)
{
    cout << "Kinovalib sending cartesian position" << endl;
    initIfnotYetInitialized();
    if(this->isKinovaInit){

        QuickStatus status;
        (*MyGetQuickStatus)(status);

        CartesianPosition actualPosition;
        (*MyGetCartesianCommand)(actualPosition);

        //MoveHome and InitFingers should be already called by Kinovalib::kinovaInit function

        /* -----------------HAND_MODE---------------------
            HAND_NOMOVEMENT, < Fingers will not move.
            POSITION_MODE,   < Fingers will move using position control.
            VELOCITY_MODE,   < Fingers will move using velocity control.
        */
        if(fingerCommand){
            //we know that in this case we are using position control instead of velocity control
            pointToSend.Position.HandMode = POSITION_MODE;
        }else{
            pointToSend.Position.HandMode = HAND_NOMOVEMENT;
        }

        pointToSend.Position.Type = CARTESIAN_POSITION;

        if(pointToSend.LimitationsActive){ //if we have limitations then we have to invoke advancedTrajectory
            cout << "Sending trajectory" << endl;
            this->sendAdvancedTrajectory(pointToSend);
            cout << "*********************************" << endl << endl << endl;
        }else{
            cout << "Sending trajectory" << endl;
            this->sendBasicTrajectory(pointToSend);
            cout << "*********************************" << endl << endl << endl;
        }

    }

}

/*
This is a high level function. The direction can be Right, Left, Up or Down for Arm commands. Open or Close for Hand commands
Speed can be 0 to 100. This function will build a TrajectotyPoint and send a cartesian velocity command
*/
void Kinovalib::moveSingleStep(int direction, float speed){
    initIfnotYetInitialized();
    if(this->isKinovaInit){
        TrajectoryPoint point;
        point.InitStruct();

        point.Position.Type = CARTESIAN_VELOCITY;

       if(direction == Close || direction == Open){
            point.Position.HandMode=VELOCITY_MODE;
        }else{
            point.Position.HandMode=HAND_NOMOVEMENT;
        }

        point.LimitationsActive = true;
        point.Limitations.speedParameter1 = speed;
        point.Limitations.speedParameter2 = 0.7;

        switch (direction) {
        case Right:
            point.Position.CartesianPosition.X = -speed;
            point.Position.CartesianPosition.Y = 0;
            point.Position.CartesianPosition.Z = 0;
            point.Position.CartesianPosition.ThetaX = 0;
            point.Position.CartesianPosition.ThetaY = 0;
            point.Position.CartesianPosition.ThetaZ = 0;
            break;
        case Down:
            point.Position.CartesianPosition.X = 0;
            point.Position.CartesianPosition.Y = 0;
            point.Position.CartesianPosition.Z = -speed;
            point.Position.CartesianPosition.ThetaX = 0;
            point.Position.CartesianPosition.ThetaY = 0;
            point.Position.CartesianPosition.ThetaZ = 0;
            break;
        case Left:
            point.Position.CartesianPosition.X = speed;
            point.Position.CartesianPosition.Y = 0;
            point.Position.CartesianPosition.Z = 0;
            point.Position.CartesianPosition.ThetaX = 0;
            point.Position.CartesianPosition.ThetaY = 0;
            point.Position.CartesianPosition.ThetaZ = 0;
            break;
        case Up:
            point.Position.CartesianPosition.X = 0;
            point.Position.CartesianPosition.Y = 0;
            point.Position.CartesianPosition.Z = speed;
            point.Position.CartesianPosition.ThetaX = 0;
            point.Position.CartesianPosition.ThetaY = 0;
            point.Position.CartesianPosition.ThetaZ = 0;
            break;
        case Push:
            point.Position.CartesianPosition.X = 0;
            point.Position.CartesianPosition.Y = -speed;
            point.Position.CartesianPosition.Z = 0;
            point.Position.CartesianPosition.ThetaX = 0;
            point.Position.CartesianPosition.ThetaY = 0;
            point.Position.CartesianPosition.ThetaZ = 0;
            break;
        case Pull:
            point.Position.CartesianPosition.X = 0;
            point.Position.CartesianPosition.Y = speed;
            point.Position.CartesianPosition.Z = 0;
            point.Position.CartesianPosition.ThetaX = 0;
            point.Position.CartesianPosition.ThetaY = 0;
            point.Position.CartesianPosition.ThetaZ = 0;
            break;
        case Open:
            point.Position.CartesianPosition.X = 0;
            point.Position.CartesianPosition.Y = 0;
            point.Position.CartesianPosition.Z = 0;
            point.Position.CartesianPosition.ThetaX = 0;
            point.Position.CartesianPosition.ThetaY = 0;
            point.Position.CartesianPosition.ThetaZ = 0;
            point.Position.Fingers.Finger1 = -speed;
            point.Position.Fingers.Finger2 = -speed;
            point.Position.Fingers.Finger3 = -speed;
            break;
        case Close:
            point.Position.CartesianPosition.X = 0;
            point.Position.CartesianPosition.Y = 0;
            point.Position.CartesianPosition.Z = 0;
            point.Position.CartesianPosition.ThetaX = 0;
            point.Position.CartesianPosition.ThetaY = 0;
            point.Position.CartesianPosition.ThetaZ = 0;
            point.Position.Fingers.Finger1 = speed;
            point.Position.Fingers.Finger2 = speed;
            point.Position.Fingers.Finger3 = speed;
            break;
        default:
            cout << "Wrong direction" << endl;
            return;
            break;
        }

        this->sendCartesianVelocitySingleStep(point);
    }
}


/*Send just 1 velocity vector command*/
void Kinovalib::sendCartesianVelocitySingleStep(TrajectoryPoint pointToSend)
{
    initIfnotYetInitialized();
    if(this->isKinovaInit){
        //We specify that this point will be used an angular(joint by joint) velocity vector.
        pointToSend.Position.Type = CARTESIAN_VELOCITY;

        if(pointToSend.LimitationsActive){ //if we have limitations then we have to invoke advancedTrajectory
            this->sendAdvancedTrajectory(pointToSend);
        }else{
            this->sendBasicTrajectory(pointToSend);
        }
    }

}


void Kinovalib::sendCartesianVelocity(bool fingerCommand, bool armCommand,TrajectoryPoint pointToSend)
{
    initIfnotYetInitialized();
    if(this->isKinovaInit){
        //We specify that this point will be used an angular(joint by joint) velocity vector.
        pointToSend.Position.Type = CARTESIAN_VELOCITY;

        if(fingerCommand){
            //we know that in this case we are using position control instead of velocity control
            pointToSend.Position.HandMode = VELOCITY_MODE;
        }else{
            pointToSend.Position.HandMode = HAND_NOMOVEMENT;
        }

        if(pointToSend.LimitationsActive){ //if we have limitations then we have to invoke advancedTrajectory
            for(int i = 0; i < 200; i++)//TODO time should came from the GUI!
            {
                //We send the velocity vector every 5 ms as long as we want the robot to move along that vector.
                this->sendAdvancedTrajectory(pointToSend);
                usleep(5000);
            }
        }else{
            for(int i = 0; i < 200; i++)//TODO time should came from the GUI!
            {
                //We send the velocity vector every 5 ms as long as we want the robot to move along that vector.
                this->sendBasicTrajectory(pointToSend);
                usleep(5000);
            }
        }
    }

}

void Kinovalib::sendAngularPosition(bool fingerCommand, bool armCommand,TrajectoryPoint pointToSend)
{
    //TODO
}

void Kinovalib::sendCommand(int OP_TYPE, bool armCommand, bool fingerCommand,TrajectoryPoint pointToSend){
    switch (OP_TYPE) {
    case CARTESIAN_POSITION:
        sendCartesianPosition(fingerCommand, armCommand,pointToSend);
        break;
    case CARTESIAN_VELOCITY:
        sendCartesianVelocity(fingerCommand,armCommand,pointToSend);
        break;
    case ANGULAR_POSITION:
        sendAngularPosition(fingerCommand,armCommand,pointToSend);
        break;
    }
}


/*Returns an array of predefined hand positions.
For example: Opened hand, closed hand, pick position.*/
HandPosition* Kinovalib::getHandPresetPositions(int &numPresetPos){
    HandPosition* positions = new HandPosition[2];
    positions[0].name = "Opened hand";
    positions[0].fingers.Finger1 = 0;
    positions[0].fingers.Finger2 = 0;
    positions[0].fingers.Finger3 = 0;

    positions[1].name = "Closed hand";
    positions[1].fingers.Finger1 = 45;
    positions[1].fingers.Finger2 = 45;
    positions[1].fingers.Finger3 = 50;

    numPresetPos=2;
    return positions;

}
