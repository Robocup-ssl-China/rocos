#ifndef RADIOPACKET_H
#define RADIOPACKET_H

#include <QSerialPort>
#include <QUdpSocket>
#include <QtDebug>
class RadioPacket : public QObject
{
public:
    explicit RadioPacket(QSerialPort* serialPtr,QObject *parent,QUdpSocket* udpSender,QUdpSocket* udpReceiver);
    bool sendStartPacketSerial();
    void sendStartPacketUdp24L01(int frequency);
    bool sendCommandNJ();
    bool sendCommandUDP24L01();
    void updateCommandParams(int robotID,int velX,int velY,int velR,bool ctrl,int ctrlLevel,bool mode,bool shoot,int power){
        this->robotID = robotID;
        this->velX = velX; this->velY = velY; this->velR = velR;
        this->ctrl = ctrl;
        this->shootMode = mode;this->shoot = shoot; this->shootPowerLevel = power;
        this->ctrlPowerLevel = ctrlLevel;
    }
    void updateAddress(const QHostAddress& sendaddress,const QHostAddress& receiveAddress);
    void updateFrequency(int);
private:
    static const int TRANSMIT_PACKET_SIZE = 25;
    static const int TRANS_FEEDBACK_SIZE = 20;
    QByteArray startPacket1;
    QByteArray startPacket2;
    QByteArray transmitPacket;
    QSerialPort* serialPtr;
    int frequency;
    QUdpSocket* udpSender,* udpReceiver;
    bool encodenj();
    bool encodeudp24l01();
    QHostAddress sendaddress, receiveAddress;
private:
    QUdpSocket sendSocket,receiveSocket;
    bool shoot;
    bool ctrl;
    bool shootMode;//false is "flat shoot" and true is "lift shoot".
    quint8 robotID;
    //packageTypes: 0x00, 0x40, 0x80, 0xc0
    quint8 packageType = 0x00;
    //gameStatus: 0x00, 0x10, 0x20, 0x30
    quint8 gameStatus = 0x00;
    qint16 velX;
    qint16 velY;
    qint16 velR;
    quint16 ctrlPowerLevel;
    quint16 shootPowerLevel;
};

#endif // RADIOPACKET_H
