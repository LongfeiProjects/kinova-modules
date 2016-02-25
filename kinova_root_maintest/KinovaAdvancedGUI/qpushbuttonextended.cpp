#include "qpushbuttonextended.h"


QPushButtonExtended::QPushButtonExtended(QWidget *parent) : QPushButtonWithLogger(parent){

}


void QPushButtonExtended::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        emit onPressRightClick();
    }
    QPushButtonWithLogger::mousePressEvent(event);
}

void QPushButtonExtended::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        emit onReleaseRightClick();
    }
    QPushButtonWithLogger::mouseReleaseEvent(event);
}
