#include <QtSerialPort/QSerialPortInfo>
#include "serialobject.h"
SerialObject::SerialObject(QObject *parent):QObject(parent){
    // add needed settings
    char tempIndex[] = {-1,8};

    udpSender = new QUdpSocket();
    udpReceiver = new QUdpSocket();
    radioPacket = new RadioPacket(&serial,this,udpSender,udpReceiver);

    const auto& ports = QSerialPortInfo::availablePorts();
    for(const auto &port : ports){
        this->ports.append(port.portName());
        tempIndex[0]++;
    }
    defaultIndex.append(tempIndex,2);
    currentIndex.append(tempIndex,2);
    settingsName.append("Ports");
    settingsName.append("BaudRate");
    settingsName.append("DataBits");
    settingsName.append("Parity");
    settingsName.append("StopBits");
    settingsName.append("Frequency");
    baudRate.append(QSerialPort::Baud115200);   stringBaudRate.append("115200");
    dataBits.append(QSerialPort::Data8);        stringDataBits.append("7");
    parity.append(QSerialPort::NoParity);       stringParity.append("OddParity");
    stopBits.append(QSerialPort::TwoStop);      stringStopBits.append("Two");
    for(int i = 0; i < 16;++i){
        frequency.append(i);
    }
    for(int i = 0; i < 16;++i){
        stringFrequency.append(QString::number(i));
    }
}

SerialObject::~SerialObject(){
    delete udpSender;
    delete udpReceiver;
}

QString SerialObject::getName(int itemIndex) const{
    return settingsName[itemIndex];
}
QStringList SerialObject::getCrazySetting(int itemIndex){
    switch(itemIndex){
    case 0:{
        this->ports.clear();
        const auto& ports1 = QSerialPortInfo::availablePorts();
        for(const auto &port : ports1){
            this->ports.append(port.portName());
            //tempIndex[0]++;
        }
            return ports;
        }
    case 1:
        return stringFrequency;
    default:
        break;
    }
    return QStringList();
}
void SerialObject::sendCrazySetting(int itemIndex,int index){
    currentIndex[itemIndex] = index;
//    qDebug() << "---------------------";
//    qDebug() << "port name -- " << (ports.size() > 0 ? ports[currentIndex[0]] : "");
//    qDebug() << "baud rate -- " << stringBaudRate[0];
//    qDebug() << "data bits -- " << stringDataBits[0];
//    qDebug() << "parity    -- " << stringParity[0];
//    qDebug() << "stop bits -- " << stringStopBits[0];
//    qDebug() << "---------------------";
//    qDebug() << "frequency -- " << stringFrequency[currentIndex[1]];
//    qDebug() << "---------------------" << endl;
}
int SerialObject::getDefaultIndex(int itemIndex) const{
    return defaultIndex[itemIndex];
}
void SerialObject::openSerialPort(){
    serial.setPortName(ports[currentIndex[0]]);
    serial.setBaudRate(baudRate[0]);
    serial.setDataBits(dataBits[0]);
    serial.setParity(parity[0]);
    serial.setStopBits(stopBits[0]);

    if (serial.open(QIODevice::ReadWrite)) {
        qDebug() << "SerialPort Connected...";
    } else {
        qDebug() << "SerialPort connect failed..." << serial.error();
    }
}
void SerialObject::closeSerialPort(){
    if (serial.isOpen()){
        serial.close();
        qDebug() << "SerialPort Disconnected...";
    }
}
void SerialObject::sendStartPacket(){
    qDebug() << "Set Frequency... : " << frequency[currentIndex[1]];
    radioPacket->updateFrequency(frequency[currentIndex[1]]);
    radioPacket->sendStartPacketSerial();
}
