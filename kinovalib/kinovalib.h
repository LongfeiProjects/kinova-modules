#ifndef KINOVALIB_H
#define KINOVALIB_H
#include "Lib_Examples/Kinova.API.UsbCommandLayerUbuntu.h"
#include "Lib_Examples/KinovaTypes.h"

class Kinovalib
{

private:
    /*Pointer to the library*/
    void * commandLayer_handle;

    /*Pointers to API functions*/
    int (*MyInitAPI)();
    int (*MyCloseAPI)();
    int (*MySendBasicTrajectory)(TrajectoryPoint command);
    int (*MyGetDevices)(KinovaDevice devices[MAX_KINOVA_DEVICE], int &result);
    int (*MySetActiveDevice)(KinovaDevice device);
    int (*MyMoveHome)();
    int (*MyInitFingers)();
    int (*MyGetQuickStatus)(QuickStatus &);
    int (*MyGetCartesianCommand)(CartesianPosition &);

    bool isKinovaInit;
    bool initResult;
    KinovaDevice list[MAX_KINOVA_DEVICE];
    int activeDevice;

public:
    Kinovalib();
    /*Wrapper functions to avoid declaring pointers to the API functions each time*/
    int kinovaInit();
    int initAPI();
    int closeAPI();
    int sendBasicTrajectory(TrajectoryPoint command);
    int getDevices(KinovaDevice devices[MAX_KINOVA_DEVICE], int &result);
    int setActiveDevice(KinovaDevice device);
    int moveHome();
    int initFingers();
    int getQuickStatus(QuickStatus &);
    int getCartesianCommand(CartesianPosition &);
    /*----------------------------------------------------------------------------*/
    void sampleSend();
};

#endif // KINOVALIB_H
