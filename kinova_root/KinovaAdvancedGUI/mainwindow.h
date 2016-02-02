#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "guilogger.h"
#include "dialog.h"
#include <QMainWindow>
#include "kinovalib.h"
#include <unistd.h>
#include <QTimer>
#include <QSignalMapper>
#include "sqlmanager.h"
#include <vector>
#include "flowlayout.h"
#include "configdialog.h"
#include <QGraphicsProxyWidget>
#include <thread>
#include "savedialog.h"
#include <kinovalib_openapi_global.h>
#include "robot.hpp"
#include "devices/kinova_controller_openAPI.hpp"
#include "devices/kinova_status_openAPI.hpp"


#define  MAX_COLUMNS_PLAY_PANEL  5
#define KINOVA_LIB 1    // 0: Oficial API , 1: Open API through vmodugno package

namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    void setActualPosition();
    void setHandPull();
    void setHandPoint();
    void setHandGrasp();
    void saveCheckPoint();

    void clearTrajectoryPanel();
    void enableJoystickMode(bool enabled);
private:


   // boost::thread* reader_stats;


    Ui::MainWindow *ui;
  //  QDialogButtonBox* save_Trajectory_Panel_Button;

    //Kinova libraries. Oficial API and vmodugno package
    Kinovalib* klib ;
    robot* bot;

    /*Kinova API stuff*/
    TrajectoryPoint point;
    int opType;
    bool armCommand;
    bool fingerCommand;
    CartesianPosition actualPosition;

    //This is to call the openAPI
    vector<string> readType;
    std::map<string,int> readTypeMap;
    /*Aux functions*/
    void initGUI();
    void changeOpType(int opType);
    QTimer* moveRightTimer;
    QTimer* moveLeftTimer;
    QTimer* moveUpTimer;
    QTimer* moveDownTimer;
    QTimer* movePushTimer;
    QTimer* movePullTimer;
    QTimer* closeHandTimer;
    QTimer* openHandTimer;
    QSignalMapper* signalMapper;

    int fixedStepsCounter[8] = {0,0,0,0,0,0,0,0};
    bool stopedTimers[8] = {false,false,false,false,false,false,false,false};

    int LOOP_SPEED_SEND_VELOCITY_COMMAND = 5; // time in miliseconds.

    bool kinova_initialized;


    HandPosition* presetPositions;



    float speed;
    float    PRECISION_SPEED = 0.04;
    float    LOW_SPEED = 0.07;
    float    MEDIUM_SPEED = 1.1;
    float    HIGH_SPEED = 1.5;

    //FORCE CHECK VARIABLES
    int score;
    int scoreindex;
    vector<int> scorecolection;

    /*Security Check Thread*/
    QTimer* securityCheckTimer;
    int CHECK_FORCE_TIMER = 100; // time in miliseconds.
    int FORCE_MEASURES = 5;   //the force signal will represent the average of FORCE_MEASURES measures


    bool isRecordingTrajecory;
    bool isMoving;
    bool isSpeedIncremented;

    vector<Trajectory> recordedTrajectories;
    vector<CartesianPosition> checkpoints;
    vector<RecordedCartesianInfo> sampledTrajectoryInfo;
    time_t initTimestampTrajectory;

    //delete it
    vector<Log> recordedLogs;

    //GUI auxiliar fields
    QGridLayout* gridPlayPanel;
    QWidget* gridLayoutWidget_6;

    void addTrajectory(Trajectory y, int col, int row);
    void addRecordedTrajectory(Trajectory t);
    void showSaveTrajectoryPanel();
    void error_kinova_not_initialized();


    vector<Log> convertTrajectory2Log(Trajectory traj);
    void startRecording();
    void stopRecording();
    void writeLogFiles(vector<Log> recordedLogs);
    vector<RecordedCartesianInfo> convertLog2Trajectory(vector<Log> logs);




    /*Data for the experiment. Participant id , etc*/
    int participantId;
    string speedToString(float speed);
    GUILogger* logger;
   // void Loop();
    void error_kinova_already_initialized();
private slots:

    void on_homeButton_clicked();


    void loopSendVelocityCommad(int direction);
    void on_upButton_pressed();
    void on_upButton_released();
    void on_leftButton_pressed();
    void on_leftButton_released();
    void on_downButton_pressed();
    void on_downButton_released();
    void on_rightButton_pressed();
    void on_rightButton_released();
    void on_button_rightClick_IncreaseSpeed();
    void on_button_rightClick_DecreaseSpeed();
    void on_initKinovaButton_clicked();
    void on_pushButton_Y_pressed();
    void on_pushButton_Y_released();
    void on_pullButton_Y_pressed();
    void on_pullButton_Y_released();
    void on_speedComboBox_currentIndexChanged(const QString &arg1);
    void on_openHandButton_pressed();
    void on_openHandButton_released();
    void on_closeHandButton_pressed();
    void on_closeHandButton_released();
    void on_record_Button_toggled(bool checked);
    void on_playPointPosition_clicked();
    void on_playPullPosition_clicked();
    void on_playGraspPosition_clicked();
    void on_undoButton_clicked();
    void on_MainWindow_destroyed();
    void playTrajectoryButtonClicked(int trajectoryId);
    void on_configButton_clicked();
    void securityCheckSlot();



protected:

    /*Delete this, we don't need it*/

    /*bool eventFilter(QObject *obj, QEvent *event){
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            switch(keyEvent->key()){
                case Qt::Key_1:
                    this->force = 0;
                break;
                case Qt::Key_2:
                    this->force = 0.2;
                break;
                case Qt::Key_3:
                    this->force = 0.4;
                break;
                case Qt::Key_4:
                    this->force = 0.6;
                break;
                case Qt::Key_5:
                    this->force = 0.8;
                break;

            }
            if (keyEvent->key() == Qt::Key_1) {
                // Special tab handling
                return true;
            } else
                return false;
        }
        return false;
    }*/
};

#endif // MAINWINDOW_H
