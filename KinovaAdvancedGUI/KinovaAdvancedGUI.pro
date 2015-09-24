#-------------------------------------------------
#
# Project created by QtCreator 2015-09-23T11:16:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KinovaAdvancedGUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../kinovalib/release/ -lkinovalib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../kinovalib/debug/ -lkinovalib
else:unix: LIBS += -L$$OUT_PWD/../kinovalib/ -lkinovalib

INCLUDEPATH += $$PWD/../kinovalib
DEPENDPATH += $$PWD/../kinovalib


win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../kinovalib/release/libkinovalib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../kinovalib/debug/libkinovalib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../kinovalib/release/kinovalib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../kinovalib/debug/kinovalib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../kinovalib/libkinovalib.a

unix|win32: LIBS += -ldl
