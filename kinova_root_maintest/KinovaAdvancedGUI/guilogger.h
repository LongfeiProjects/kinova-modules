#ifndef GUILOGGER_H
#define GUILOGGER_H


#include <QtGui>
#include <vector>
#include "types.h"
#include <time.h>
#include <iostream>
#include <sstream>
#include <fstream>

class GUILogger{

    public:
        static GUILogger& getInstance(){
            static GUILogger instance;

            return instance;
        }

        void addMouseEvent(QWidget* source, QMouseEvent* event, string element);
        void addComboChanged(string element, string value);
        void addDialogEvent(string element,string value);
        void dumpEvents(string filename, bool append);
        void clearLogs();
        void startLogging();
        void enableLogging(bool enabled);


        void dumpTrajectories(string filename, vector<Trajectory> trajectories, bool append);
private:
        GUILogger(){
            time_t t;
            time(&t);
            ostringstream filename;
            filename << "GUIEventsTemp_" <<  t << ".log";
            this->realTimeOutfile.open(filename.str(), std::ios_base::app); //In this mode we always append data
        };

        GUILogger(GUILogger const&) = delete;
        void operator=(GUILogger const&) = delete;

        vector<LogInfo> loggedEvents;
        bool enabled = false;
        std::ofstream realTimeOutfile;


};

#endif // GUILOGGER_H
