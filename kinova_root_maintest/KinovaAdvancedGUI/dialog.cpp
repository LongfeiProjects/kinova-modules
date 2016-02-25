#include "dialog.h"
#include <QProgressDialog>
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    cout << "constructing the save panel" << endl;
    ui->setupUi(this);
    this->success = false;
}


Dialog::~Dialog()
{
  //  delete ui;
}

void Dialog::on_save_Trajectory_Panel_Button_accepted()
{
    QProgressDialog dlg;
    dlg.setParent(NULL);
     //dlg.setParent(this);
    dlg.setWindowModality(Qt::WindowModal);
    dlg.setRange(0,0);
    dlg.setCancelButton(0);
    dlg.show();

    cout << "saving trajectory" << endl;
    this->savedTrajectory.name = this->ui->nameEdit->text().toStdString();
    this->savedTrajectory.description = this->ui->descriptionEdit->toPlainText().toStdString();
    //this->success = SqlManager::getInstance().saveRecordedTrajectory(this->savedTrajectory);
    this->success = SqlManager::getInstance().saveRecordedTrajectoryBulkMode(this->savedTrajectory);
    dlg.cancel();
}



Trajectory Dialog::execAndReturnSavedTrajectory(vector<RecordedCartesianInfo> sampledTrajectoryInfo, int participantID, time_t initTimestampTrajectory){
    cout << "Entering save panel" << endl;
    this->savedTrajectory.trajectoryInfo = sampledTrajectoryInfo;
    cout << "before execute save panel" << endl;
    this->savedTrajectory.participantID=participantID;
    this->savedTrajectory.initialTimestamp = initTimestampTrajectory;
    bool result = QDialog::exec();//this->exec();
    string res = result?"Yes":"No";
    GUILogger::getInstance().addDialogEvent("saveTrajectoryDialog",res);
    cout << "after execute save panel" << endl;

    if(result && this->success){
         return this->savedTrajectory;
    }else{
        Trajectory t;
        t.id=-1;
        return t;
    }
}

int Dialog::execFake(){
    cout << "before exec" << endl;
    return this->exec();
    cout << "after exec" << endl;
}

void Dialog::on_save_Trajectory_Panel_Button_rejected()
{
    this->success=false;
}
