#include "qpushbuttonextended.h"


QPushButtonExtended::QPushButtonExtended(QWidget *parent) : QPushButton(parent){

}


void QPushButtonExtended::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        emit onPressRightClick();
    }
    QPushButton::mousePressEvent(event);
}

void QPushButtonExtended::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        emit onReleaseRightClick();
    }
    QPushButton::mouseReleaseEvent(event);
}
