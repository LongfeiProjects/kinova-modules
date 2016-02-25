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


public slots:
  void handleReadyRead();
  void handleError(QSerialPort::SerialPortError serialPortError);
   void handleTimeout();
private slots:
    void on_startGSR_clicked();

    void on_stopGSR_clicked();

private:
    Ui::GSRWidget *ui;
 QSerialPort serial;
 QTimer      m_timer;
 QByteArray  m_readData;

};

#endif // GSRWIDGET_H
