#include "qpushbuttonextended.h"


QPushButtonExtended::QPushButtonExtended(QWidget *parent) : QPushButton(parent){

}


void QPushButtonExtended::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::RightButton){

        emit onPressRightClick();
        qDebug("clic droit");
    }

}
