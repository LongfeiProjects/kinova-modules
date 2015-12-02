#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initGUI();
}

void MainWindow::initGUI(){
    this->ui->participantIdtextEdit->setDisabled(true);
    this->ui->taskManagementGroupBox->setDisabled(true);

    this->ui->taskscomboBox->addItem(tr("1:Turn on light"));
    this->ui->taskscomboBox->addItem(tr("2:Push little switch"));
    this->ui->taskscomboBox->addItem(tr("3:Push big switch"));
    this->ui->taskscomboBox->addItem(tr("4:Open drawer"));
    this->ui->taskscomboBox->addItem(tr("5:Pick and place object"));
    this->ui->taskscomboBox->addItem(tr("6:Open door"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showErrorMessage(QString msg){
    QMessageBox* msgBox = new QMessageBox(this);
    msgBox->setWindowTitle(tr("Error"));
    msgBox->setText(msg);
    msgBox->exec();
    delete msgBox;
}

void MainWindow::on_setButton_clicked()
{
    this->participantId = this->ui->participantIdSpinBox->value();
    if(this->participantId>-1){
           this->ui->participantIdtextEdit->setText(QString::number(this->participantId));
           this->ui->participantIdIconFrame->setStyleSheet(QStringLiteral("image: url(:/img/ok.png);"));
    }else{
        showErrorMessage(tr("Invalid participant ID"));
    }
}


bool MainWindow::checkExperimentPreConditions(){
    bool res = false;
    if(participantId>-1){
        res=true;
    }else{
        showErrorMessage(tr("You must set the participant ID before start the experiment"));
    }
    return res;
}

void MainWindow::saveExperimentalData(){
    //TODO
    showErrorMessage(QString("Dumping data is still not implemented!! Do it!"));
}

void MainWindow::on_startExperimentButton_clicked()
{
    if(!this->experimentStarted){   //We have start the experiment
        if(checkExperimentPreConditions()){
            this->experimentStarted = true;
            this->ui->taskManagementGroupBox->setDisabled(false);
            time(&this->initExperimentTime);
            this->ui->experimentStatusFrame->setStyleSheet(QStringLiteral("image: url(:/img/ok.png);"));
            this->ui->startExperimentButton->setText(tr("Finish Experiment"));
            this->ui->statustextEdit->setText(tr("STARTED!"));
        }
    }else{ //We have to finish the experiment
        this->experimentStarted = false;
        this->ui->experimentStatusFrame->setStyleSheet(QStringLiteral("image: url(:/img/wrong.png);"));
        this->participantId = -1;
        this->ui->participantIdSpinBox->setValue(-1);
        this->ui->participantIdtextEdit->setText("");
        this->ui->participantIdIconFrame->setStyleSheet(QStringLiteral("image: url(:/img/wrong.png);"));
        this->ui->startExperimentButton->setText(tr("Start Experiment"));
        this->ui->taskManagementGroupBox->setDisabled(true);
        saveExperimentalData();
    }
}

void MainWindow::on_startTask_clicked()
{
    if(!this->runningTask){
        this->runningTask = true;
        this->ui->startTask->setText(tr("Finish Task"));
        time_t(&actualTask.initTimestamp);
        actualTask.taskName = this->ui->taskscomboBox->currentText().toStdString();
        this->ui->taskscomboBox->setDisabled(true);
    }else{
        time_t(&actualTask.endTimestamp);

        //TODO Make a Dialog where the experimenter can add commets and also indicate if the task was finished properly

        this->performedTasks.push_back(actualTask);
        this->ui->listFinishedTasks->addItem(QString::fromStdString(actualTask.taskName));
        this->ui->startTask->setText(tr("Start Task"));
        this->ui->taskscomboBox->setDisabled(false);
        this->runningTask=false;

    }
}
