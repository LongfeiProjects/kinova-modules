#include "mainwindow.h"

#include <QPushButton>
#include <QMessageBox>
#include <QMainWindow>
#include <QHBoxLayout>
#include <iostream>
#include <future>

using namespace std;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    this->moveDownTimer = new QTimer(this);
    this->moveLeftTimer = new QTimer(this);
    this->moveUpTimer = new QTimer(this);
    this->moveRightTimer = new QTimer(this);
    this->movePushTimer = new QTimer(this);
    this->movePullTimer = new QTimer(this);
    this->openHandTimer = new QTimer(this);
    this->closeHandTimer = new QTimer(this);
    this->klib = new Kinovalib();
    this->sqlManager = new SqlManager();
    this->sqlManager->init();
    this->point.InitStruct();
    initGUI();
}




void MainWindow::initGUI(){
    this->armCommand=true;
    this->fingerCommand=true;

    this->kinova_initialized = false;
    this->isRecordedTrajecory=false;

    this->ui->speedComboBox->addItem(tr("precision"), PRECISION_SPEED);
    this->ui->speedComboBox->addItem(tr("low"), LOW_SPEED);
    this->ui->speedComboBox->addItem(tr("medium"), MEDIUM_SPEED);
    this->ui->speedComboBox->addItem(tr("high"), HIGH_SPEED);
    this->ui->speedComboBox->setCurrentIndex(1);
    this->speed= LOW_SPEED;

    /*********************************** Recorded Trajectories panel ******************/
    this->recordedTrajectories = this->sqlManager->getTrajectoriesInfo();


    int row = 0;
    int col = 0;
    this->gridLayoutWidget_6 = new QWidget(this->ui->playBox);
    this->gridLayoutWidget_6->setObjectName(QStringLiteral("gridLayoutWidget_6"));
    this->gridLayoutWidget_6->setGeometry(QRect(QPoint(0, 25), QSize(800, 270) ));

    this->gridPlayPanel = new QGridLayout(gridLayoutWidget_6);

    /*For each recorded trajectory we create an Button with the play icon*/
    for(vector<Trajectory>::iterator iter = this->recordedTrajectories.begin(); iter!=this->recordedTrajectories.end();++iter){
        Trajectory t = *iter;

        this->addTrajectory(t,col,row);
        if(col==MAX_COLUMNS_PLAY_PANEL-1){
            col=0;
            row= row+2;
        }else{
            col ++;
        }
    }


    /*Hide recording label*/
    this->ui->recordingLabel->setVisible(false);

    /*Disable Undo button*/
    this->ui->undoButton->setEnabled(false);

}


QLatin1String getLabelStyle(){
    return QLatin1String("background-color: rgb(51, 51, 51);\n"
                         "color: rgb(247, 247, 246);\n"
                         "");
}

QFont getLabelFont(){
    QFont font2;
    font2.setBold(true);
    font2.setWeight(90);
//    font2.set
    font2.setPointSize(15);
    return font2;
}

void MainWindow::addTrajectory(Trajectory t, int col, int row){
    //Add push button
    QPushButton* pushButton = new QPushButton(this->gridLayoutWidget_6);
    pushButton->setMaximumSize(QSize(120, 120));

    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
    pushButton->setSizePolicy(sizePolicy);

    QIcon icon8;
    icon8.addFile(QStringLiteral(":/imagenes/img/play.png"), QSize(), QIcon::Normal, QIcon::Off);
    pushButton->setIcon(icon8);
    pushButton->setIconSize(QSize(75, 75));


    this->gridPlayPanel->addWidget(pushButton, row, col);

    //add label
    QLabel* label = new QLabel(this->gridLayoutWidget_6);
    label->setText(QString::fromStdString(t.name));
    label->setMinimumSize(QSize(120, 40));
    label->setMaximumSize(QSize(16777215, 40));
    label->setStyleSheet(getLabelStyle());
    label->setFont(getLabelFont());
    label->setAlignment(Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignCenter);
    label->setMargin(5);
    this->gridPlayPanel->addWidget(label, row+1, col, 1, 1, Qt::AlignHCenter);
}



MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::clickedSlot(){
    cout<<"Sending command to kinova arm"<<endl;
    bool actualPositionObtained = true;
    if(!this->armCommand){
        /*As the Kinova API does not provide any finger only command, then we simulate it by sending the actual coordinates of the arm.
        So, here we load the actual position and then send the command with the finger parameters*/
        int res = this->klib->getActualCartesianPosition(this->actualPosition);
        if(res==SUCCESS){
            this->point.Position.CartesianPosition.X = actualPosition.Coordinates.X;
            this->point.Position.CartesianPosition.Y = actualPosition.Coordinates.Y;
            this->point.Position.CartesianPosition.Z = actualPosition.Coordinates.Z;
            this->point.Position.CartesianPosition.ThetaX= actualPosition.Coordinates.ThetaX;
            this->point.Position.CartesianPosition.ThetaY=actualPosition.Coordinates.ThetaY;
            this->point.Position.CartesianPosition.ThetaZ=actualPosition.Coordinates.ThetaZ;
        }else{
            QMessageBox* msgBox = new QMessageBox();
            msgBox->setWindowTitle(tr("Send Command"));
            msgBox->setText(tr("Something went wrong, couldn't get the actual position of the robot"));
            msgBox->exec();
            actualPositionObtained=false;
        }
    }
    if(actualPositionObtained){
        klib->sendCommand(this->opType,this->armCommand,this->fingerCommand,this->point);
    }
}

void MainWindow::on_homeButton_clicked()
{
    cout << "Moving home"<< endl;
    int res = klib->moveHome();
    if(res!=SUCCESS){
        QMessageBox* msgBox = new QMessageBox();
        msgBox->setWindowTitle(tr("Send Command"));
        msgBox->setText(tr("Couldn't go home"));
        msgBox->exec();
    }
}


void MainWindow::on_xDoubleSpinBox_valueChanged(double arg1)
{
    this->point.Position.CartesianPosition.X=arg1;
}

void MainWindow::on_yDoubleSpinBox_valueChanged(double arg1)
{
    this->point.Position.CartesianPosition.Y=arg1;
}

void MainWindow::on_zDoubleSpinBox_valueChanged(double arg1)
{
    this->point.Position.CartesianPosition.Z=arg1;
}

void MainWindow::on_xDoubleSpinBox_2_valueChanged(double arg1)
{
    this->point.Position.CartesianPosition.ThetaX=arg1;
}

void MainWindow::on_yDoubleSpinBox_2_valueChanged(double arg1)
{
    this->point.Position.CartesianPosition.ThetaY=arg1;
}

void MainWindow::on_zDoubleSpinBox_2_valueChanged(double arg1)
{
    this->point.Position.CartesianPosition.ThetaZ=arg1;
}

void MainWindow::on_speed1DoubleSpinBox_valueChanged(double arg1)
{
    this->point.Limitations.speedParameter1=arg1;
}

void MainWindow::on_speed2DoubleSpinBox_valueChanged(double arg1)
{
    this->point.Limitations.speedParameter2=arg1;
}

void MainWindow::on_limitationsGroup_toggled(bool arg1)
{
    this->point.LimitationsActive= (int)arg1;
}

void MainWindow::on_ArmControl_Widget_toggled(bool arg1)
{
    this->armCommand=arg1;
}

void MainWindow::on_FingersControl_Widget_toggled(bool arg1)
{
    this->fingerCommand=arg1;
}



void MainWindow::loopSendVelocityCommad(int direction){
    if(!this->stopedTimers[direction-1]){ //we have to use a bool to know I we stopped the timer  because the timer->stop() is not immediate
       cout << "en el loop --> enviar comandos de velocidad: "  << this->fixedStepsCounter[direction-1] <<endl;
       if(direction==Close || direction==Open){
           this->klib->moveSingleStep(direction,30); //Fixed speed, the unit velocity of the fingers is not clear at all!
       }else{
            this->klib->moveSingleStep(direction,this->speed);
       }
    }
}


