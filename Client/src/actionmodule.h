#ifndef ZACTIONMODULE_H
#define ZACTIONMODULE_H
#include <QObject>
#include <QMutex>
#include <QUdpSocket>
#include <QSerialPort>
#include "singleton.hpp"
#include "zss_cmd.pb.h"
#include "staticparams.h"
namespace ZSS {
class ActionModule : public QObject {
    Q_OBJECT
  public:
    ActionModule(QObject *parent = 0);
    ~ActionModule();
    void sendLegacy(int t, const ZSS::Protocol::Robots_Command&);
    bool connectRadio(int, int);
    bool disconnectRadio(int);
    void setSimulation(bool);
    int team[PARAM::TEAMS];
  private slots:
    void readData();
  private:
    void sendStartPacket(int, int);
  private:
    QByteArray tx;
    QByteArray rx;
    QUdpSocket sendSocket;
    QUdpSocket receiveSocket;
  signals:
    void receiveRobotInfo(int, int);
};
typedef Singleton<ActionModule> ZActionModule;

class ActionModuleSerialVersion : public QObject
{
    Q_OBJECT
public:
    ActionModuleSerialVersion(QObject *parent = 0);
    ~ActionModuleSerialVersion();
    bool init();
    void setMedusaSettings(bool,bool);
    void sendLegacy(const ZSS::Protocol::Robots_Command&);
    QStringList& updatePortsList();
    int getFrequency(){ return frequency;}
    bool changePorts(int);
    bool changeFrequency(int);
    bool openSerialPort();
    bool closeSerialPort();
signals:
    void receiveRobotInfo(int,int);
private slots:
    void readData();
private:
    void sendStartPacket();
private:
    QSerialPort serial;
    QStringList ports;
    int frequency;
    QByteArray tx;
    QByteArray rx;
    int _color;
    int _side;
};
typedef Singleton<ActionModuleSerialVersion> NActionModule;
}
#endif // ZACTIONMODULE_H
