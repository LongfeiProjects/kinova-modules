#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <string.h>
#include <vector>
#include <qmainwindow.h>
#include <qmessagebox.h>
#include <time.h>

using namespace std;
namespace Ui {
class MainWindow;
}

typedef struct{
    time_t initTimestamp;
    time_t endTimestamp;
    string taskName;
    bool finished;
}Task;

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

private:
    Ui::MainWindow *ui;

    int participantId = -1;
    time_t initExperimentTime;
    time_t endExperimentTime;
    vector<Task> performedTasks;
    Task actualTask;

    bool experimentStarted = false;
    bool runningTask = false;





    void initGUI();
    void showErrorMessage(QString msg);
    bool checkExperimentPreConditions();
    void saveExperimentalData();
};

#endif // MAINWINDOW_H
