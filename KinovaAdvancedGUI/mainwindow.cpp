#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QMessageBox>
#include <QMainWindow>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPushButton *buttonSend = ui->pushButton;
    QObject::connect(buttonSend, SIGNAL(clicked()), this, SLOT(clickedSlot()));

    this->klib = new Kinovalib();
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

    klib->sampleSend();
}
