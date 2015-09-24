#-------------------------------------------------
#
# Project created by QtCreator 2015-09-23T11:15:24
#
#-------------------------------------------------

QT       -= gui

TARGET = kinovalib
TEMPLATE = lib
CONFIG += staticlib

SOURCES += kinovalib.cpp


HEADERS += kinovalib.h Lib_Examples/Kinova.API.CommLayerUbuntu.h Lib_Examples/Kinova.API.UsbCommandLayerUbuntu.h Lib_Examples/KinovaTypes.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \


#if we want to include external headers....
#INCLUDEPATH += ../../../catkin_ws/src/samplekinova/src/Lib_Examples
#----------------------------