void MainWindow::on_rightButton_pressed()
{
    cout<<"rightPressed"<<endl;
     this->stopedTimers[Right-1] = false;

    //Save actual position as a checkpoint
    this->saveCheckPoint();

    //We use QSignal mapper to send a parameter to the slot loopSendVelocityCommand. This parameter is the movement direction
    QSignalMapper* signalMapper = new QSignalMapper (this) ;
    connect (this->moveRightTimer, SIGNAL(timeout()), signalMapper, SLOT(map())) ;

    signalMapper -> setMapping (this->moveRightTimer, Right) ;
    connect (signalMapper, SIGNAL(mapped(int)), this, SLOT(loopSendVelocityCommad(int)));

    this->moveRightTimer->start(LOOP_SPEED_SEND_VELOCITY_COMMAND);//TODO Este valor 1000 debería depender de la velociad.
}


void MainWindow::on_rightButton_released()
{
    cout << "right button released" << endl;
    this->moveRightTimer->stop();
    cout<<"right timer stopped"<<endl;
    this->stopedTimers[Right-1] = true;
}

void MainWindow::on_upButton_pressed()
{
    this->stopedTimers[Up-1] = false;

    //Save actual position as a checkpoint
    this->saveCheckPoint();
    //We use QSignal mapper to send a parameter to the slot loopSendVelocityCommand. This parameter is the movement direction
    QSignalMapper* signalMapper = new QSignalMapper (this) ;
    connect (this->moveUpTimer, SIGNAL(timeout()), signalMapper, SLOT(map())) ;
    signalMapper->setMapping (this->moveUpTimer, Up) ;
    connect (signalMapper, SIGNAL(mapped(int)), this, SLOT(loopSendVelocityCommad(int)));
    this->moveUpTimer->start(LOOP_SPEED_SEND_VELOCITY_COMMAND);
}

void MainWindow::on_upButton_released()
{
    this->moveUpTimer->stop();
    this->stopedTimers[Up-1] = true;

}

void MainWindow::on_leftButton_pressed()
{
    this->stopedTimers[Left-1] = false;

    //Save actual position as a checkpoint
    this->saveCheckPoint();

    //We use QSignal mapper to send a parameter to the slot loopSendVelocityCommand. This parameter is the movement direction
    QSignalMapper* signalMapper = new QSignalMapper (this) ;
    connect (this->moveLeftTimer, SIGNAL(timeout()), signalMapper, SLOT(map())) ;

    signalMapper -> setMapping (this->moveLeftTimer, Left) ;
    connect (signalMapper, SIGNAL(mapped(int)), this, SLOT(loopSendVelocityCommad(int)));
    this->moveLeftTimer->start(LOOP_SPEED_SEND_VELOCITY_COMMAND);
}

void MainWindow::on_leftButton_released()
{
    this->moveLeftTimer->stop();
    this->stopedTimers[Left-1] = true;

}

void MainWindow::on_downButton_pressed()
{
    this->stopedTimers[Down-1] = false;

    //Save actual position as a checkpoint
    this->saveCheckPoint();

    //We use QSignal mapper to send a parameter to the slot loopSendVelocityCommand. This parameter is the movement direction
    QSignalMapper* signalMapper = new QSignalMapper (this) ;
    delete this->moveDownTimer;
    this->moveDownTimer = new QTimer(this);
    connect (this->moveDownTimer, SIGNAL(timeout()), signalMapper, SLOT(map())) ;

    signalMapper -> setMapping (this->moveDownTimer, Down) ;
    connect (signalMapper, SIGNAL(mapped(int)), this, SLOT(loopSendVelocityCommad(int)));
    this->moveDownTimer->start(LOOP_SPEED_SEND_VELOCITY_COMMAND);
}

void MainWindow::on_downButton_released()
{
   this->moveDownTimer->stop();
   this->stopedTimers[Down-1] = true;
}



