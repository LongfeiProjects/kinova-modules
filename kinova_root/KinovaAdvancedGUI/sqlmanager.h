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
    SqlManager();

    void getRecordedTrajectories();
    int init();

    /*Return the full trajectory including the cartesian positions*/
    vector<Trajectory> getCompleteTrajectories();

    /*Just return the name and description of the trajectories*/
    vector<Trajectory> getTrajectoriesInfo();

    bool saveRecordedTrajectory(Trajectory trajectory);
private:
    QSqlDatabase database;
public slots:
};

#endif // SQLMANAGER_H
