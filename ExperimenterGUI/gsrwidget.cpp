#include "gsrwidget.h"
#include "ui_gsrwidget.h"
#include <iostream>
#include <QtGui>
#include <unistd.h>


using namespace  std;

GSRWidget::GSRWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GSRWidget)
{
    ui->setupUi(this);
    this->communicationOpened = false;

    // generate some data:
    QVector<double> x(101), y(101); // initialize with entries 0..100
    for (int i=0; i<101; ++i)
    {
      x[i] = i/50.0 - 1; // x goes from -1 to 1
      y[i] = x[i]*x[i]; // let's plot a quadratic function
    }
    // create graph and assign data to it:
    this->ui->customPlot->addGraph();
    this->ui->customPlot->graph(0)->setData(x, y);
    // give the axes some labels:
    this->ui->customPlot->xAxis->setLabel("x");
    this->ui->customPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    this->ui->customPlot->xAxis->setRange(-1, 1);
    this->ui->customPlot->yAxis->setRange(0, 1);
    this->ui->customPlot->replot();
}


GSRWidget::~GSRWidget()
{
    delete ui;
}

void GSRWidget::init_port(){

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        if(info.portName().contains("ttyACM")){
            qDebug() << "Using the port --> " << endl;
            qDebug() << "Name : " << info.portName();
            qDebug() << "Description : " << info.description();
            qDebug() << "Manufacturer: " << info.manufacturer();
            this->serial.setPort(info);
         //   this->serial.setBaudRate(9600);
            if (serial.open(QIODevice::ReadWrite)){
                qDebug() << "Port opened succesfully!" << endl;
                this->communicationOpened = true;

                break;
            }else{
                qDebug() << "Couldn't open the port. We'll continue scanning other available ports!" << endl;
            }

         //   connect(&this->serial, SIGNAL(readyRead()), SLOT(handleReadyRead()));
         //   connect(&this->serial, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(handleError(QSerialPort::SerialPortError)));
        }
    }

}



void GSRWidget::handleReadyRead()
{    QByteArray lastData = this->serial.readAll();
    m_readData.append(lastData);


     //TODO buscar el ultmio "sss;aaa"
    // lastData.toStdString().find_last_of()
     //add a la graphica
     //correr los ejes
     int i = index*0.01;
     this->ui->customPlot->graph(0)->addData(i,(index+1)*0.01);
      if(i>8){
        this->ui->customPlot->graph(0)->removeDataBefore(i-8);
      }
     this->ui->customPlot->replot();
     index ++;


  //qDebug() << "data: m_readData" << m_readData << endl;
}



void GSRWidget::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError) {
        qDebug() << QObject::tr("An I/O error occurred while reading the data from port %1, error: %2").arg(this->serial.portName()).arg(this->serial.errorString()) << endl;
    }
}



void GSRWidget::stopGSR()
{
    QByteArray data;
    data.append("F");
    int writtenBytes = this->serial.write(data);
    qDebug() << "written bytes = " << writtenBytes << endl;
   // this->serial.close();
    string strData = m_readData.toStdString();
    qDebug() << "The accumulated data is: " << m_readData ;
   cout << "Accumulated STR data :" << strData << endl;
}

bool GSRWidget::startGSR()
{index=0;
    bool success = false;
    if(!this->communicationOpened){
        init_port();
    }
    if(this->communicationOpened){
        //we can use a timer to read the data in fixed time intervals. As we don't need to perform any real time control with this data, this
        //idea seems to be a good solution to save CPU cycles.
      // connect(&m_timer, SIGNAL(timeout()), SLOT(handleTimeout()));
      // m_timer.start(5000);

        m_readData.clear();

        sleep(3);
        QByteArray data;
        data.append('R'); //This will restart the timestamp of the arduino and start the logging via serial port.
        int writtenBytes = this->serial.write(data);
        qDebug() << "written bytes = " << writtenBytes << endl;
        time(&this->intialTimestampGSR);
        connect(&this->serial, SIGNAL(readyRead()), SLOT(handleReadyRead()));
        connect(&this->serial, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(handleError(QSerialPort::SerialPortError)));
        success=true;
    }
    return success;
}


void GSRWidget::handleTimeout()
{
    //We also could read all the acumulated data here
    //QByteArray lastData = this->serial.readAll();
    //m_readData.append(lastData);

    qDebug() << "All Data appended =  " << m_readData << endl;


}

QByteArray GSRWidget::getData(){
    return this->m_readData;
}

time_t GSRWidget::getInitialTimestamp(){
    return this->intialTimestampGSR;
}



