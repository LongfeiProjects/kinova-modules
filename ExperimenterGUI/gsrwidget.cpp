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

    this->isRunning=false;
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
    this->isRunning=false;
    qDebug() << "The accumulated data is: " << m_readData ;
   cout << "Accumulated STR data :" << strData << endl;
}

bool GSRWidget::startGSR()
{

    bool success = false;
    if(!this->communicationOpened){
        init_port();
    }
    if(this->communicationOpened){
        //we can use a timer to read the data in fixed time intervals. As we don't need to perform any real time control with this data, this
        //idea seems to be a good solution to save CPU cycles.
      // connect(&m_timer, SIGNAL(timeout()), SLOT(handleTimeout()));
      // m_timer.start(5000);

        if(m_readData.size()>0){
           // QMessageBox::StandardButton reply;
            int reply;
            QMessageBox msg(QMessageBox::Question,tr("Warning"),tr("There is previous data, do yo want to overrite or append the new data?"),
                            QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,this);
            msg.setButtonText(QMessageBox::Yes,tr("Append"));
            msg.setButtonText(QMessageBox::No,tr("Overrite"));
            msg.setButtonText(QMessageBox::Cancel,tr("Cancel"));

            reply = msg.exec();

            if(reply == QMessageBox::No){
                cout << "overrite gsr data" << endl;
                 m_readData.clear();
            }else if(reply == QMessageBox::Cancel){
                cout << "cancel" << endl;
                return false;
            }else{
                cout << "Append gsr data" << endl;
            }
        }



        sleep(3);
        QByteArray data;
        data.append('R'); //This will restart the timestamp of the arduino and start the logging via serial port.
        int writtenBytes = this->serial.write(data);
        qDebug() << "written bytes = " << writtenBytes << endl;
        time(&this->intialTimestampGSR);
        connect(&this->serial, SIGNAL(readyRead()), SLOT(handleReadyRead()));
        connect(&this->serial, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(handleError(QSerialPort::SerialPortError)));
        success=true;
        this->isRunning=true;
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

bool GSRWidget::isGSRRunning(){
    return this->isRunning;
}


