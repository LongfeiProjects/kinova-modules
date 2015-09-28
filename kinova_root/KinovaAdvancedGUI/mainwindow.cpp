#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QMessageBox>
#include <QMainWindow>
#include <QHBoxLayout>
#include <iostream>
#include "mainwindow.h"


using namespace std;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPushButton *buttonSend = ui->SendCommandButton;
    QObject::connect(buttonSend, SIGNAL(clicked()), this, SLOT(clickedSlot()));

    this->klib = new Kinovalib();
    initGUI();
}

void MainWindow::initGUI(){
    this->opType = CARTESIAN_POSITION;
    ui->typeComboBox->addItem(QString("Cartesian Position"),QVariant(CARTESIAN_POSITION));
    ui->typeComboBox->addItem(QString("Angular Position"),QVariant(ANGULAR_POSITION));
    ui->typeComboBox->addItem(QString("Cartesian Velocity"),QVariant(CARTESIAN_VELOCITY));
    ui->typeComboBox->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::clickedSlot(){
    QMessageBox* msgBox = new QMessageBox();
    msgBox->setWindowTitle("Hello");
    msgBox->setText("Sending command to kinova arm"+ ((QPushButton*)sender())->text());
    msgBox->exec();

    klib->sampleSendCartesianVelocityType();

}


/*  FIXME ACCIONES VIEJAS, BORRARLAS MAS ADELANTE
void MainWindow::on_pushButton_2_clicked()
{
    ui->comoActiveDevice->clear();
    cout << "Scan Devices..." << endl;
    int opRes;
    int devicesCount = klib->scanDevices(opRes);
    if(devicesCount>0){
        KinovaDevice* devices = klib->getScanedDevices();
        for(int i =0;i<devicesCount;i++){
            ui->comoActiveDevice->addItem(devices[i].SerialNumber);
        }
    }
}


void MainWindow::on_comoActiveDevice_currentTextChanged(const QString &arg1){
     KinovaDevice* devices = klib->getScanedDevices();
     for(int i =0;i<this->klib->getDevicesCount();i++){
         QString qstr = QString(devices[i].SerialNumber);
         if(QString::compare(arg1,qstr) == 0){
             cout << "Seting "+ qstr.toStdString() + " as active device" << endl;
            this->klib->setActiveDevice(devices[i]);
            break;
         }
     }
}
*/

void MainWindow::on_homeButton_clicked()
{
    cout << "Moving home" << endl;
    klib->moveHome();
}

void MainWindow::on_typeComboBox_currentIndexChanged(int index)
{
    int* data = (int*)this->ui->typeComboBox->currentData().data();
    this->opType=*data;
}
