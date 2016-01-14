#ifndef GSRWIDGET_H
#define GSRWIDGET_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>

namespace Ui {
class GSRWidget;
}

class GSRWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GSRWidget(QWidget *parent = 0);
    ~GSRWidget();
  void init_port(); // port initialisation function

  bool startGSR();
  void stopGSR();
  QByteArray getData();

  time_t getInitialTimestamp();
public slots:
  void handleReadyRead();
  void handleError(QSerialPort::SerialPortError serialPortError);
   void handleTimeout();
private slots:


private:
    Ui::GSRWidget *ui;
    QSerialPort serial;
    QTimer      m_timer;
    QByteArray  m_readData;
    time_t intialTimestampGSR;
    bool communicationOpened;
    int index;
};

#endif // GSRWIDGET_H
