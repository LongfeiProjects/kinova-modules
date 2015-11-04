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

    QPushButton *buttonSend = ui->SendCommandButton;
    QObject::connect(buttonSend, SIGNAL(clicked()), this, SLOT(clickedSlot()));

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


void MainWindow::changeOpType(int opType){
    this->opType=opType;
    if(opType==CARTESIAN_POSITION){
        this->ui->pushButton->setEnabled(true);
        this->ui->ArmControl_Widget->setTitle(QString("Arm Control (position units)"));
        this->ui->FingersControl_Widget->setTitle(QString("Finger Control (position units)"));

        this->ui->labelArmControlX->setText("mts");
        this->ui->labelArmControlY->setText("mts");
        this->ui->labelArmControlZ->setText("mts");

        this->ui->labelArmControlX_2->setText("rad");
        this->ui->labelArmControlY_2->setText("rad");
        this->ui->labelArmControlZ_2->setText("rad");

    }else if(opType==CARTESIAN_VELOCITY){
        this->ui->pushButton->setDisabled(true);
        this->ui->ArmControl_Widget->setTitle(QString("Arm Control (velocity units)"));
        this->ui->FingersControl_Widget->setTitle(QString("Finger Control (velocity units)"));

        this->ui->labelArmControlX->setText("mts/s");
        this->ui->labelArmControlY->setText("mts/s");
        this->ui->labelArmControlZ->setText("mts/s");

        this->ui->labelArmControlX_2->setText("rad/s");
        this->ui->labelArmControlY_2->setText("rad/s");
        this->ui->labelArmControlZ_2->setText("rad/s");
    }
}

