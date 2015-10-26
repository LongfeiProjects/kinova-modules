#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "kinovalib.h"
#include <unistd.h>
#include <QTimer>
#include <QSignalMapper>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
  //  QDialogButtonBox* save_Trajectory_Panel_Button;

    Kinovalib* klib ;

    /*Kinova API stuff*/
    TrajectoryPoint point;
    int opType;
    bool armCommand;
    bool fingerCommand;
    CartesianPosition actualPosition;
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

    int fixedStepsCounter[8] = {0,0,0,0,0,0,0,0};
    bool stopedTimers[8] = {false,false,false,false,false,false,false,false};

    int LOOP_SPEED_SEND_VELOCITY_COMMAND = 5; // time in miliseconds.

    bool kinova_initialized;
    void error_kinova_not_initialized();

    HandPosition* presetPositions;


    float    PRECISION_SPEED = 0.04;
    float    LOW_SPEED = 0.07;
    float    MEDIUM_SPEED = 1.1;
    float    HIGH_SPEED = 1.5;


    bool isRecordedTrajecory;

    void showSaveTrajectoryPanel();

private slots:
     void clickedSlot();
    void on_homeButton_clicked();
    void on_typeComboBox_currentIndexChanged(int index);

    void on_xDoubleSpinBox_valueChanged(double arg1);
    void on_yDoubleSpinBox_valueChanged(double arg1);
    void on_zDoubleSpinBox_valueChanged(double arg1);
    void on_xDoubleSpinBox_2_valueChanged(double arg1);
    void on_yDoubleSpinBox_2_valueChanged(double arg1);
    void on_zDoubleSpinBox_2_valueChanged(double arg1);
    void on_speed1DoubleSpinBox_valueChanged(double arg1);
    void on_speed2DoubleSpinBox_valueChanged(double arg1);
    void on_limitationsGroup_toggled(bool arg1);
    void on_ArmControl_Widget_toggled(bool arg1);
    void on_FingersControl_Widget_toggled(bool arg1);
    void on_pushButton_clicked();
    void on_finger1DoubleSpinBox_valueChanged(double arg1);
    void on_finger2DoubleSpinBox_valueChanged(double arg1);
    void on_finger3DoubleSpinBox_valueChanged(double arg1);
    void on_fixedSteps_radioButton_clicked(bool checked);
    void on_continuousRadioButton_clicked(bool checked);
    void loopSendVelocityCommad(int direction);
    void on_upButton_pressed();
    void on_upButton_released();
    void on_leftButton_pressed();
    void on_leftButton_released();
    void on_downButton_pressed();
    void on_downButton_released();
    void on_rightButton_pressed();
    void on_rightButton_released();
    void on_speed_horizontalSlider_sliderMoved(int position);
    void on_pushButton_2_clicked();
    void on_finger1_slider_sliderMoved(int position);
    void on_finger2_slider_sliderMoved(int position);
    void on_finger3_slider_sliderMoved(int position);
    void on_presetHandPositions_currentIndexChanged(int index);
    void on_pushButton_Y_pressed();
    void on_pushButton_Y_released();
    void on_pullButton_Y_pressed();
    void on_pullButton_Y_released();
    void on_speedComboBox_currentIndexChanged(const QString &arg1);
    void on_openHandButton_pressed();
    void on_openHandButton_released();
    void on_closeHandButton_pressed();
    void on_closeHandButton_released();
    void save_recorded_trajectory();
    void save_arm_position();
    void save_fingers_position();
    void save_full_position();
    void on_record_Button_toggled(bool checked);

};

#endif // MAINWINDOW_H
