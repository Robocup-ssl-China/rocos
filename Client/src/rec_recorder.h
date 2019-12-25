#ifndef REC_RECORDER_H
#define REC_RECORDER_H
#include "zss_rec_old.pb.h"
#include "globaldata.h"
#include "singleton.hpp"
//struct RecMsg {
//    //ctrlc
//    bool ctrlc;
//    //selectedArea
//    float maxx;
//    float maxy;
//    float minx;
//    float miny;
//    //maintainVision
//    const OriginMessage processRobot;
//    int lastTouch;
//    const OriginMessage maintain;
//    //debugMsgs
//    QByteArray debugBlueMessage;
//    QByteArray debugYellowMessage;
//};

class RecRecorder
{
public:
    RecRecorder();
    void init();
    void stop();
    void store();

private:

};

typedef Singleton<RecRecorder> ZRecRecorder;

#endif // REC_RECORDER_H
