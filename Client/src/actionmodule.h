#ifndef ZACTIONMODULE_H
#define ZACTIONMODULE_H
#include <QObject>
#include <QMutex>
#include <QUdpSocket>
#include <QSerialPort>
#include "singleton.hpp"
#include "zss_cmd.pb.h"
#include "zss_cmd_new.pb.h"
#include "staticparams.h"
namespace ZSS {
using __callback_type = std::function<void(const void*,const size_t)>;
enum struct ProtocolType{
    UDP_24L01,
    Serial_24L01,
    UDP_WIFI,
};
enum CommType{
    DEFAULT = 0,
    SERIAL_24L01 = 1,
    UDP_24L01 = 2,
    UDP_WIFI = 3,
};
class Action_Serial24L01 : public QObject {
    Q_OBJECT
public:
    struct Config{
        QString portName;
    };
    Action_Serial24L01(const Config& config,const __callback_type& f={},QObject *parent=nullptr);
    ~Action_Serial24L01();
    bool start();
    bool stop();
    void reConfig(const Config& config);
    bool sendConfigPacket(const char*,const size_t);
    bool send(const char*,const size_t);
private slots:
    void receiveData();
private:
    Config _config;
    QSerialPort serial;
    const __callback_type _cb;
};
class Action_UDP24L01 : public QObject {
    Q_OBJECT
public:
    struct Config{
        QString send_ip;
        int send_port;
        QString recv_ip;
        int recv_port;
    };
    Action_UDP24L01(const Config& config,const __callback_type& f={},QObject *parent=nullptr);
    ~Action_UDP24L01();
    bool start();
    bool stop();
    void reConfig(const Config& config);
    bool sendConfigPacket(const char*,const size_t);
    bool send(const char*,const size_t);
private slots:
    void receiveData();
private:
    Config _config;
    QUdpSocket _sendSocket;
    QUdpSocket _recvSocket;
    const __callback_type _cb;
};
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
    void changeAddress(int team,int index);
    void setcar_varity(int index,int varity) { car_varity[index] = varity; }
    int getcar_varity(int index) { return car_varity[index]; }
    qint64 last_receive_time[PARAM::TEAMS][PARAM::ROBOTNUM];
    QStringList getAllAddress();
    QString getRealAddress(int team);
    QStringList getAllCarChoose();
    QString getNeedAllCarVariety(int index);
  private slots:
    void readData();
  private:
    void sendStartPacket(int, int);
  private:
    int car_varity[PARAM::ROBOTNUM];
    QByteArray tx;
    QByteArray rx;
    QUdpSocket sendSocket;
    QUdpSocket receiveSocket;
    ProtocolType _protocolType[PARAM::ROBOTNUM];
  public:
    void resetWifiCommInterface(int);
  public slots:
    void n_mcRecvData();
    void n_recvData();
  private:
    bool _n_binded;
    int _n_last_interface = -1;
    QString _n_address[PARAM::TEAMS][PARAM::ROBOTNUM];
    QString _n_mcAddr;
    int _n_mcPort;
    int _n_recvPort;
    int _n_sendPort;
    QUdpSocket _n_recvMcSocket;
    QUdpSocket _n_recvSocket;
    QUdpSocket _n_sendSocket;
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
     bool init(int team);
     void setMedusaSettings(bool,bool);
     void sendLegacy(int t,const ZSS::Protocol::Robots_Command&);
     QStringList& updatePortsList();
     int getFrequency(int team){ return team ? yellow_frequency:blue_frequency;}
     bool changePorts(int,int);
     bool changeFrequency(int,int);
     bool openSerialPort(int team);
     bool closeSerialPort(int team);
     int get_team(){return _color;}
 signals:
     void receiveRobotInfo(int,int);
 private slots:
     void readData();
 private:
     void sendStartPacket(int team);
 private:
     QSerialPort b_serial;
     QSerialPort y_serial;
     QStringList ports;
     int blue_frequency;
     int yellow_frequency;
     QByteArray tx;
     QByteArray rx;
     int _team;
     int _color;
     int _side;
     ProtocolType _protocolType[PARAM::ROBOTNUM];
 };
 typedef Singleton<ActionModuleSerialVersion> NActionModule;
 }
 #endif // ZACTIONMODULE_H
