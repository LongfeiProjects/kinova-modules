#include "guilogger.h"
#include <qwidget.h>
using namespace std;

class QWidget;

void GUILogger::addMouseEvent(QWidget *source, QMouseEvent* event, string element){
    if(this->enabled){
        LogInfo info;
        string button;

        QPointF qpointf = event->localPos();
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

        this->realTimeOutfile << info.timestamp << " - " << info.event << " - " << info.element << endl;
    }
}



void GUILogger::addComboChanged(string element,string value){
    if(this->enabled){
        LogInfo info;
        info.element=element;
        time(&info.timestamp);
        info.event = "valueChangeTo_"+value;
        this->loggedEvents.push_back(info);

        this->realTimeOutfile << info.timestamp << " - " << info.event << " - " << info.element << endl;
    }
}


void GUILogger::addDialogEvent(string element,string value){
    if(this->enabled){
        LogInfo info;
        info.element=element;
        time(&info.timestamp);
        info.event = "DialogReturnValue_"+value;
        this->loggedEvents.push_back(info);

        this->realTimeOutfile << info.timestamp << " - " << info.event << " - " << info.element << endl;

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


void GUILogger::dumpTrajectories(string filename,vector<Trajectory> trajectories, bool append)
{
    std::ofstream outfile;
    if(append)
        outfile.open(filename, std::ios_base::app);
    else
        outfile.open(filename, std::ios_base::out);

    for(int i = 0; i < trajectories.size();i++){
        Trajectory t = trajectories[i];
        outfile << "---Trajectory---" << endl;
        outfile << "name:"<<t.name<<endl;
        outfile << "id(internal for database):"<<t.id<<endl;
        outfile << "description:"<<t.description<<endl;
        outfile << "participantId:"<<t.participantID<<endl;
        outfile << "InitialTimestamp:"<<t.initialTimestamp<<endl;
        outfile << "Trajectory Description: (timestamp,x,y,z,thetaX,thetaY,thetaZ,finger1,finger2,finger3,angvel_1,angvel_2,angvel_3,angvel_4,angvel_5,angvel_6)" << endl;
        for(int j=0;j<t.trajectoryInfo.size();j++){
            RecordedCartesianInfo info= t.trajectoryInfo[j];
            outfile << info.timestamp << "," << info.pos_x << "," << info.pos_y << "," << info.pos_z << "," << info.theta_x << "," << info.theta_y << "," << info.theta_z;
            outfile << "," << info.finger1 << "," << info.finger2 << "," << info.finger3 << "," << info.angvel_j1 << "," << info.angvel_j2 << "," << info.angvel_j3 << ",";
            outfile << info.angvel_j4 << "," << info.angvel_j5 << "," << info.angvel_j6 << endl;
        }
        outfile << "---EndTrajectory---" << endl;
    }
}

void GUILogger::clearLogs(){
    this->loggedEvents.clear();
}

void GUILogger::enableLogging(bool enabled){
    this->enabled = enabled;
}
