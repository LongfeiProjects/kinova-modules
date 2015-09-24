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
    this->activeDevice=-1;
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
    this->MyGetDevices = (int (*)(KinovaDevice devices[MAX_KINOVA_DEVICE], int &result)) dlsym(commandLayer_handle,"GetDevices");
    this->MySetActiveDevice = (int (*)(KinovaDevice devices)) dlsym(commandLayer_handle,"SetActiveDevice");
    this->MySendBasicTrajectory = (int (*)(TrajectoryPoint)) dlsym(commandLayer_handle,"SendBasicTrajectory");
    this->MyGetQuickStatus = (int (*)(QuickStatus &)) dlsym(commandLayer_handle,"GetQuickStatus");
    this->MyGetCartesianCommand = (int (*)(CartesianPosition &)) dlsym(commandLayer_handle,"GetCartesianCommand");

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
        this->isKinovaInit=true;
        this->initResult=result;

        int devicesCount = MyGetDevices(this->list, result);
        if(devicesCount > 0){ //We set the first device as active
            cout << "Found a robot on the USB bus (" << list[0].SerialNumber << ")" << endl;
            this->setActiveDevice(list[0]);
            this->activeDevice=0;
        }else{
            cout << "No robot found. Is the kinova arm plugged";
        }
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

int Kinovalib::getDevices(KinovaDevice devices[MAX_KINOVA_DEVICE], int &result){
    return (*(this->MyGetDevices))(devices,result);
}

int Kinovalib::setActiveDevice(KinovaDevice device){
    return (*(this->MySetActiveDevice))(device);
}

int Kinovalib::moveHome(){
    return (*(this->MyMoveHome))();
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


void Kinovalib::sampleSend(){
    if(!this->isKinovaInit){
        this->kinovaInit();
    }

    CartesianPosition currentCommand;

    if(this->activeDevice>-1){
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
