#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <string.h>
#include <vector>
#include <qmainwindow.h>
#include <qmessagebox.h>
#include <time.h>
#include "dialogtaskfinished.h"
#include "gsrwidget.h"
#include "logger.h"
#include "types.h"

using namespace std;
namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_setButton_clicked();

    void on_startExperimentButton_clicked();

    void on_startTask_clicked();


    void on_gsrStartButton_clicked(bool checked);

    void on_setButton_2_clicked(bool checked);

private:
    Ui::MainWindow *ui;

    int participantId = -1;
    time_t initExperimentTime;
    time_t endExperimentTime;
    vector<Task> performedTasks;
    Task actualTask;
    GSRWidget* gsrwidget;

    bool experimentStarted = false;
    bool runningTask = false;





    void initGUI();
    bool showQuestion(string questionTitle, string msg);
    void showErrorMessage(QString msg);
    bool checkExperimentPreConditions();
    void saveExperimentalData();
    void showInfoMessage(QString msg);
};

#endif // MAINWINDOW_H
