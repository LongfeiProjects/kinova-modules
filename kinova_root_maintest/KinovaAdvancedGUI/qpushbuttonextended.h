#ifndef QPUSHBUTTONEXTENDED_H
#define QPUSHBUTTONEXTENDED_H

#include <QtGui>
#include <qpushbutton.h>
#include "qpushbuttonwithlogger.h"

class QPushButtonExtended : public QPushButtonWithLogger
{
    Q_OBJECT
public:
        QPushButtonExtended(QWidget *parent = 0);

signals:
    void onPressRightClick(void);
    void onReleaseRightClick(void);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


public slots:
};

#endif // QPUSHBUTTONEXTENDED_H
