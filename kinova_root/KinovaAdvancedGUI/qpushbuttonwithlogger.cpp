#include "qpushbuttonwithlogger.h"
#include <iostream>

using namespace std;

QPushButtonWithLogger::QPushButtonWithLogger(QWidget *parent):QPushButton(parent){
}




void QPushButtonWithLogger::mousePressEvent(QMouseEvent *event)
{
    //string button = (event->button() == Qt::LeftButton)?"Left button":"Right Button";
    //cout << "pressed button = " <<  button << endl;
    GUILogger::getInstance().addMouseEvent(this,event,this->objectName().toStdString());

    QPushButton::mousePressEvent(event);
}

void QPushButtonWithLogger::mouseReleaseEvent(QMouseEvent *event)
{
    //string button = (event->button() == Qt::LeftButton)?"Left button":"Right Button";
    //cout << "released button = " <<  button << endl;
    GUILogger::getInstance().addMouseEvent(this,event,this->objectName().toStdString());

    QPointF qpointf = event->localPos();
    cout << "qpointf.x(),qpointf.y() =  " << qpointf.x() << "," << qpointf.y() <<endl;
    cout << "qpointf.rx(),qpointf.ry() =  " << qpointf.rx() << "," << qpointf.ry() <<endl;
    int w = this->width();
    int h = this->height();

    if( qpointf.x()<0 || qpointf.y()<0 ||  qpointf.x() > w || qpointf.y() > h ){
        cout << "NO FUE CLICK!" <<endl;
    }


   // qpointf.
    QPushButton::mouseReleaseEvent(event);
}





