#ifndef QCOMBOWITHLOGGER_H
#define QCOMBOWITHLOGGER_H

#include <QtGui>
#include <qcombobox.h>
#include <iostream>

using namespace std;

class QComboWithLogger : public QComboBox
{
    Q_OBJECT
public:
    QComboWithLogger(QWidget *parent = 0);

    void changeEvent(QEvent *e);
    bool event(QEvent *event);
signals:

public slots:
};

#endif // QCOMBOWITHLOGGER_H
