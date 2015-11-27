#include "guilogger.h"

using namespace std;

void GUILogger::addMouseEvent(QMouseEvent* event, string element){
    LogInfo info;
    string button;

    //Left of Right?
    if(event->button() == Qt::RightButton){
        button = "Right";
    }else if(event->button() == Qt::LeftButton){
        button = "Left";
    }else{
        button = "NotIdentifiedButton";
    }

    //Press, Release or DblClick?
    switch (event->type()) {
        case QMouseEvent::MouseButtonPress:
            info.event = button + "ButtonPress";
        break;
        case QMouseEvent::MouseButtonRelease:
            info.event = button + "ButtonRelease";
        break;
        case QMouseEvent::MouseButtonDblClick:
            info.event = button + "ButtonDoubleClick";
        break;
        default:
            ostringstream oss;
            oss << button << " button event not identified - event->type() = " <<  (int)event->type();
            info.event=oss.str();
        break;
    }

    time(&info.timestamp);

    info.element=element;
    this->loggedEvents.push_back(info);
}



void GUILogger::addComboChanged(string element,string value){
    LogInfo info;
    info.element=element;
    time(&info.timestamp);
    info.event = "valueChangeTo_"+value;
    this->loggedEvents.push_back(info);
}

void GUILogger::dumpEvents(string filename, bool append){
    std::ofstream outfile;
    if(append)
        outfile.open(filename, std::ios_base::app);
    else
        outfile.open(filename, std::ios_base::out);

    for(unsigned int i=0;i<this->loggedEvents.size();i++){
        LogInfo info = this->loggedEvents[i];
        //TODO dump in txt
        cout << info.timestamp << " - " << info.event << " - " << info.element << endl;
        outfile << info.timestamp << " - " << info.event << " - " << info.element << endl;
    }

    outfile.close();
}

void GUILogger::clearLogs(){
    this->loggedEvents.clear();
}

