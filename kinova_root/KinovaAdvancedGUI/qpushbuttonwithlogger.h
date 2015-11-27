#ifndef QPUSHBUTTONWITHLOGGER_H
#define QPUSHBUTTONWITHLOGGER_H

#include <QtGui>
#include <qpushbutton.h>
#include "guilogger.h"
class QPushButtonWithLogger : public QPushButton
{
     Q_OBJECT
public:
    QPushButtonWithLogger(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:

public slots:
};

#endif // QPUSHBUTTONWITHLOGGER_H
