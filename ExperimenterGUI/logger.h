#ifndef LOGGER_H
#define LOGGER_H

#include <string.h>
#include <vector>
#include <mainwindow.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "types.h"

using namespace std;
class Logger
{
public:
    Logger();

    void dumpEvents(bool append, string gsrData, time_t intialGSRTimestamp, vector<Task> performedTasks, int participantId, time_t initTime, time_t endTime);

};

#endif // LOGGUER_H
