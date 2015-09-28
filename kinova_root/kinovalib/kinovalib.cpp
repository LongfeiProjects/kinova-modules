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

void Kinovalib::sampleSendCartesianVelocityType(){
    initIfnotYetInitialized();

    CartesianPosition currentCommand;

    if(this->isKinovaInit){
        cout << "Send the robot to HOME position" << endl;
        MyMoveHome();

        cout << "Initializing the fingers" << endl;
        MyInitFingers();

        TrajectoryPoint pointToSend;
        pointToSend.InitStruct();

        //We specify that this point will be used an angular(joint by joint) velocity vector.
        pointToSend.Position.Type = CARTESIAN_VELOCITY;

        pointToSend.Position.CartesianPosition.X = 0;
        pointToSend.Position.CartesianPosition.Y = -0.15; //Move along Y axis at 20 cm per second
        pointToSend.Position.CartesianPosition.Z = 0;
        pointToSend.Position.CartesianPosition.ThetaX = 0;
        pointToSend.Position.CartesianPosition.ThetaY = 0;
        pointToSend.Position.CartesianPosition.ThetaZ = 0;

        pointToSend.Position.Fingers.Finger1 = 0;
        pointToSend.Position.Fingers.Finger2 = 0;
        pointToSend.Position.Fingers.Finger3 = 0;

        for(int i = 0; i < 200; i++)
        {
            //We send the velocity vector every 5 ms as long as we want the robot to move along that vector.
            MySendBasicTrajectory(pointToSend);
            usleep(5000);
        }

        pointToSend.Position.CartesianPosition.Y = 0;
        pointToSend.Position.CartesianPosition.Z = 0.1;

        for(int i = 0; i < 200; i++)
        {
            //We send the velocity vector every 5 ms as long as we want the robot to move along that vector.
            MySendBasicTrajectory(pointToSend);
            usleep(5000);
        }

        cout << "Send the robot to HOME position" << endl;
        MyMoveHome();

        //We specify that this point will be an angular(joint by joint) position.
        pointToSend.Position.Type = CARTESIAN_POSITION;

        //We get the actual angular command of the robot.
        MyGetCartesianCommand(currentCommand);

        pointToSend.Position.CartesianPosition.X = currentCommand.Coordinates.X;
        pointToSend.Position.CartesianPosition.Y = currentCommand.Coordinates.Y - 0.1f;
        pointToSend.Position.CartesianPosition.Z = currentCommand.Coordinates.Z;
        pointToSend.Position.CartesianPosition.ThetaX = currentCommand.Coordinates.ThetaX;
        pointToSend.Position.CartesianPosition.ThetaY = currentCommand.Coordinates.ThetaY;
        pointToSend.Position.CartesianPosition.ThetaZ = currentCommand.Coordinates.ThetaZ;

        cout << "*********************************" << endl;
        cout << "Sending the first point to the robot." << endl;
        MySendBasicTrajectory(pointToSend);

        pointToSend.Position.CartesianPosition.Z = currentCommand.Coordinates.Z + 0.1f;
        cout << "Sending the second point to the robot." << endl;
        MySendBasicTrajectory(pointToSend);

        cout << "*********************************" << endl << endl << endl;

    }

    //TODO To be done in the destructor
    /*
    cout << endl << "C L O S I N G   A P I" << endl;
    result = (*MyCloseAPI)();


    dlclose(commandLayer_handle);
    */
}




void Kinovalib::sampleSendCartesianPositionType(){
    initIfnotYetInitialized();
    if(this->isKinovaInit){

        QuickStatus status;
        (*MyGetQuickStatus)(status);

        CartesianPosition actualPosition;
        (*MyGetCartesianCommand)(actualPosition);


        cout << "Send the robot to HOME position" << endl;
        MyMoveHome();

        cout << "Initializing the fingers" << endl;
        MyInitFingers();

        TrajectoryPoint pointToSend;
        pointToSend.InitStruct();

        /**NEW BLOCK, deleted the old one*/
        pointToSend.Position.HandMode = POSITION_MODE;
        pointToSend.Position.Type = CARTESIAN_POSITION;
        pointToSend.LimitationsActive = 1;

        //Note that the first position has a velocity limitation of 8 cm/sec
        pointToSend.Limitations.speedParameter1 = 0.08;
        pointToSend.Limitations.speedParameter2 = 0.7;

        pointToSend.Position.CartesianPosition.X = 0.21f;
        pointToSend.Position.CartesianPosition.Y = -0.35f;
        pointToSend.Position.CartesianPosition.Z = 0.48f;
        pointToSend.Position.CartesianPosition.ThetaX = 1.55f;
        pointToSend.Position.CartesianPosition.ThetaY = 1.02f;
        pointToSend.Position.CartesianPosition.ThetaZ = -0.03f;

        //If the robotic arm is a JACO, we use those value for the fingers.
        if(status.RobotType == 0)
        {
            pointToSend.Position.Fingers.Finger1 = 45.0f;
            pointToSend.Position.Fingers.Finger2 = 45.0f;
            pointToSend.Position.Fingers.Finger3 = 45.0f;
        }
        //If the robotic arm is a MICO, we use those value for the fingers.
        else if(status.RobotType == 1)
        {
            pointToSend.Position.Fingers.Finger1 = 4500.0f;
            pointToSend.Position.Fingers.Finger2 = 4500.0f;
            pointToSend.Position.Fingers.Finger3 = 4500.0f;
        }
        else
        {
            pointToSend.Position.Fingers.Finger1 = 0.0f;
            pointToSend.Position.Fingers.Finger2 = 0.0f;
            pointToSend.Position.Fingers.Finger3 = 0.0f;
        }

        cout << "Sending trajectory" << endl;
        (*MySendAdvanceTrajectory)(pointToSend);

        pointToSend.LimitationsActive = 0;
        pointToSend.Position.CartesianPosition.Z = 0.59f;

        cout << "Sending trajectory" << endl;
        (*MySendAdvanceTrajectory)(pointToSend);

        cout << "*********************************" << endl << endl << endl;

    }

    //TODO To be done in the destructor
    /*
    cout << endl << "C L O S I N G   A P I" << endl;
    result = (*MyCloseAPI)();


    dlclose(commandLayer_handle);
    */
}
