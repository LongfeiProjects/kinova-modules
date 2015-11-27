#include "qpushbuttonwithlogger.h"
#include <iostream>

using namespace std;
QPushButtonWithLogger::QPushButtonWithLogger(QWidget *parent):QPushButton(parent){
}




void QPushButtonWithLogger::mousePressEvent(QMouseEvent *event)
{
    string button = (event->button() == Qt::LeftButton)?"Left button":"Right Button";
    cout << "pressed button = " <<  button << endl;
    GUILogger::getInstance().addMouseEvent(event,this->objectName().toStdString());

    QPushButton::mousePressEvent(event);
}

void QPushButtonWithLogger::mouseReleaseEvent(QMouseEvent *event)
{
    string button = (event->button() == Qt::LeftButton)?"Left button":"Right Button";
    cout << "released button = " <<  button << endl;
    GUILogger::getInstance().addMouseEvent(event,this->objectName().toStdString());

    QPushButton::mouseReleaseEvent(event);
}





