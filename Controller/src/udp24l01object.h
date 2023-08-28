#ifndef UDP24L01OBJECT_H
#define UDP24L01OBJECT_H
#include <QObject>
#include <QString>
#include <QtDebug>
#include <QUdpSocket>
#include "radiopacket.h"

class udp24l01object : public QObject
{
    Q_OBJECT
public:
    udp24l01object(QObject *parent = 0);
    ~udp24l01object();
    Q_INVOKABLE QStringList getCrazySetting(int itemIndex)const;
    Q_INVOKABLE void sendCrazySetting(int itemIndex,int index);
    Q_INVOKABLE void sendStartPacket(int index);
    Q_INVOKABLE void sendCommand(){radioPacket->sendCommandUDP24L01();}
    Q_INVOKABLE void updateCommandParams(int robotID,int velX,int velY,int velR,bool dribble,int dribbleLevel,bool mode,bool shoot,int power){
        radioPacket->updateCommandParams(robotID,velX,velY,velR,dribble,dribbleLevel,mode,shoot,power);
    }
private:
    QStringList stringFrequency;
    int addressIndex;
    int frequencyIndex;
    QSerialPort serial;
    QUdpSocket *udpSender,*udpReceiver;
    RadioPacket *radioPacket;
};

#endif // UDP24L01OBJECT_H
