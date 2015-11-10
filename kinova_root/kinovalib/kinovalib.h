#ifndef KINOVALIB_H
#define KINOVALIB_H
#include <Kinova.API.UsbCommandLayerUbuntu.h>
#include <KinovaTypes.h>
#include "kinovalibtypes.h"

class Kinovalib
{

private:
    /*Pointer to the library*/
    void * commandLayer_handle;

    /*Pointers to API functions*/
    int (*MyInitAPI)();
    int (*MyCloseAPI)();
    int (*MySendBasicTrajectory)(TrajectoryPoint command);
    int (*MyMoveHome)();
    int (*MyInitFingers)();
    int (*MyGetQuickStatus)(QuickStatus &);
    int (*MyGetCartesianCommand)(CartesianPosition &);
    int (*MyStartControlAPI)();
    int (*MySendAdvanceTrajectory)(TrajectoryPoint command);
    int (*MySetCartesianControl)();
    int (*MyGetActualTrajectoryInfo)(TrajectoryPoint &point);
    int (*MyGetAngularVelocity)(AngularPosition &);
    /* Class atributes*/
    bool isKinovaInit;
    bool initResult;

    /*Auxiliar Functions*/
    void initIfnotYetInitialized();


public:
    Kinovalib();
    /*Wrapper functions to avoid declaring pointers to the API functions each time*/
    int kinovaInit();
    int initAPI();
    int closeAPI();
    int sendBasicTrajectory(TrajectoryPoint command);
    int moveHome();
    int initFingers();
    int getQuickStatus(QuickStatus &);
    int getCartesianCommand(CartesianPosition &);
    int startControlAPI();
    int sendAdvancedTrajectory(TrajectoryPoint trajectory);
    int setCartesianControl();
    /*----------------------------------------------------------------------------*/
    void sampleSendCartesianVelocityType();
    void sampleSendCartesianPositionType();
    void sendCommand(int OP_TYPE, bool armCommand, bool fingerCommand,TrajectoryPoint pointToSend);
    void sendCartesianPosition(bool fingerCommand, bool armCommand, TrajectoryPoint pointToSend);
    void sendCartesianVelocity(bool fingerCommand, bool armCommand,TrajectoryPoint pointToSend);
    void sendAngularPosition(bool fingerCommand, bool armCommand,TrajectoryPoint pointToSend);
    int getActualCartesianPosition(CartesianPosition &position);
    void moveSingleStep(int direction, float speed);
    void sendCartesianVelocitySingleStep(TrajectoryPoint pointToSend);
    void getTrajectoryInfo(TrajectoryPoint &point);
    int getAngularVelocity(AngularPosition &angPos);
    HandPosition *getHandPresetPositions(int &numPresetPos);
};

#endif // KINOVALIB_H
