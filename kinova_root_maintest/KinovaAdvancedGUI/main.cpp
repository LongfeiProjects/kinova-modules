#include "robot.hpp"

#include <iostream>
#include <map>
#include "kinovalib.h"
#include <unistd.h>
#include <vector>
#include <kinovalib_openapi_global.h>
#include "devices/kinova_controller_openAPI.hpp"
#include "devices/kinova_status_openAPI.hpp"



using namespace std;



TrajectoryPoint setActualPosition(robot* bot, std::map<string,int> readTypeMap,vector<string> readType){
    vector<State> result;
    bot->ReadCurrentState(result,readType);
    int indice = readTypeMap["cart_pos"];
    TrajectoryPoint point;
    point.Position.CartesianPosition.X = result[indice][0];
    point.Position.CartesianPosition.Y = result[indice][1];
    point.Position.CartesianPosition.Z = result[indice][2];
    point.Position.CartesianPosition.ThetaX = result[indice][3];
    point.Position.CartesianPosition.ThetaY = result[indice][4];
    point.Position.CartesianPosition.ThetaZ = result[indice][5];
    point.Position.Fingers.Finger1 = result[indice][6];
    point.Position.Fingers.Finger2 = result[indice][7];
    point.Position.Fingers.Finger3 = result[indice][8];
    return point;
}



State convertOficialAPItoStatePoint_(TrajectoryPoint p){
    State s(9);
    s[0] =  p.Position.CartesianPosition.X;
    s[1] =  p.Position.CartesianPosition.Y;
    s[2] =  p.Position.CartesianPosition.Z;
    s[3] =  p.Position.CartesianPosition.ThetaX;
    s[4] =  p.Position.CartesianPosition.ThetaY;
    s[5] =  p.Position.CartesianPosition.ThetaZ;
    s[6] =  p.Position.Fingers.Finger1;
    s[7] =  p.Position.Fingers.Finger2;
    s[8] =  p.Position.Fingers.Finger3;
    return s;
}

TrajectoryPoint setHandPull(robot* bot,std::map<string,int> readTypeMap,vector<string> readType){
    TrajectoryPoint point = setActualPosition(bot,readTypeMap,readType);
    point.Position.Fingers.Finger1 = 23;
    point.Position.Fingers.Finger2 = 19;
    point.Position.Fingers.Finger3 = 29;
    point.Position.CartesianPosition.ThetaX = 1.5;
    point.Position.CartesianPosition.ThetaY = -0.1;
    point.Position.CartesianPosition.ThetaZ = -1.8;
    return point;
}

TrajectoryPoint setHandPoint(robot* bot,std::map<string,int> readTypeMap,vector<string> readType){
    TrajectoryPoint point = setActualPosition(bot,readTypeMap,readType);

    point.Position.CartesianPosition.ThetaX=1.5;
    point.Position.CartesianPosition.ThetaY=-0.1;
    point.Position.CartesianPosition.ThetaZ=0;
    point.Position.Fingers.Finger1 = 47;
    point.Position.Fingers.Finger2 = 43;
    point.Position.Fingers.Finger3 = 54;
    return point;
}

TrajectoryPoint setHandGrasp(robot* bot,std::map<string,int> readTypeMap,vector<string> readType){
    TrajectoryPoint point =  setActualPosition(bot,readTypeMap,readType);
    point.Position.CartesianPosition.ThetaX = 2.9;
    point.Position.CartesianPosition.ThetaY = 0;
    point.Position.CartesianPosition.ThetaZ = -1.5;

    point.Position.Fingers.Finger1 = 27;
    point.Position.Fingers.Finger2 = 25;
    point.Position.Fingers.Finger3 = 25;
    return point;
}



