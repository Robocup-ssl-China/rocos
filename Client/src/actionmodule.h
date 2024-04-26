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
using __callback_type = std::function<void(const void*,const size_t)>;
enum struct ProtocolType{
    UDP_24L01,
    Serial_24L01,
};
// class Action_Serial24L01 : public QObject {
//     Q_OBJECT
// public:
//     struct Config{
//         QString portName;
//     };
//     Action_Serial24L01(const Config& config,const __callback_type& f={},QObject *parent=nullptr);
//     ~Action_Serial24L01();
//     bool start();
//     bool stop();
//     void reConfig(const Config& config);
//     bool sendConfigPacket(const char*,const size_t);
//     bool send(const char*,const size_t);
// private slots:
//     void receiveData();
// private:
//     Config _config;
//     QSerialPort serial;
//     const __callback_type _cb;
// };
// class Action_UDP24L01 : public QObject {
//     Q_OBJECT
// public:
//     struct Config{
//         QString send_ip;
//         int send_port;
//         QString recv_ip;
//         int recv_port;
//     };
//     Action_UDP24L01(const Config& config,const __callback_type& f={},QObject *parent=nullptr);
//     ~Action_UDP24L01();
//     bool start();
//     bool stop();
//     void reConfig(const Config& config);
//     bool sendConfigPacket(const char*,const size_t);
//     bool send(const char*,const size_t);
// private slots:
//     void receiveData();
// private:
//     Config _config;
//     QUdpSocket _sendSocket;
//     QUdpSocket _recvSocket;
//     const __callback_type _cb;
// };
// class ActionModule : public QObject {
//     Q_OBJECT
//   public:
//     ActionModule(QObject *parent = 0);
//     ~ActionModule();
//     void sendLegacy(int t, const ZSS::Protocol::Robots_Command&);
//     bool connectRadio(int, int);
//     bool disconnectRadio(int);
//     void setSimulation(bool);
//     int team[PARAM::TEAMS];
//     void changeAddress(int team,int index);
//     QStringList getAllAddress();
//     QString getRealAddress(int team);
//   private slots:
//     void readData();
//   private:
//     void sendStartPacket(int, int);
//   private:
//     QByteArray tx;
//     QByteArray rx;
//     QUdpSocket sendSocket;
//     QUdpSocket receiveSocket;
//     ProtocolType _protocolType[PARAM::ROBOTNUM];
//   signals:
//     void receiveRobotInfo(int, int);
// };
// typedef Singleton<ActionModule> ZActionModule;

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
    ProtocolType _protocolType[PARAM::ROBOTNUM];
};
typedef Singleton<ActionModuleSerialVersion> NActionModule;
}
#endif // ZACTIONMODULE_H
