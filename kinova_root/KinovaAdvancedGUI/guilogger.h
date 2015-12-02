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


    private:
        GUILogger(){};

        GUILogger(GUILogger const&) = delete;
        void operator=(GUILogger const&) = delete;

        vector<LogInfo> loggedEvents;
        bool enabled = false;


};

#endif // GUILOGGER_H
