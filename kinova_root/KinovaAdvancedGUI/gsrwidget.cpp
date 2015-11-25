#include "gsrwidget.h"
#include "ui_gsrwidget.h"
#include <iostream>
#include <QtGui>


using namespace  std;

GSRWidget::GSRWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GSRWidget)
{
    init_port();

    ui->setupUi(this);


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
                break;
            }else{
                qDebug() << "Couldn't open the port. We'll continue scanning other available ports!" << endl;
            }

            connect(&this->serial, SIGNAL(readyRead()), SLOT(handleReadyRead()));
            connect(&this->serial, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(handleError(QSerialPort::SerialPortError)));
        }
    }

}



void GSRWidget::handleReadyRead()
{
    QByteArray lastData = this->serial.readAll();
    m_readData.append(lastData);
 //   qDebug() << "data: m_readData" << m_readData << endl;
}



void GSRWidget::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError) {
        qDebug() << QObject::tr("An I/O error occurred while reading the data from port %1, error: %2").arg(this->serial.portName()).arg(this->serial.errorString()) << endl;
    }
}



void GSRWidget::on_stopGSR_clicked()
{
    QByteArray data;
    data.append("F");
    int writtenBytes = this->serial.write(data);
    qDebug() << "written bytes = " << writtenBytes << endl;
   // this->serial.close();
    qDebug() << "The accumulated data is: " << m_readData << endl;
}

void GSRWidget::on_startGSR_clicked()
{

    //we can use a timer to read the data in fixed time intervals. As we don't need to perform any real time control with this data, this
    //idea seems to be a good solution to save CPU cycles.
  // connect(&m_timer, SIGNAL(timeout()), SLOT(handleTimeout()));
  // m_timer.start(5000);

    m_readData.clear();

    QByteArray data;
    data.append('R');
    int writtenBytes = this->serial.write(data);
    qDebug() << "written bytes = " << writtenBytes << endl;
    connect(&this->serial, SIGNAL(readyRead()), SLOT(handleReadyRead()));
    connect(&this->serial, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(handleError(QSerialPort::SerialPortError)));
}


void GSRWidget::handleTimeout()
{
    //We also could read all the acumulated data here
    //QByteArray lastData = this->serial.readAll();
    //m_readData.append(lastData);

    qDebug() << "All Data appended =  " << m_readData << endl;


}



