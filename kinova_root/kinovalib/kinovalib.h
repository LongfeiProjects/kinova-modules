#ifndef KINOVALIB_H
#define KINOVALIB_H
#include <Kinova.API.UsbCommandLayerUbuntu.h>
#include <KinovaTypes.h>

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
    int sendAdvanceTrajectory(TrajectoryPoint trajectory);
    int setCartesianControl();
    /*----------------------------------------------------------------------------*/
    void sampleSendCartesianVelocityType();
    void sampleSendCartesianPositionType();
};

#endif // KINOVALIB_H