void MainWindow::on_pushButton_Y_pressed()
{

    this->stopedTimers[Push-1] = false;
    //We use QSignal mapper to send a parameter to the slot loopSendVelocityCommand. This parameter is the movement direction

    //Save actual position as a checkpoint
    this->saveCheckPoint();

    QSignalMapper* signalMapper = new QSignalMapper (this) ;
    delete this->movePushTimer;
    this->movePushTimer = new QTimer(this);
    connect (this->movePushTimer, SIGNAL(timeout()), signalMapper, SLOT(map())) ;

    signalMapper -> setMapping (this->movePushTimer, Push) ;
    connect (signalMapper, SIGNAL(mapped(int)), this, SLOT(loopSendVelocityCommad(int)));
    this->movePushTimer->start(LOOP_SPEED_SEND_VELOCITY_COMMAND);
}

void MainWindow::on_pushButton_Y_released()
{
    this->movePushTimer->stop();
    this->stopedTimers[Push-1] = true;

}


void MainWindow::on_pullButton_Y_pressed()
{
    this->stopedTimers[Pull-1] = false;

    //Save actual position as a checkpoint
    this->saveCheckPoint();

    //We use QSignal mapper to send a parameter to the slot loopSendVelocityCommand. This parameter is the movement direction
    QSignalMapper* signalMapper = new QSignalMapper (this) ;
    delete this->movePullTimer;
    this->movePullTimer = new QTimer(this);
    connect (this->movePullTimer, SIGNAL(timeout()), signalMapper, SLOT(map())) ;

    signalMapper -> setMapping (this->movePullTimer, Pull) ;
    connect (signalMapper, SIGNAL(mapped(int)), this, SLOT(loopSendVelocityCommad(int)));
    this->movePullTimer->start(LOOP_SPEED_SEND_VELOCITY_COMMAND);
}

void MainWindow::on_pullButton_Y_released()
{
    this->movePullTimer->stop();
    this->stopedTimers[Pull-1] = true;

}



/*Real time Hand Control*/
void MainWindow::on_openHandButton_pressed(){
    this->stopedTimers[Open-1] = false;

    //Save actual position as a checkpoint
    this->saveCheckPoint();

    //We use QSignal mapper to send a parameter to the slot loopSendVelocityCommand. This parameter is the movement direction
    QSignalMapper* signalMapper = new QSignalMapper (this) ;
    delete this->openHandTimer;
    this->openHandTimer = new QTimer(this);
    connect (this->openHandTimer, SIGNAL(timeout()), signalMapper, SLOT(map())) ;

    signalMapper -> setMapping (this->openHandTimer, Open) ;
    connect (signalMapper, SIGNAL(mapped(int)), this, SLOT(loopSendVelocityCommad(int)));
    this->openHandTimer->start(LOOP_SPEED_SEND_VELOCITY_COMMAND);
}

void MainWindow::on_openHandButton_released()
{
    this->openHandTimer->stop();
    this->stopedTimers[Open-1] = true;
}


void MainWindow::on_closeHandButton_pressed(){
    this->stopedTimers[Close-1] = false;

    //Save actual position as a checkpoint
    this->saveCheckPoint();

    //We use QSignal mapper to send a parameter to the slot loopSendVelocityCommand. This parameter is the movement direction
    QSignalMapper* signalMapper = new QSignalMapper (this) ;
    delete this->closeHandTimer;
    this->closeHandTimer = new QTimer(this);
    connect (this->closeHandTimer, SIGNAL(timeout()), signalMapper, SLOT(map())) ;

    signalMapper -> setMapping (this->closeHandTimer, Close) ;
    connect (signalMapper, SIGNAL(mapped(int)), this, SLOT(loopSendVelocityCommad(int)));
    this->closeHandTimer->start(LOOP_SPEED_SEND_VELOCITY_COMMAND);
}

void MainWindow::on_closeHandButton_released()
{
    this->closeHandTimer->stop();
    this->stopedTimers[Close-1] = true;
}


void MainWindow::on_pushButton_2_clicked()
{
    cout << "Initilizating kinova" << endl;
    int res = this->klib->kinovaInit();
    if(res == SUCCESS){
        this->ui->status_icon->setStyleSheet("image: url(:/imagenes/img/ok_icon.png);");
        this->ui->status_label->setText(QString(tr("Success!")));
        this->kinova_initialized = true;
    }else{
        this->ui->status_label->setText(QString(tr("Initialization wrong, try again")));
    }
}


