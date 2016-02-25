#include "qcombowithlogger.h"

QComboWithLogger::QComboWithLogger(QWidget *parent):QComboBox(parent)
{}

void QComboWithLogger::changeEvent(QEvent *e){
   // cout <<  this->currentIndex() << endl;
    QComboBox::changeEvent(e);
}


bool QComboWithLogger::event(QEvent *event){
 //   cout << "event!  " << endl;
   return QComboBox::event(event);
}
