#include "udp24l01object.h"


namespace{
const QStringList radioSendAddress2choose =  {"10.12.225.240", "10.12.225.241","10.12.225.109","10.12.225.78"};
const QStringList radioReceiveAddress2choose =  {"10.12.225.240", "10.12.225.241","10.12.225.110","10.12.225.79"};
}

udp24l01object::udp24l01object(QObject *parent):QObject(parent)
{
    udpSender = new QUdpSocket();
    udpReceiver = new QUdpSocket();
    radioPacket = new RadioPacket(&serial,this,udpSender,udpReceiver);
    addressIndex = 0;
    radioPacket->updateAddress(QHostAddress(radioSendAddress2choose[addressIndex]),QHostAddress(radioReceiveAddress2choose[addressIndex]));
    frequencyIndex = 0;
    for(int i = 0;i < 16; ++i){
        stringFrequency.append(QString::number(i));
    }
}

udp24l01object::~udp24l01object(){
    delete udpSender;
    delete udpReceiver;
}

QStringList udp24l01object::getCrazySetting(int itemIndex) const{
    switch(itemIndex){
    case 0:
        return radioSendAddress2choose;
    case 1:
        return stringFrequency;
    default:
        break;
    }
    return QStringList();
}

void udp24l01object::sendCrazySetting(int itemIndex,int index){
    switch(itemIndex){
    case 0:
        addressIndex = index;
        radioPacket->updateAddress(QHostAddress(radioSendAddress2choose[addressIndex]),QHostAddress(radioReceiveAddress2choose[addressIndex]));
    case 1:
        frequencyIndex = index;
    default:
        break;
    }
}
void udp24l01object::sendStartPacket(int index){
    radioPacket->sendStartPacketUdp24L01(index);
}
