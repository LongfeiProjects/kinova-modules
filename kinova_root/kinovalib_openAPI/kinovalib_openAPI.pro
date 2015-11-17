#-------------------------------------------------
#
# Project created by QtCreator 2015-11-16T16:27:57
#
#-------------------------------------------------

QT       -= gui

TARGET = kinovalib_openAPI
TEMPLATE = lib

DEFINES += KINOVALIB_OPENAPI_LIBRARY

SOURCES += dummy.cpp \
    ../../Jaco1Control/src/visualization.cpp \
    ../../Jaco1Control/src/safetycheck.cpp \
    ../../Jaco1Control/src/geometry.cpp \
    ../../Jaco1Control/src/common.cpp \
    ../../Jaco1Control/src/robot.cpp \
    ../../Jaco1Control/src/model.cpp \
    ../../Jaco1Control/src/main.cpp \
    ../../Jaco1Control/src/controller.cpp \
    ../../Jaco1Control/src/devices/driverbot.cpp \
    ../../Jaco1Control/src/devices/driverbot_contr.cpp \
    ../../Jaco1Control/src/devices/kinova_controller.cpp \
    ../../Jaco1Control/src/devices/kinova_controller_openAPI.cpp \
    ../../Jaco1Control/src/devices/kinova_status.cpp \
    ../../Jaco1Control/src/devices/kinova_status_openAPI.cpp \
    ../../Jaco1Control/src/Interface/Kinova_Driver/exception.cpp \
    ../../Jaco1Control/src/Interface/Kinova_Driver/kindrv.cpp \
    ../../Jaco1Control/src/Interface/VREP/extApi.cpp \
    ../../Jaco1Control/src/Interface/VREP/extApiCustom.cpp \
    ../../Jaco1Control/src/Interface/VREP/extApiInternal.cpp \
    ../../Jaco1Control/src/Interface/VREP/extApiPlatform.cpp \
    ../../Jaco1Control/src/robot/Jaco.cpp \
    ../../Jaco1Control/src/robot/Lwr.cpp

HEADERS += dummy.h\
        kinovalib_openapi_global.h \
    ../../Jaco1Control/src/visualization.hpp \
    ../../Jaco1Control/src/robotmodel.hpp \
    ../../Jaco1Control/src/model.hpp \
    ../../Jaco1Control/src/geometry.hpp \
    ../../Jaco1Control/src/stats.hpp \
    ../../Jaco1Control/src/safetycheck.hpp \
    ../../Jaco1Control/src/robot.hpp \
    ../../Jaco1Control/src/controller.hpp \
    ../../Jaco1Control/src/common.hpp \
    ../../Jaco1Control/src/Idevice.hpp \
    ../../Jaco1Control/src/devices/driverbot.hpp \
    ../../Jaco1Control/src/devices/driverbot_contr.hpp \
    ../../Jaco1Control/src/devices/kinova_controller.hpp \
    ../../Jaco1Control/src/devices/kinova_controller_openAPI.hpp \
    ../../Jaco1Control/src/devices/kinova_status.hpp \
    ../../Jaco1Control/src/devices/kinova_status_openAPI.hpp \
    ../../Jaco1Control/src/Interface/Kinova_Driver/exception.h \
    ../../Jaco1Control/src/Interface/Kinova_Driver/kindrv.h \
    ../../Jaco1Control/src/Interface/Kinova_Driver/types.h \
    ../../Jaco1Control/src/Interface/VREP/include/extApiCustomConst.h \
    ../../Jaco1Control/src/Interface/VREP/include/v_repConst.h \
    ../../Jaco1Control/src/Interface/VREP/include/v_repLib.h \
    ../../Jaco1Control/src/Interface/VREP/include/v_repTypes.h \
    ../../Jaco1Control/src/Interface/VREP/extApi.hpp \
    ../../Jaco1Control/src/Interface/VREP/extApiCustom.hpp \
    ../../Jaco1Control/src/Interface/VREP/extApiInternal.hpp \
    ../../Jaco1Control/src/Interface/VREP/extApiPlatform.hpp \
    ../../Jaco1Control/src/robot/dummybot.hpp \
    ../../Jaco1Control/src/robot/Jaco.hpp \
    ../../Jaco1Control/src/robot/Lwr.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += /usr/include/libusb-1.0/

LIBS += -lusb-1.0 -lsfml-graphics -lsfml-system -lsfml-window -lboost_system -lboost_thread -lpthread -ldl -llapack -larmadillo -lplplotcxxd -lboost_filesystem -lboost_timer -lboost_chrono
#-lsfml-graphics -lsfml-system -lsfml-window -lboost_timer -lboost_chrono -lboost_system -lboost_thread -lpthread -ldl -llapack -larmadillo -lplplotcxxd -lboost_filesystem -lkindrv

DISTFILES += \
    ../../Jaco1Control/src/Makefile
