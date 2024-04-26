#ifndef __GLOBAL_DATA__
#define __GLOBAL_DATA__
#include "singleton.hpp"
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
    void setCameraMatrix(bool);
    bool cameraUpdate[PARAM::CAMERA];
    bool cameraControl[PARAM::CAMERA];
    bool processControl[3];
    int cameraID[PARAM::CAMERA];//show the mapping of cameras  id
    double robotPossible[2][PARAM::ROBOTMAXID];
    RobotInformation robotInformation[PARAM::TEAMS][PARAM::ROBOTMAXID];
    QMutex robotInfoMutex;
    DataQueue<RobotCommands> robotCommand[PARAM::TEAMS];
    int commandMissingFrame[PARAM::TEAMS];//team command VALID  --> commandMissingFrame<20
    CameraFix cameraFixMatrix[PARAM::CAMERA];
    SingleCamera cameraMatrix[PARAM::CAMERA];
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

    void CameraInit();

private:
    CGeoPoint saoConvert(CGeoPoint);
    void  saoConvertEdge();
    int saoAction;
};
typedef Singleton<CGlobalData> GlobalData;
#endif // __GLOBAL_DATA__