void MainWindow::initGUI(){
    this->changeOpType(CARTESIAN_POSITION);
    ui->typeComboBox->addItem(QString("Cartesian Position"),QVariant(CARTESIAN_POSITION));
    //ui->typeComboBox->addItem(QString("Angular Position"),QVariant(ANGULAR_POSITION));
    ui->typeComboBox->addItem(QString("Cartesian Velocity"),QVariant(CARTESIAN_VELOCITY));
    ui->typeComboBox->setCurrentIndex(0);

    this->armCommand=true;
    this->fingerCommand=true;

    this->kinova_initialized = false;
    this->isRecordedTrajecory=false;

    int numPresetPositions = 0;
    this->presetPositions = klib->getHandPresetPositions(numPresetPositions);
    for(int i=0;i<numPresetPositions;i++){
        this->ui->presetHandPositions->addItem(QString(presetPositions[i].name.data()));
    }

    this->ui->speedComboBox->addItem("precision", PRECISION_SPEED);
    this->ui->speedComboBox->addItem("low", LOW_SPEED);
    this->ui->speedComboBox->addItem("medium", MEDIUM_SPEED);
    this->ui->speedComboBox->addItem("high", HIGH_SPEED);
    this->ui->speedComboBox->setCurrentIndex(1);
    this->speed= LOW_SPEED;

    /************************************   Save menu *****************************************/
    this->ui->saveToolButton->addAction(this->ui->actionSave_Arm_Current_Position);
    this->ui->saveToolButton->addAction(this->ui->actionSave_current_fingers_position);
    this->ui->saveToolButton->addAction(this->ui->actionSave_full_position_arm_fingers);
    this->ui->saveToolButton->addAction(this->ui->actionSave_Recorded_Trajectory);


    this->ui->actionSave_Recorded_Trajectory->setEnabled(this->isRecordedTrajecory);
    this->ui->actionSave_Arm_Current_Position->setEnabled(kinova_initialized);
    this->ui->actionSave_current_fingers_position->setEnabled(kinova_initialized);
    this->ui->actionSave_full_position_arm_fingers->setEnabled(kinova_initialized);


    QObject::connect(this->ui->actionSave_Arm_Current_Position, SIGNAL(triggered()),
                                    this, SLOT(save_arm_position()));
    QObject::connect(this->ui->actionSave_current_fingers_position, SIGNAL(triggered()),
                                      this, SLOT(save_fingers_position()));
    QObject::connect(this->ui->actionSave_full_position_arm_fingers, SIGNAL(triggered()),
                                      this, SLOT(save_full_position()));
    QObject::connect(this->ui->actionSave_Recorded_Trajectory, SIGNAL(triggered()),
                                      this, SLOT(save_recorded_trajectory()));

    /*************************************End save menu**********************************/

    /*********************************** Recorded Trajectories panel ******************/
    this->recordedTrajectories = this->sqlManager->getTrajectoriesInfo();

    QGraphicsWidget* container = new QGraphicsWidget();
    FlowLayout *lay = new FlowLayout;

    int row = 0;
    int col = 0;
    QWidget* gridLayoutWidget_6 = new QWidget(this->ui->playBox);
    gridLayoutWidget_6->setObjectName(QStringLiteral("gridLayoutWidget_6"));
    gridLayoutWidget_6->setGeometry(QRect(QPoint(0, 25), QSize(800, 270) ));

    QGridLayout* gridPlayPanel = new QGridLayout(gridLayoutWidget_6);

    //gridPlayPanel->setSpacing(6);
    //gridPlayPanel->setContentsMargins(5, 5, 5, 5);
    //gridPlayPanel->setObjectName(QStringLiteral("gridPlayPanel"));
    //gridPlayPanel->setGeometry(QRect(QPoint(0, 0), QSize(600, 300) ));

    /*For each recorded trajectory we create an Button with the play icon*/
    for(vector<Trajectory>::iterator iter = recordedTrajectories.begin(); iter!=recordedTrajectories.end();++iter){
        Trajectory t = *iter;

        //Add push button
        QPushButton* pushButton = new QPushButton(gridLayoutWidget_6);
       // pushButton->setMinimumSize(QSize(60, 60));
        pushButton->setMaximumSize(QSize(120, 120));
      //  pushButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy);

        QIcon icon8;
        icon8.addFile(QStringLiteral(":/imagenes/img/play.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton->setIcon(icon8);
        pushButton->setIconSize(QSize(60, 60));
      //  pushButton->setGeometry(0,0,55,55);


        gridPlayPanel->addWidget(pushButton, row, col);

        //add label
        QLabel* label = new QLabel(gridLayoutWidget_6);
        label->setText(QString::fromStdString(t.name));
        label->setMinimumSize(QSize(16777215, 20));
        label->setMaximumSize(QSize(16777215, 20));
        gridPlayPanel->addWidget(label, row+1, col, 1, 1, Qt::AlignHCenter);


        if(col==MAX_COLUMNS_PLAY_PANEL-1){
            col=0;
            row= row+2;
        }else{
            col ++;
        }
    }
    container->setLayout(lay);


    /*Hide recording label*/
    this->ui->recordingLabel->setVisible(false);
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
            msgBox->setWindowTitle("Send Command");
            msgBox->setText("Something went wrong, couldn't get the actual position of the robot");
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
    cout << "Moving home" << endl;
    int res = klib->moveHome();
    if(res!=SUCCESS){
        QMessageBox* msgBox = new QMessageBox();
        msgBox->setWindowTitle("Send Command");
        msgBox->setText("Couldn't go home");
        msgBox->exec();
    }
}

void MainWindow::on_typeComboBox_currentIndexChanged(int index)
{
    int* data = (int*)this->ui->typeComboBox->currentData().data();
    this->changeOpType(*data);
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

void MainWindow::on_pushButton_clicked()
{
    int res = this->klib->getActualCartesianPosition(this->actualPosition);
    if(res==SUCCESS){
        this->ui->xDoubleSpinBox->setValue(actualPosition.Coordinates.X);
        this->ui->yDoubleSpinBox->setValue(actualPosition.Coordinates.Y);
        this->ui->zDoubleSpinBox->setValue(actualPosition.Coordinates.Z);
        this->ui->xDoubleSpinBox_2->setValue(actualPosition.Coordinates.ThetaX);
        this->ui->yDoubleSpinBox_2->setValue(actualPosition.Coordinates.ThetaY);
        this->ui->zDoubleSpinBox_2->setValue(actualPosition.Coordinates.ThetaZ);

        this->ui->finger1DoubleSpinBox->setValue(this->actualPosition.Fingers.Finger1);
        this->ui->finger2DoubleSpinBox->setValue(this->actualPosition.Fingers.Finger2);
        this->ui->finger3DoubleSpinBox->setValue(this->actualPosition.Fingers.Finger3);
    }else{
        QMessageBox* msgBox = new QMessageBox();
        msgBox->setWindowTitle("Send Command");
        msgBox->setText("Couldn't get the actual position of the robot");
        msgBox->exec();
    }
}

void MainWindow::on_finger1DoubleSpinBox_valueChanged(double arg1)
{
    this->point.Position.Fingers.Finger1=arg1;
    this->ui->finger1_slider->setValue(arg1);
}

void MainWindow::on_finger2DoubleSpinBox_valueChanged(double arg1)
{
    this->point.Position.Fingers.Finger2=arg1;
    this->ui->finger2_slider->setValue(arg1);
}


void MainWindow::on_finger3DoubleSpinBox_valueChanged(double arg1)
{
    this->point.Position.Fingers.Finger3=arg1;
    this->ui->finger3_slider->setValue(arg1);
}


void MainWindow::loopSendVelocityCommad(int direction){
    if(!this->stopedTimers[direction-1]){ //we have to use a bool to know I we stopped the timer  because the timer->stop() is not immediate
       cout << "en el loop --> enviar comandos de velocidad: "  << this->fixedStepsCounter[direction-1] <<endl;
       if(direction==Close || direction==Open){
           speed=30; //Fixed speed, the unit velocity of the fingers is not clear at all!
       }
       this->klib->moveSingleStep(direction,this->speed);//put the speed from the gui
    }
}


void MainWindow::on_rightButton_pressed()
{
    cout<<"rightPressed"<<endl;
     this->stopedTimers[Right-1] = false;

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
        this->ui->status_label->setText(QString("Success!"));
        this->kinova_initialized = true;
    }else{
        this->ui->status_label->setText(QString("Initialization wrong, try again"));
    }
}


void MainWindow::error_kinova_not_initialized(){
    int res = klib->moveHome();
    if(res!=SUCCESS){
        QMessageBox* msgBox = new QMessageBox();
        msgBox->setWindowTitle("Initialization Error");
        msgBox->setText("Kinova is not initilized. Try Init Kinova before running any command");
        msgBox->exec();
    }
}

void MainWindow::on_finger1_slider_sliderMoved(int position)
{
    this->ui->finger1DoubleSpinBox->setValue(position);
}

void MainWindow::on_finger2_slider_sliderMoved(int position)
{
    this->ui->finger2DoubleSpinBox->setValue(position);
}

void MainWindow::on_finger3_slider_sliderMoved(int position)
{
    this->ui->finger3DoubleSpinBox->setValue(position);
}



void MainWindow::on_presetHandPositions_currentIndexChanged(int index)
{
    HandPosition pos = this->presetPositions[index];
    this->ui->finger1DoubleSpinBox->setValue(pos.fingers.Finger1);
    this->ui->finger2DoubleSpinBox->setValue(pos.fingers.Finger2);
    this->ui->finger3DoubleSpinBox->setValue(pos.fingers.Finger3);
}


void MainWindow::on_speedComboBox_currentIndexChanged(const QString &arg1)
{
    if(QString::compare(arg1,QString("low")) == 0 ){
        this->speed = LOW_SPEED;
    }else if(QString::compare(arg1,QString("precision")) == 0 ){
        this->speed = PRECISION_SPEED;
    }else if(QString::compare(arg1,QString("medium")) == 0 ){
        this->speed = MEDIUM_SPEED;
    }else if(QString::compare(arg1,QString("high")) == 0 ){
        this->speed = HIGH_SPEED;
   }
}


/****************************Save menu actions**********************************/


void MainWindow::save_arm_position(){
    //TODO
}

void MainWindow::save_fingers_position(){
    //TODO
}

void MainWindow::save_full_position(){
    //TODO
}

void MainWindow::save_recorded_trajectory(){
    //TODO
}

void MainWindow::showSaveTrajectoryPanel(){
   Dialog* dialog2 = new Dialog();
   dialog2->init(this->sqlManager);
   Trajectory saved =  dialog2->execAndReturnSavedTrajectory();
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
        this->ui->label_record_stop->setText(QString("Stop"));
        this->ui->record_Button->setIcon(QIcon(":/imagenes/img/stop.png"));
        this->ui->recordingLabel->setVisible(true);
    }else{
        this->ui->label_record_stop->setText(QString("Record"));
        this->ui->record_Button->setIcon(QIcon(":/imagenes/img/record.png"));

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,"Save","Do you want to save the recorded trajectory?", QMessageBox::Yes|QMessageBox::No);
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



