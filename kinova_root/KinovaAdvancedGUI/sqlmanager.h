#ifndef SQLMANAGER_H
#define SQLMANAGER_H

#include <QtSql>
#include <QtDebug>
#include <QFileInfo>
#include <unistd.h>
#include <iostream>
#include <vector>
#include "types.h"

using namespace std;




class SqlManager
{
public:
    static SqlManager& getInstance(){
        static SqlManager instance;
        return instance;
    }

    void getRecordedTrajectories();


    /*Return the full trajectory including the cartesian positions*/
    vector<Trajectory> getCompleteTrajectories();
    Trajectory getCompleteTrajectorie(int trajectoryId);
    /*Just return the name and description of the trajectories*/
    vector<Trajectory> getTrajectoriesInfo();
    /*Save the complete trajectory in the database and also updates the id of the trajectory parameter*/
    bool saveRecordedTrajectory(Trajectory &trajectory);
    vector<Trajectory> getCompleteTrajectoriesByParticipant(int participantID);

    bool cleanDB();
private:

    SqlManager(){
        bool databaseExist = false;
        this->database = QSqlDatabase::addDatabase("QSQLITE");
        QFile dbFile("/home/smaricha/qt_workspace/kinova-modules/kinova_root/KinovaAdvancedGUI/database/trajectoryProg.db");
        if(!dbFile.exists()){
            //TODO create database ??
        }else{
            databaseExist=true;
            this->database.setDatabaseName(dbFile.fileName());
        }

        if(databaseExist){
            if(this->database.open()){
                cout << "database opened" << endl;
            }
        }
    }

    SqlManager(SqlManager const&) = delete;
    void operator=(SqlManager const&) = delete;

    QSqlDatabase database;


};

#endif // SQLMANAGER_H