int main(int argc, char *argv[])
{

//----------------------------------------------------------------------------------

    cout << "Start main...." << endl;


    vector<string> readType;
    std::map<string,int> readTypeMap;
    robot* bot;

    // status reader
    Jaco* mdl = new Jaco();
    try{
        kinova_status_openapi * st= new kinova_status_openapi(mdl);

        readType.push_back("comp_t");
        readType.push_back("cart_pos");
        readType.push_back("j_vel");
        readType.push_back("j_pos");
        readType.push_back("hand_vel");

        readTypeMap.insert(make_pair("comp_t",0));
        readTypeMap.insert(make_pair("cart_pos",1));
        readTypeMap.insert(make_pair("j_vel",2));
        readTypeMap.insert(make_pair("j_pos",3));
        readTypeMap.insert(make_pair("hand_vel",4));
        cout << "time index = " << readTypeMap["comp_t"] <<endl;
        cout << "vel index = " << readTypeMap["j_vel"] <<endl;
        cout << "cartpos index = " << readTypeMap["cart_pos"] <<endl;

        // controller
        Option opt;
        opt.type_of_controller = "CartesianKinematic";
        opt.control_action = 28;
        const double Pid_coef[] = {5,0,0}; // deg
        std::vector<double> Pid(Pid_coef,End(Pid_coef));
        const char * _namefiles[] = {"cart_pos_openapi.mat","joint_vel_openapi.mat"};
        std::vector<std::string> namefile (_namefiles,End(_namefiles));
        Jaco* md = new Jaco();
        string namefileindex = "index_openapi.mat";
        kinova_controller_openapi * ct = new kinova_controller_openapi(namefile,namefileindex,opt,Pid,md,st->arm); // very rough patch because i can have only one API handle
        // checking module
        // define bounding box

        //const double bb_point[] = {-0.6,-0.8,-0.4};
        //const double bb_dims[]  = {1.2,1.6,0.8};
        const double bb_point[] = {-0.5,-0.75,-0.1};
        const double bb_dims[]  = {1,0.75,0.85};

        std::vector<double> bb_p(bb_point,End(bb_point)),bb_d(bb_dims,End(bb_dims));

        // define all the limit
        const char *cl[] = {"j_pos","j_tau"};
        std::vector<std::string> chekclist(cl,End(cl));
        const double joint_min[] = {-10000,47,19,-10000,-10000,-10000}; // deg
        const double joint_max[] = {10000,313,341,10000,10000,10000}; // deg
        const double tau_min[] = {-25}; // Nm
        const double tau_max[] = {25};  // nm
        std::vector<double> j_min(joint_min,End(joint_min)),j_max(joint_max,End(joint_max));
        std::vector<double> t_min(tau_min,End(tau_min)),t_max(tau_max,End(tau_max));

        std::vector<std::vector<double> > l_down_left_corner,l_dims,l_min,l_max;
        l_down_left_corner.push_back(bb_p);l_dims.push_back(bb_d);
        l_min.push_back(j_min);l_min.push_back(t_min);
        l_max.push_back(j_max);l_max.push_back(t_max);

        safetycheck checker(l_down_left_corner,l_dims,l_min,l_max,chekclist);
        checker.launch_thread=true;

        bot= new robot(st,ct,checker);

        bot->MoveHome();
        State moveFingers(9);
        moveFingers[0] = 0;
        moveFingers[1] = 0;
        moveFingers[2] = 0;
        moveFingers[3] = 0;
        moveFingers[4] = 0;
        moveFingers[5] = 0;
        moveFingers[6] = 0;
        moveFingers[7] = 1.0;
        moveFingers[8] = 1.0;
        moveFingers[9] = 1.0;
        for(int i=0;i<10;i++){
            bot->SendCommand(moveFingers,17);
        }
        int res = SUCCESS;
    }catch(KinDrv::KinDrvException e){
        cout << "error initializing"<< endl;
    }





    State right(9);
    State left(9);
    State up(9);
    State down(9);
    State push(9);
    State pull(9);

    right.zeros(9);
    left.zeros(9);
    up.zeros(9);
    down.zeros(9);
    push.zeros(9);
    pull.zeros(9);

    int cantMovements = 20;
    int cicles = 1;
    float wait_time = 5;
    float speed = 0.04;

    right[0] = -speed;
    down[2] = -speed;
    left[0] = speed;
    up[2] = speed;
    push[1] = -speed;
    pull[1] = speed;



    bot->StartLog(readType);


  //  State s = convertOficialAPItoStatePoint_(setHandPull(bot,readTypeMap,readType));
  //  bot->SendCommand(s,11);

    cout << "Start the loop!!" << endl;
    for(int n=0;n<=cicles;n++){


        for(int k=0;k<50;k++){
            for (int i = 0;i< cantMovements; i++){
                //save checkpoint
                bot->st->SaveCheckPoint(readType);
                //send command
                bot->SendCommand(left,7);
                usleep(wait_time*1000);
            }
            usleep(300*1000);
        }

        //GO HOME
        bot->MoveHome();
        usleep(1000*1000);

        //SET PULL POSITION
        TrajectoryPoint point = setHandPull(bot, readTypeMap,readType);
        State s = convertOficialAPItoStatePoint_(point);
        bot->SendCommand(s,11); //move arm and hand using cartesian position


        for(int k=0;k<50;k++){
            for (int i = 0;i< cantMovements; i++){
                //save checkpoint
                bot->st->SaveCheckPoint(readType);
                //send command
                bot->SendCommand(up,7);
                usleep(wait_time*1000);
            }
            usleep(300*1000);
        }


        //GO HOME and wait 1 sec
        bot->MoveHome();
        usleep(1000*1000);

        //SET GRAB POSITION
        point = setHandGrasp(bot, readTypeMap,readType);
        s = convertOficialAPItoStatePoint_(point);
        bot->SendCommand(s,11); //move arm and hand using cartesian position


        for(int k=0;k<50;k++){
            for (int i = 0;i< cantMovements; i++){
                //save checkpoint
                bot->st->SaveCheckPoint(readType);
                //send command
                bot->SendCommand(right,7);
                usleep(wait_time*1000);
            }
            usleep(300*1000);
        }


        for(int k=0;k<40;k++){
            for (int i = 0;i< cantMovements; i++){
                //save checkpoint
                bot->st->SaveCheckPoint(readType);
                //send command
                bot->SendCommand(down,7);
                usleep(wait_time*1000);
            }
            usleep(300*1000);
        }

        if(n==0){
            cout << "before stop recording" << endl;
            vector<Log> recordedLogs = bot->StopLog(readType);
            cout << "after stop recording" << endl;
        }
    }




    cout << "End main...." << endl;
    return 0;
}
