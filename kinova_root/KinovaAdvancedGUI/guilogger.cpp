#include "guilogger.h"
#include <qwidget.h>
using namespace std;

class QWidget;

void GUILogger::addMouseEvent(QWidget *source, QMouseEvent* event, string element){
    if(this->enabled){
        LogInfo info;
        string button;

        QPointF qpointf = event->localPos();
        cout << "qpointf.x(),qpointf.y() =  " << qpointf.x() << "," << qpointf.y() <<endl;
        cout << "qpointf.rx(),qpointf.ry() =  " << qpointf.rx() << "," << qpointf.ry() <<endl;
        int w = source->width();
        int h = source->height();



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
                if( qpointf.x()<0 || qpointf.y()<0 ||  qpointf.x() > w || qpointf.y() > h ){
                   info.event = button + "ButtonRelease_NoClicked";
                   cout << "WAS NOT A CLICK!" <<endl;
                }else{
                    info.event = button + "ButtonRelease_Clicked";
                }
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
}



void GUILogger::addComboChanged(string element,string value){
    if(this->enabled){
        LogInfo info;
        info.element=element;
        time(&info.timestamp);
        info.event = "valueChangeTo_"+value;
        this->loggedEvents.push_back(info);
    }
}


void GUILogger::addDialogEvent(string element,string value){
    if(this->enabled){
        LogInfo info;
        info.element=element;
        time(&info.timestamp);
        info.event = "DialogReturnValue_"+value;
        this->loggedEvents.push_back(info);
    }
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

void GUILogger::enableLogging(bool enabled){
    this->enabled = enabled;
}
