#ifndef __VISIONMODULE_H__
#define __VISIONMODULE_H__

#include <QObject>
#include <QUdpSocket>
#include <atomic>
#include "zos/utils/singleton.h"
#include "staticparams.h"
#include "messageformat.h"
#include "zos/core.h"
#include "zos/socketplugin.h"
#include "vision_detection.pb.h"
#include "zss_cmd.pb.h"
class VisionModule : public QObject,public Singleton<VisionModule>{
    Q_OBJECT
public:
    VisionModule();
    ~VisionModule()=default;
    // UDP
    void udpSocketConnect(bool);
    void udpSocketDisconnect();

    // Vision parse
    void parse(void *, int);
    bool collectNewVision();
    bool dealWithData();
    quint16 getFPS();
    zos::Publisher p_draw_signal;

    double calculateWeight(const int camID, const CGeoPoint&);

    std::atomic_bool _running = false;
public slots:
    void storeData();
    void oneStepSimData();
private:
    void readSimData();
    void udpSend();
    void checkCommand();
    QUdpSocket udpReceiveSocket;
    zos::udp::Plugin<Vision_DetectionFrame> udpSender;

    zos::Publisher p_sim_signal;
    zos::Subscriber<10> s_ssl_vision;

    QHostAddress groupAddress;
    quint64 counter;
    int vision_port;

    // vision parse
    bool _camera_control[PARAM::CAMERA];
    Vision_DetectionFrame _detectionFrame;
    std::shared_mutex _visionmutex;
    void resetVision();
    void updateEdge();
public:
    struct CheckRobotResult{
        bool res;
        int team;
        unsigned int id;
        double x,y,orientation;
    };
    CheckRobotResult checkRobot(double x,double y);
};
#endif // __VISIONMODULE_H__
