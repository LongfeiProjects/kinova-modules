#-------------------------------------------------
#
# Project created by QtCreator 2015-12-02T11:38:57
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = ExperimenterGUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dialogtaskfinished.cpp \
    gsrwidget.cpp \
    logger.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    dialogtaskfinished.h \
    gsrwidget.h \
    logger.h \
    types.h \
    qcustomplot.h

FORMS    += mainwindow.ui \
    dialogtaskfinished.ui \
    gsrwidget.ui

RESOURCES += \
    resources.qrc
