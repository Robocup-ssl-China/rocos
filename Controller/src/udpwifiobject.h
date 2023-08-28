#ifndef UDPWIFIOBJECT_H
#define UDPWIFIOBJECT_H
#include <QStringList>
#include <QObject>
#include <QString>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <QDateTime>
#include "zss_cmd.pb.h"

class udpwifiobject : public QObject
{
    Q_OBJECT
public:
    udpwifiobject(QObject *parent = 0);
    Q_INVOKABLE void updateInterfaces();
    Q_INVOKABLE QStringList getInterfaces() { return interfaces;}
    Q_INVOKABLE void resetWifiCommInterface(int _interface_index);
    QNetworkInterface getFromIndex(const int index){ return QNetworkInterface::interfaceFromName(interfaces[index]); }
    Q_INVOKABLE void updateCommandParams(int robotID,int velX,int velY,int velR,bool dribble,int dribbleLevel,bool mode,bool shoot,int power){
        this->robotID = robotID;
        this->velX = velX; this->velY = velY; this->velR = velR;
        this->dribble = dribble;
        this->shootMode = mode;this->shoot = shoot; this->shootPowerLevel = power;
        this->dribbleLevel = dribbleLevel;
    }
    Q_INVOKABLE QStringList getipList() { return currentinterfaces;}
    Q_INVOKABLE void changeActionWifiCommInterface(int index) { resetWifiCommInterface(index);}
    Q_INVOKABLE void sendControlIP(int index) { _controlId = carid[index]; _controlIP = currentinterfaces[index];}
    Q_INVOKABLE int getControlId(){ return _controlId; }
    //Q_INVOKABLE void initInterface(){ currentinterfaces.clear(); }
    Q_INVOKABLE void sendCommand();
public slots:
  void n_mcRecvData();
  void n_recvData();
private:
  bool _n_binded;
  int _n_last_interface = -1;
  QString _n_address[100];
  QString _n_mcAddr;
  QString _controlIP;
  QStringList currentinterfaces;
  int num;
  int _controlId;
  int carid[32];
  int _n_mcPort;
  int _n_recvPort;
  int _n_sendPort;
  QUdpSocket _n_recvMcSocket;
  QUdpSocket _n_recvSocket;
  QUdpSocket _n_sendSocket;
private:
    QStringList interfaces;
private:
    QUdpSocket sendSocket,receiveSocket;
    bool shoot;
    bool dribble;
    bool shootMode;//false is "flat shoot" and true is "lift shoot".
    quint8 robotID;
    //packageTypes: 0x00, 0x40, 0x80, 0xc0
    quint8 packageType = 0x00;
    //gameStatus: 0x00, 0x10, 0x20, 0x30
    quint8 gameStatus = 0x00;
    qint16 velX;
    qint16 velY;
    qint16 velR;
    quint16 dribbleLevel;
    quint16 shootPowerLevel;
};

#endif // UDPWIFIOBJECT_H
