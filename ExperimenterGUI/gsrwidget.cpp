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


    //creation of tables of your data
    data_x = new QVector<double>;
    data_y = new QVector<double>;

    //initiation of the graphic
    init_plot();

    //creation of a layout to put the graphic inside
    QHBoxLayout *layout = new QHBoxLayout( this );
    layout->setContentsMargins( 5, 5, 5, 5 ); //we set the margin of the layout
    layout->addWidget( plot ); //we add the plot

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
     temportalData.append(lastData);

      //this is just to update the plot
     QList<QByteArray> splitted = temportalData.split('\r');
     if(splitted.size()>1){
        QByteArray numericalPart = splitted.at(0);
        QList<QByteArray> time_and_vals = numericalPart.split(';');
        if(time_and_vals.size()>1){
            this->lastValue = time_and_vals.at(1).toFloat();
        }else{
            this->lastValue = -1; //this shouln't happend, only at the begging
        }
     //   qDebug() << "last parsed GSR measure" << this->lastValue << endl;
        temportalData.clear();
        addpoint(this->lastValue);
     }
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
 //   qDebug() << "The accumulated data is: " << m_readData ;
 //  cout << "Accumulated STR data :" << strData << endl;
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
                 this->resetPlot();
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




void GSRWidget::init_plot()
{
    plot = new QwtPlot(this); //creation of the plot
    plot->setMinimumSize(500,250); //we set the minimum size of the plot

    plot->setTitle("GSR = f(t)"); //title of the graphe

    // axes names
    plot->setAxisTitle(QwtPlot::xBottom, "ticks" );
    plot->setAxisTitle(QwtPlot::yLeft, "GSR");



    // canvas properties
//    plot->canvas()->setLineWidth( 1 );
//    plot->canvas()->setFrameStyle( QFrame::Box | QFrame::Plain );
//    plot->canvas()->setBorderRadius( 3 );

    plot->setLineWidth( 1 );
    plot->setFrameStyle( QFrame::Box | QFrame::Plain );


    //plot->setAxisAutoScale(0,true);
    //plot->setAxisAutoScale(1,true);
    //plot->setBorderRadius( 3 );

    // creation of the curve (you can add more curve to a graphe)
    curve = new QwtPlotCurve;
    curve->setPen(QPen(Qt::red));
    curve->attach(plot);
}


void GSRWidget::addpoint(float point)
{ cout << point << endl;
    data_x->push_back(data_x->size());// add the point to the data
    data_y->push_back(point);
    curve->setSamples(*data_x,*data_y);// we set the data to the curve

    plot->replot(); // we redraw the graphe
}

void GSRWidget::resetPlot(){
    data_x->clear();
    data_y->clear();
}
