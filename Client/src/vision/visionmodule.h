#ifndef __VISIONMODULE_H__
#define __VISIONMODULE_H__

#include <QObject>
#include <QUdpSocket>
#include "singleton.hpp"
#include "vision_detection.pb.h"
#include "staticparams.h"
#include "messageformat.h"
#include "zsplugin.hpp"
class CVisionModule : public QObject,public ZSPlugin  {
    Q_OBJECT
  public:
    CVisionModule(QObject *parent = 0);
    void udpSocketConnect(bool);
    void udpSocketDisconnect();
    void parse(void *, int);
    void setIfEdgeTest(bool);
    bool showIfEdgeTest();
    void run(){};
    quint16 getFPS();
    bool dealWithData();
    void setInterfaceIndex(const int);
  signals:
    void needDraw();
  public slots:
    void storeData();
    void oneStepSimData();
  private:
    void readSimData();
    void readRemoteSimData();
    CGeoPoint saoConvert(CGeoPoint);
    double saoConvert(double);
    void edgeTest();
    void udpSend();
    void checkCommand();
    QUdpSocket udpReceiveSocket, udpSendSocket;
    QHostAddress groupAddress;
    quint64 counter;
    int _interface;
    int vision_port;
    int saoAction;
    bool collectNewVision();
    bool IF_EDGE_TEST;
    Vision_DetectionFrame detectionFrame;
};
typedef Singleton <CVisionModule> VisionModule;
#endif // __VISIONMODULE_H__