void MainWindow::error_kinova_not_initialized(){
    int res = klib->moveHome();
    if(res!=SUCCESS){
        QMessageBox* msgBox = new QMessageBox();
        msgBox->setWindowTitle(tr("Initialization Error"));
        msgBox->setText(tr("Kinova is not initilized. Try Init Kinova before running any command"));
        msgBox->exec();
    }
}


void MainWindow::on_speedComboBox_currentIndexChanged(const QString &arg1)
{
    if(QString::compare(arg1,QString(tr("low"))) == 0 ){
        this->speed = LOW_SPEED;
    }else if(QString::compare(arg1,QString(tr("precision"))) == 0 ){
        this->speed = PRECISION_SPEED;
    }else if(QString::compare(arg1,QString(tr("medium"))) == 0 ){
        this->speed = MEDIUM_SPEED;
    }else if(QString::compare(arg1,QString(tr("high"))) == 0 ){
        this->speed = HIGH_SPEED;
   }
}




/*Add the recorded trajectory to the list of trajectories and also to the panel*/
void MainWindow::addRecordedTrajectory(Trajectory t){
    this->recordedTrajectories.push_back(t);
    int row, col;
    int cant_trajectories = this->recordedTrajectories.size();
    row = cant_trajectories/MAX_COLUMNS_PLAY_PANEL;
    row=row*2;
    col = cant_trajectories%MAX_COLUMNS_PLAY_PANEL;
    if(col==0){
        col=MAX_COLUMNS_PLAY_PANEL-1;
        row-=2;
    }else{
        col=col-1;
    }
    this->addTrajectory(t,col,row);
}

void MainWindow::showSaveTrajectoryPanel(){
   Dialog* dialog2 = new Dialog();
   dialog2->init(this->sqlManager);
   Trajectory saved =  dialog2->execAndReturnSavedTrajectory();

   this->addRecordedTrajectory(saved);
   delete dialog2;
}


void tarea(string msg){
    for(int i=0;i<10;i++){
        cout << "task1 " << msg <<endl;
        sleep(1);
    }
}

void tarea1(){
    for(int i=0;i<10;i++){
        cout << "task1 " << endl;
        sleep(1);
    }
}

void MainWindow::on_record_Button_toggled(bool checked)
{
    if(checked){
        this->ui->label_record_stop->setText(QString(tr("Stop")));
        this->ui->record_Button->setIcon(QIcon(":/imagenes/img/stop.png"));
        this->ui->recordingLabel->setVisible(true);
    }else{
        this->ui->label_record_stop->setText(QString(tr("Record")));
        this->ui->record_Button->setIcon(QIcon(":/imagenes/img/record.png"));

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,tr("Save"),tr("Do you want to save the recorded trajectory?"), QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes){
            showSaveTrajectoryPanel();
        }
        this->ui->recordingLabel->setVisible(false);
    }


    //TODO launch a status-read thread. This has to be implemented after integrate the openAPI
    //pthread_create()
  /*  if(checked){
        cout << "antes"<<endl;
        std::thread t1(tarea, "Hello");
        cout << "dsps"<<endl;
        t1.detach();
        t1.join();
    }



    std::future<void> result(std::async(tarea1));
*/
}

/*Save the actual position as a checkpoint. This allows the user to undo actions*/
void MainWindow::saveCheckPoint(){
    int res = this->klib->getActualCartesianPosition(this->actualPosition);
    if(res==SUCCESS){
        this->checkpoints.push_back(this->actualPosition);
        this->ui->undoButton->setEnabled(true);
    }else{
        QMessageBox* msgBox = new QMessageBox();
        msgBox->setWindowTitle(tr("Send Command"));
        msgBox->setText(tr("Something went wrong, couldn't get the actual position of the robot"));
        msgBox->exec();
    }
}

