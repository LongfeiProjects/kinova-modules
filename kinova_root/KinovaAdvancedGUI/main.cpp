#include "mainwindow.h"
#include <QApplication>
#include "kinovalib.h"
#include <QtSerialPort/QSerialPort>
#include "robot.hpp"
#include "devices/kinova_status_openAPI.hpp"

#include <QDebug>

#include <QSerialPort>
#include <QSerialPortInfo>



int main(int argc, char *argv[])
{

//----------------------------------------------------------------------------------


    QApplication a(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
   // myappTranslator.load("myapp_" + QLocale::system().name());
   //myappTranslator.load("kinovaGUI_en",":/translations");
    myappTranslator.load("kinovaGUI_fr",":/translations");

    a.installTranslator(&myappTranslator);
    MainWindow w;
    w.show();





    return a.exec();
}
