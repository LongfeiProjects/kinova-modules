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

        void addMouseEvent(QMouseEvent* event, string element);
        void addComboChanged(string element, string value);
        void dumpEvents(string filename, bool append);
        void clearLogs();
        void startLogging();


    private:
        GUILogger(){};

        GUILogger(GUILogger const&) = delete;
        void operator=(GUILogger const&) = delete;

        vector<LogInfo> loggedEvents;


};

#endif // GUILOGGER_H