void MainWindow::setActualPosition(){
    /*As the Kinova API does not provide any finger only command, then we simulate it by sending the actual coordinates of the arm.
    So, here we load the actual position and then send the command with the finger parameters*/
    int res = this->klib->getActualCartesianPosition(this->actualPosition);
    if(res==SUCCESS){
        this->point.Position.CartesianPosition.X = actualPosition.Coordinates.X;
        this->point.Position.CartesianPosition.Y = actualPosition.Coordinates.Y;
        this->point.Position.CartesianPosition.Z = actualPosition.Coordinates.Z;
        this->point.Position.CartesianPosition.ThetaX= actualPosition.Coordinates.ThetaX;
        this->point.Position.CartesianPosition.ThetaY=actualPosition.Coordinates.ThetaY;
        this->point.Position.CartesianPosition.ThetaZ=actualPosition.Coordinates.ThetaZ;
    }else{
        QMessageBox* msgBox = new QMessageBox();
        msgBox->setWindowTitle(tr("Send Command"));
        msgBox->setText(tr("Something went wrong, couldn't get the actual position of the robot"));
        msgBox->exec();
    }
}

void MainWindow::setHandPull(){
    this->setActualPosition();
    this->point.Position.Fingers.Finger1 = 23;
    this->point.Position.Fingers.Finger2 = 19;
    this->point.Position.Fingers.Finger3 = 29;
    this->point.Position.CartesianPosition.ThetaX = 1.5;
    this->point.Position.CartesianPosition.ThetaY = -0.1;
    this->point.Position.CartesianPosition.ThetaZ = -1.8;

}

void MainWindow::setHandPoint(){
    this->setActualPosition();

    this->point.Position.CartesianPosition.ThetaX=1.5;
    this->point.Position.CartesianPosition.ThetaY=-0.1;
    this->point.Position.CartesianPosition.ThetaZ=0;
    this->point.Position.Fingers.Finger1 = 47;
    this->point.Position.Fingers.Finger2 = 43;
    this->point.Position.Fingers.Finger3 = 54;
}

void MainWindow::setHandGrasp(){
    this->setActualPosition();
    this->point.Position.CartesianPosition.ThetaX = 2.9;
    this->point.Position.CartesianPosition.ThetaY = 0;
    this->point.Position.CartesianPosition.ThetaZ = -1.5;

    this->point.Position.Fingers.Finger1 = 27;
    this->point.Position.Fingers.Finger2 = 25;
    this->point.Position.Fingers.Finger3 = 25;
}

void MainWindow::on_playPointPosition_clicked()
{
    this->setHandPoint();
    klib->sendCommand(CARTESIAN_POSITION,false,true,this->point);
}

void MainWindow::on_playPullPosition_clicked()
{
    this->setHandPull();
    klib->sendCommand(CARTESIAN_POSITION,true,true,this->point);
}

void MainWindow::on_playGraspPosition_clicked()
{
    this->setHandGrasp();
    klib->sendCommand(CARTESIAN_POSITION,true,true,this->point);
}


void MainWindow::on_undoButton_clicked()
{

    this->actualPosition = this->checkpoints.back();
    this->checkpoints.pop_back();

    this->point.Position.CartesianPosition.X = this->actualPosition.Coordinates.X;
    this->point.Position.CartesianPosition.Y = this->actualPosition.Coordinates.Y;
    this->point.Position.CartesianPosition.Z = this->actualPosition.Coordinates.Z;
    this->point.Position.CartesianPosition.ThetaX= this->actualPosition.Coordinates.ThetaX;
    this->point.Position.CartesianPosition.ThetaY=this->actualPosition.Coordinates.ThetaY;
    this->point.Position.CartesianPosition.ThetaZ=this->actualPosition.Coordinates.ThetaZ;
    this->point.Position.Fingers.Finger1 = this->actualPosition.Fingers.Finger1;
    this->point.Position.Fingers.Finger2 = this->actualPosition.Fingers.Finger2;
    this->point.Position.Fingers.Finger3 = this->actualPosition.Fingers.Finger3;

    this->ui->undoButton->setDisabled(this->checkpoints.empty());

    klib->sendCommand(CARTESIAN_POSITION,true,true,this->point);
}
