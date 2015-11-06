#-------------------------------------------------
#
# Project created by QtCreator 2015-09-23T11:16:51
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KinovaAdvancedGUI
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x -pthread

SOURCES += main.cpp\
        mainwindow.cpp \
    sqlmanager.cpp \
    dialog.cpp \
    flowlayout.cpp

HEADERS  += mainwindow.h \
    sqlmanager.h \
    dialog.h \
    types.h \
    flowlayout.h

FORMS    += mainwindow.ui \
    dialog.ui

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

RESOURCES += \
    resources.qrc

TRANSLATIONS = kinovaGUI_en.ts \n
                kinovaGUI_fr.ts
