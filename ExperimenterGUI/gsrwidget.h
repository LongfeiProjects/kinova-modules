#ifndef GSRWIDGET_H
#define GSRWIDGET_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>

#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>

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
  bool isGSRRunning();

  void resetPlot();
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
    QByteArray temportalData;
    time_t intialTimestampGSR;
    bool communicationOpened;
    int index;
    bool isRunning;

    /*For GSR live plot*/
    float lastValue = -1;
    QVector<double>* data_y;
    QVector<double>* data_x;
    QwtPlot *plot;
    QwtPlotCurve *curve;

    void addpoint(float point);
    void init_plot();
};

#endif // GSRWIDGET_H
