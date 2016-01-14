#include "logger.h"

Logger::Logger()
{

}

void logGSRData(bool append, int participantId,string gsrData,time_t initialGSRTimestamp){
    ostringstream filename;
    filename << "GSRLog_Participant_" <<  participantId << ".log";
    std::ofstream outfile;
    if(append)
        outfile.open(filename.str().data(), std::ios_base::app);
    else
        outfile.open(filename.str().data(), std::ios_base::out);
    outfile << initialGSRTimestamp << endl;
    outfile << gsrData << endl;
    outfile.close();
}


void logExperimentData(vector<Task> performedTasks, int participantId,bool append, time_t initTime, time_t endTime){
    ostringstream filename;
    filename << "ExperimentalTaksLog_Participant_" <<  participantId << ".log";
    std::ofstream outfile;
    if(append)
        outfile.open(filename.str().data(), std::ios_base::app);
    else
        outfile.open(filename.str().data(), std::ios_base::out);

    outfile << "Experiment init time: "<< initTime << endl;
    outfile << "Experiment end time: "<< endTime << endl;
    for(unsigned int i=0;i<performedTasks.size();i++){
        Task t = performedTasks[i];
        outfile << "---Task--->" << endl;
        outfile << "Name:" << t.taskName << endl;
        outfile << "Init:" << t.initTimestamp<< endl;
        outfile << "End:" << t.endTimestamp<< endl;
        outfile << "GiveUp?:" << t.giveUp<< endl;
        outfile << "Comments:" << t.comments<< endl;
    }
    outfile.close();

}

void Logger::dumpEvents(bool append, string gsrData, time_t intialGSRTimestamp, vector<Task> performedTasks, int participantId, time_t initTime, time_t endTime){
    logGSRData(append, participantId,gsrData,intialGSRTimestamp);
    logExperimentData(performedTasks,participantId,append,initTime,endTime);
}


