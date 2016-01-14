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

    this->gsrwidget = new GSRWidget();
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

void MainWindow::showInfoMessage(QString msg){
    QMessageBox* msgBox = new QMessageBox(this);
    msgBox->setWindowTitle(tr("Info"));
    msgBox->setText(msg);
    msgBox->exec();
    delete msgBox;
}


bool MainWindow::showQuestion(string questionTitle, string msg){
    return QMessageBox::question(this,tr(questionTitle.data()),tr(msg.data()), QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes;
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
    //Save this data in a file TODO
    QByteArray gsrData = this->gsrwidget->getData();
    time_t initialGSRTimestamp = this->gsrwidget->getInitialTimestamp();
    string gsrStringData = gsrData.toStdString();
    Logger * logger =  new Logger();
    logger->dumpEvents(true,gsrStringData,initialGSRTimestamp,this->performedTasks,this->participantId,this->initExperimentTime,this->endExperimentTime);
    delete logger;

    showInfoMessage(tr("The experiment is ended. The log data was saved"));
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
        time(&this->endExperimentTime);
        this->ui->gsrStartButton->click();
        this->experimentStarted = false;
        this->ui->experimentStatusFrame->setStyleSheet(QStringLiteral("image: url(:/img/wrong.png);"));

        this->ui->participantIdSpinBox->setValue(-1);
        this->ui->participantIdtextEdit->setText("");
        this->ui->participantIdIconFrame->setStyleSheet(QStringLiteral("image: url(:/img/wrong.png);"));
        this->ui->startExperimentButton->setText(tr("Start Experiment"));
        this->ui->taskManagementGroupBox->setDisabled(true);

        this->ui->listFinishedTasks->clear();
        saveExperimentalData();
        this->participantId = -1;
    }
}

void MainWindow::on_startTask_clicked()
{ 
    if(!this->runningTask){
        bool cont = true;
        QList<QListWidgetItem *> items = this->ui->listFinishedTasks->findItems(QString::fromStdString(this->ui->taskscomboBox->currentText().toStdString()), Qt::MatchExactly);
        if(items.size()>0){
            cont = showQuestion("Task already performed", "This task was already performed by this participant. Do you want to continue anyway?");
        }
        if(cont){
            this->runningTask = true;
            this->ui->startTask->setText(tr("Finish Task"));
            time(&actualTask.initTimestamp);
            actualTask.taskName = this->ui->taskscomboBox->currentText().toStdString();
            this->ui->taskscomboBox->setDisabled(true);
        }
    }else{
        time(&actualTask.endTimestamp);

        DialogTaskFinished* dialog = new DialogTaskFinished(this,actualTask.taskName);
        dialog->exec();
        actualTask.giveUp = dialog->getGiveUpStr();
        actualTask.comments = dialog->getComments();
        delete dialog;

        this->performedTasks.push_back(actualTask);
        this->ui->listFinishedTasks->addItem(QString::fromStdString(actualTask.taskName));
        this->ui->startTask->setText(tr("Start Task"));
        this->ui->taskscomboBox->setDisabled(false);
        this->runningTask=false;

    }
}



void MainWindow::on_gsrStartButton_clicked(bool checked)
{
    if(checked){
        this->ui->skinStatusText->setText(tr("Starting..."));
        this->ui->skinConductanceIconFrame->setStyleSheet(QStringLiteral("image: url(:/img/wait.png);"));
        this->ui->gsrStartButton->setDisabled(true);
        QApplication::processEvents( QEventLoop::ExcludeUserInputEvents);
        if(!this->gsrwidget->startGSR()){
            this->ui->skinStatusText->setText(tr("Error"));
            showErrorMessage(tr("Error starting skin conductance measurement"));
        }else{
            this->ui->skinConductanceIconFrame->setStyleSheet(QStringLiteral("image: url(:/img/ok.png);"));
            this->ui->skinStatusText->setText(tr("Enabled"));
            this->ui->gsrStartButton->setText("Stop");
        }
        this->ui->gsrStartButton->setDisabled(false);
    }else{
        this->gsrwidget->stopGSR();
        this->ui->skinConductanceIconFrame->setStyleSheet(QStringLiteral("image: url(:/img/wrong.png);"));
        this->ui->skinStatusText->setText(tr("Disabled"));
        this->ui->gsrStartButton->setText("Start");
    }
}

void MainWindow::on_setButton_2_clicked(bool checked)
{
    if(checked){
        this->gsrwidget->show();
    }else{
        this->gsrwidget->hide();
    }
}
