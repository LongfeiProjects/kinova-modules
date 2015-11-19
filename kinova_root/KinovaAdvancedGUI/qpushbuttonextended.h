#ifndef QPUSHBUTTONEXTENDED_H
#define QPUSHBUTTONEXTENDED_H

#include <QtGui>
#include <qpushbutton.h>
class QPushButtonExtended : public QPushButton
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
