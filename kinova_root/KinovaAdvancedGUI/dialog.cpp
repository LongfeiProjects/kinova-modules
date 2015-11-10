#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}


void Dialog::init(SqlManager* sqlManager){
    this->setSqlManager(sqlManager);
    this->loadTrajectoryList();
    this->success = false;
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_save_Trajectory_Panel_Button_accepted()
{
    cout << "saving trajectory" << endl;
    //TODO Here we have to include the collection of CartesianInfo
    this->savedTrajectory.name = this->ui->nameEdit->text().toStdString();
    this->savedTrajectory.description = this->ui->descriptionEdit->toPlainText().toStdString();
    this->success = this->sqlManager->saveRecordedTrajectory(this->savedTrajectory);
}


void Dialog::setSqlManager(SqlManager* sqlManager){
    this->sqlManager = sqlManager;
}

Trajectory Dialog::execAndReturnSavedTrajectory(vector<RecordedCartesianInfo> sampledTrajectoryInfo){
    this->savedTrajectory.trajectoryInfo = sampledTrajectoryInfo;
    this->exec();
    if(this->success){
         return this->savedTrajectory;
    }else{
        Trajectory t;
        t.id=-1;
        return t;
    }
}


/*
not used
*/
void Dialog::loadTrajectoryList(){
    recordedTrajectories = this->sqlManager->getTrajectoriesInfo();
    for(vector<Trajectory>::iterator iter = recordedTrajectories.begin(); iter!=recordedTrajectories.end();++iter){
       Trajectory t = *iter;
       //this->ui->listWidget->addItem(QString::fromStdString(t.name));
    }
}
