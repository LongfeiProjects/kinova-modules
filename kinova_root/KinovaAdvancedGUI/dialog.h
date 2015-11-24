#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <iostream>
#include <vector>

/**this incluision is not nice at all. The code to save the trajectory should be in the mainwindow.cpp
buy qtcreator designer has some problems when I try to put the dialog.ui slots in the
mainwindow.cpp. So, It should be re-designed in a more elegant way.*/
#include "sqlmanager.h"

using namespace std;

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    void setSqlManager(SqlManager* sqlManager);

    void init(SqlManager* sqlManager);
    Trajectory execAndReturnSavedTrajectory(vector<RecordedCartesianInfo> sampledTrajectoryInfo);
private:
    Ui::Dialog *ui;
    SqlManager* sqlManager;

    Trajectory savedTrajectory;
    bool success;
    std::vector<Trajectory> recordedTrajectories; //initialized when dialog is created

private slots:

    void on_save_Trajectory_Panel_Button_accepted();

    void on_save_Trajectory_Panel_Button_rejected();
};

#endif // DIALOG_H
