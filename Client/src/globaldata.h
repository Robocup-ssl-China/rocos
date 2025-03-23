#ifndef __GLOBAL_DATA__
#define __GLOBAL_DATA__
#include "zos/utils/singleton.h"
#include "dataqueue.hpp"
#include "messageformat.h"
#include "ballrecords.h"
#include "zss_cmd.pb.h"
#include <QMutex>
struct RobotInformation {
    bool infrared;
    bool flat;
    bool chip;
    double battery;
    double capacitance;
    RobotInformation(): infrared(false), flat(false), chip(false), battery(0), capacitance(0) {}
};
struct RobotSpeed {
    float vx;
    float vy;
    float vr;
    RobotSpeed(): vx(0), vy(0), vr(0) {}
    RobotSpeed(float _x, float _y, float _r):
        vx(_x), vy(_y), vr(_r) {}
};
struct RobotCommands {
    RobotSpeed robotSpeed[PARAM::ROBOTMAXID];
    bool valid;
    RobotCommands(): valid(false) {
        for (int i = 0; i < PARAM::ROBOTMAXID; i++)
            robotSpeed[i] = RobotSpeed();
    }
};

class CGlobalData {
public:
    CGlobalData();
    bool cameraUpdate[PARAM::CAMERA];
    bool cameraControl[PARAM::CAMERA];
    int cameraID[PARAM::CAMERA];//show the mapping of cameras  id
    double robotPossible[2][PARAM::ROBOTMAXID];
    RobotInformation robotInformation[PARAM::TEAMS][PARAM::ROBOTMAXID];
    QMutex robotInfoMutex;
    DataQueue<RobotCommands> robotCommand[PARAM::TEAMS];
    int commandMissingFrame[PARAM::TEAMS];//team command VALID  --> commandMissingFrame<20
    Msg::CameraEdge cameraAnchor[PARAM::CAMERA];
    DataQueue<ReceiveVisionMessage> camera[PARAM::CAMERA];
    DataQueue<ReceiveVisionMessage> processBall;
    DataQueue<ReceiveVisionMessage> processRobot;
    DataQueue<ReceiveVisionMessage> maintain;
    DataQueue<BallRecord> ballrecords;
    int lastTouch;//Be attention it's id!!!
    QByteArray debugBlueMessages;
    QByteArray debugYellowMessages;
    QMutex debugMutex;// debugMessages;
    bool ctrlC;
    QMutex ctrlCMutex;
    double calculateWeight(const int camID, const CGeoPoint&);
};
typedef Singleton<CGlobalData> GlobalData;
#endif // __GLOBAL_DATA__
