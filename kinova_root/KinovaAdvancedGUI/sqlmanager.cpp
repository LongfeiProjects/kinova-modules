#include "sqlmanager.h"

SqlManager::SqlManager(){

}


int SqlManager::init(){
    bool databaseExist = false;
    int res = -1;
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
    return res;
}

void SqlManager::getRecordedTrajectories(){
    //TODO
}

bool SqlManager::saveRecordedTrajectory(Trajectory trajectory){
    QSqlQuery query(this->database);
    //FIXME change the sentence and use bindValue to se paramteres in the query
    QString strQuery = "INSERT INTO Trajectory (name,description) values('"+QString::fromStdString(trajectory.name)+"','"+QString::fromStdString(trajectory.description)+"')";
   if(!query.exec(strQuery)){
       qDebug() << "error inserting trajectory in database. Query was: " << strQuery << endl;
       return false;
   }

   return true;
}


/*Return the full trajectory including the cartesian positions*/
vector<Trajectory> SqlManager::getCompleteTrajectories(){

}

/*Just return the name and description of the trajectories*/
vector<Trajectory> SqlManager::getTrajectoriesInfo(){
    vector<Trajectory> result;
    QSqlQuery query(this->database);
    QString strQuery = "SELECT * from Trajectory";
    if(query.exec(strQuery)){
        while(query.next()){
            Trajectory t;
            t.id = query.value(0).toInt();
            t.name = query.value(1).toString().toStdString();
            t.description = query.value(2).toString().toStdString();
            result.push_back(t);
        }
    }else{
        qDebug() << "Error loading trajectories! " <<endl;
    }
    return result;
}

