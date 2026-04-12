#include "debugger.h"
#include "globaldata.h"

Debugger::Debugger(QObject *parent) : QObject(parent){
    if (receiverBlue.bind(QHostAddress::AnyIPv4,
                          ZSS::Athena::DEBUG_MSG_RECEIVE[0],
                          QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        QObject::connect(&receiverBlue, &QUdpSocket::readyRead, this, &Debugger::receiveBlue);
    }
    if (receiverYellow.bind(QHostAddress::AnyIPv4,
                            ZSS::Athena::DEBUG_MSG_RECEIVE[1],
                            QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        QObject::connect(&receiverYellow, &QUdpSocket::readyRead, this, &Debugger::receiveYellow);
    }
}

void Debugger::receiveBlue(){
    auto& datagram = GlobalData::instance()->debugBlueMessages;
    while (receiverBlue.state() == QUdpSocket::BoundState && receiverBlue.hasPendingDatagrams()) {
        QMutexLocker lock(&GlobalData::instance()->debugMutex);
        datagram.resize(receiverBlue.pendingDatagramSize());
        receiverBlue.readDatagram(datagram.data(), datagram.size());
    }
}

void Debugger::receiveYellow(){
    auto& datagram = GlobalData::instance()->debugYellowMessages;
    while (receiverYellow.state() == QUdpSocket::BoundState && receiverYellow.hasPendingDatagrams()) {
        QMutexLocker lock(&GlobalData::instance()->debugMutex);
        datagram.resize(receiverYellow.pendingDatagramSize());
        receiverYellow.readDatagram(datagram.data(), datagram.size());
    }
}
