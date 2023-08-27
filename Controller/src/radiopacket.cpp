#include "radiopacket.h"
#include "lib/crc/crc.h"
#include <QSerialPort>
#include <QElapsedTimer>
#include <cstdio>
namespace  {
    struct NJ_Command{
        float power;
        float dribble;
        float vx;
        float vy;
        float vr;
        int id;
        bool valid;
        bool kick_mode;
    };
    struct Udp24l01_Command{
        float power;
        float dribble;
        float vx;
        float vy;
        float vr;
        int id;
        bool valid;
        bool kick_mode;
    };
    const int TRANSMIT_START_PACKET_SIZE = 25;
    const int START_PACKET_SIZE = 6;
    const int PORT_SEND = 1030;
    void encodeLegacy(const Udp24l01_Command& command,QByteArray& tx,int num);
    void encodeLegacyNJ(const NJ_Command& command,QByteArray& tx,int num);
}
RadioPacket::RadioPacket(QSerialPort* serialPtr,QObject *parent,QUdpSocket* udpSender,QUdpSocket* udpReceiver)
    : QObject(parent)
    , startPacket1(TRANSMIT_PACKET_SIZE,0)
    , startPacket2(TRANSMIT_PACKET_SIZE,0)
    , transmitPacket(TRANSMIT_PACKET_SIZE,0)
    , serialPtr(serialPtr), frequency(0)
    , udpSender(udpSender),udpReceiver(udpReceiver)
    , shoot(false), ctrl(false), shootMode(false), robotID(0)
    , velX(0), velY(0), velR(0)
    , ctrlPowerLevel(2), shootPowerLevel(0)
    , packageType(0x40),gameStatus(0x00){
}

bool RadioPacket::sendStartPacketSerial(){
    startPacket1[0] = 0xff;
    startPacket1[1] = 0xb0;
    startPacket1[2] = 0x01;
    startPacket1[3] = 0x02;
    startPacket1[4] = 0x03;
    startPacket1[TRANSMIT_PACKET_SIZE - 1] = CCrc8::calc((unsigned char*)(startPacket1.data()), TRANSMIT_PACKET_SIZE - 1);

    startPacket2[0] = 0xff;
    startPacket2[1] = 0xb0;
    startPacket2[2] = 0x04;
    startPacket2[3] = 0x05;
    startPacket2[4] = 0x06;
    encodenj();
    if(serialPtr != NULL){
        //send startPacket1，第一次握手
        serialPtr->write((startPacket1.data()),TRANSMIT_PACKET_SIZE);
        serialPtr->flush();
        if (serialPtr->waitForBytesWritten(2000)) {
            if (serialPtr->waitForReadyRead(2000)) {
                //收到包，第二次握手
                QByteArray responseData = serialPtr->readAll();
                while (serialPtr->waitForReadyRead(10))
                    responseData += serialPtr->readAll();
            }
        } else {
            qDebug() << "Start packet write timeout!";
        }
        //send startPacket2，第三次握手
        serialPtr->write((startPacket2.data()),TRANSMIT_PACKET_SIZE);
        serialPtr->flush();
        return true;
    }
    return false;
}
void RadioPacket::updateFrequency(int frequency){
    this->frequency = frequency;
    startPacket2[5] = 0x10 + frequency;
    startPacket2[TRANSMIT_PACKET_SIZE - 1] = CCrc8::calc((unsigned char*)(startPacket2.data()), TRANSMIT_PACKET_SIZE - 1);
}
//发送指令
bool RadioPacket::sendCommandNJ(){
    static int times = 0;
    static QElapsedTimer timer;
    if(times == 0) timer.start();
    if(serialPtr != NULL){
        encodenj();
        qDebug() << "0x" << transmitPacket.toHex();
        //transmitPacket是包含命令的包
        transmitPacket.data();
        serialPtr->write((transmitPacket.data()),TRANSMIT_PACKET_SIZE);
        serialPtr->flush();
//        sendSocket.writeDatagram(transmitPacket.data(), 25 ,QHostAddress("10.12.225.78"),1030);
        return true;
    }
    return false;
}

bool RadioPacket::sendCommandUDP24L01(){
    static int times = 0;
    static QElapsedTimer timer;
    if(times == 0) timer.start();
    if(udpSender != NULL){
        encodeudp24l01();
//        if(shootPowerLevel > 0 && shoot == true){
            qDebug() << "sent : 0x" << transmitPacket.toHex() << shootPowerLevel;
//        }
        udpSender->writeDatagram(transmitPacket.data(),TRANSMIT_PACKET_SIZE, sendaddress, PORT_SEND);
        return true;
    }
    return false;
}

bool RadioPacket::encodenj(){
    NJ_Command NJ_CMD;

    NJ_CMD.valid = true;
    NJ_CMD.id = this->robotID;
    NJ_CMD.vx = this->velX;
    NJ_CMD.vy = this->velY;
    NJ_CMD.vr = this->velR;
    NJ_CMD.dribble = this->ctrl ? this->ctrlPowerLevel : 0;
    NJ_CMD.power = this->shoot ? this->shootPowerLevel : 0;
    NJ_CMD.kick_mode = this->shootMode;

    auto& tx = transmitPacket;
    int count = 0;
    tx.fill(0x00);
    tx[0] = 0xff;
    tx[21] = ((this->frequency&0x0f)<<4) | 0x07;

    encodeLegacyNJ(NJ_CMD,tx,0);

    qDebug() << tx.toHex();

//    transmitPacket[0] = packageType | gameStatus;
//    //RobotID
//    transmitPacket[1] = (robotID) & 0x0f;
//    transmitPacket[0] = transmitPacket[0] | 0x08;
//    //Robot1 Config
//    //shoot or chip
//    transmitPacket[1] = transmitPacket[1] | (shootMode << 6 );
//    //power level
//    transmitPacket[1] = transmitPacket[1] | (ctrl ? (ctrlPowerLevel << 4):0);
//    //速度的低位
//    transmitPacket[2] = ((velX >= 0)?0:0x80) | (abs(velX) & 0x7f);
//    transmitPacket[3] = ((velY >= 0)?0:0x80) | (abs(velY) & 0x7f);
//    transmitPacket[4] = ((velR >= 0)?0:0x80) | (abs(velR) & 0x7f);
//    //Don't understand !
//    if(transmitPacket[2] == char(0xff)) transmitPacket[4] = 0xfe;
//    if(transmitPacket[3] == char(0xff)) transmitPacket[5] = 0xfe;
//    if(transmitPacket[4] == char(0xff)) transmitPacket[6] = 0xfe;
//    //clear Byte[17-24]
//    transmitPacket[17] = transmitPacket[18] = transmitPacket[19] = transmitPacket[20] = transmitPacket[21] = transmitPacket[22] = transmitPacket[23] = transmitPacket[24] = 0;
//    //速度的高位
//    transmitPacket[17] = ((abs(velX) & 0x180) >> 1) | ((abs(velY) & 0x180) >> 3) | ((abs(velR) & 0x780) >> 7);
//    //shoot power
//    transmitPacket[21] = (shoot ? shootPowerLevel:0) & 0x7f;
    return true;
}

bool RadioPacket::encodeudp24l01(){
    Udp24l01_Command Udp24l01_CMD;

    Udp24l01_CMD.valid = true;
    Udp24l01_CMD.id = this->robotID;
    Udp24l01_CMD.vx = this->velX;
    Udp24l01_CMD.vy = this->velY;
    Udp24l01_CMD.vr = this->velR;
    Udp24l01_CMD.dribble = this->ctrl ? this->ctrlPowerLevel : 0;
    Udp24l01_CMD.power = this->shoot ? this->shootPowerLevel : 0;
    Udp24l01_CMD.kick_mode = this->shootMode;

    auto& tx = transmitPacket;
    tx.fill(0x00);
    tx[0] = 0x40;

    encodeLegacy(Udp24l01_CMD,tx,0);

    qDebug() << tx.toHex();
    return true;
}

void RadioPacket::sendStartPacketUdp24L01(int frequency) {
    // this 't' is id
    QByteArray startPacketSend(TRANSMIT_START_PACKET_SIZE, 0);
    QByteArray startPacketReceive(TRANSMIT_START_PACKET_SIZE, 0);
    int freq_encode = 0;
    if(frequency < 8){
        freq_encode = frequency*4;
    }else{
        freq_encode = frequency*4+58;
    }
    startPacketSend[0] = (char)0xf0;
    startPacketSend[1] = (char)(freq_encode & 0xff);
    startPacketSend[2] = (char)(freq_encode & 0xff);
    startPacketSend[3] = (char)0x01;
    startPacketSend[4] = (char)0x02;
    int temp_value = 0;
    for(int i=0;i<TRANSMIT_START_PACKET_SIZE-1;i++){
        temp_value += startPacketSend[i];
    }
    startPacketSend[TRANSMIT_START_PACKET_SIZE-1] = (char)(temp_value & 0xff);

    startPacketReceive[0] = (char)0xf0;
    startPacketReceive[1] = (char)(freq_encode & 0xff);
    startPacketReceive[2] = (char)(freq_encode & 0xff);
    startPacketReceive[3] = (char)0x02;
    startPacketReceive[4] = (char)0x02;
    temp_value = 0;
    for(int i=0;i<TRANSMIT_START_PACKET_SIZE-1;i++){
        temp_value += startPacketReceive[i];
    }
    startPacketReceive[TRANSMIT_START_PACKET_SIZE-1] = (char)(temp_value & 0xff);
    sendSocket.writeDatagram(startPacketSend, TRANSMIT_START_PACKET_SIZE, sendaddress,PORT_SEND);
    receiveSocket.writeDatagram(startPacketReceive, TRANSMIT_START_PACKET_SIZE, receiveAddress, PORT_SEND);
//    std::this_thread::sleep_for(std::chrono::milliseconds(4));
    qDebug() << "Frequency:" << frequency << " Send IP:" << sendaddress << " Receive IP:" << receiveAddress;
}

void RadioPacket::updateAddress(const QHostAddress& sendaddress,const QHostAddress& receiveAddress){
    this->sendaddress = sendaddress;
    this->receiveAddress = receiveAddress;
    qDebug() << "get address : " << sendaddress << receiveAddress;
}

namespace{
void encodeLegacyNJ(const NJ_Command& command,QByteArray& tx,int num){
    auto& TXBuff = tx;
    int i = num;
    int real_num = command.id;
    qint16 vx = (qint16)(command.vx);
    qint16 vy = (qint16)(command.vy);
    qint16 ivr = (qint16)(command.vr);
    qint16 vr = abs(ivr)> 511 ? (ivr > 0 ? 1 : -1)*(511) : (ivr);
    qint16 power = (qint16)(command.power);
    bool kick_mode = command.kick_mode;
    qint16 dribble = (qint16)(command.dribble*3+0.4);
    // vx
    unsigned int vx_value_uint = (unsigned int)abs(vx);
    // vy
    unsigned int vy_value_uint = (unsigned int)abs(vy);
    // w
    unsigned int w_value_uint = (unsigned int)abs(vr);

    if(real_num >= 8)
        TXBuff[1] = TXBuff[1] | 0x01 << (real_num - 8);

    TXBuff[2] =  TXBuff[2] | 0x01 << real_num;
    // dribble & kick_mode
    TXBuff[6*i + 3] = 0x01 | ((((kick_mode?0x01:0x00)<<2)|(0x03 & dribble))<<4);

    // velx
    if(vx < 0) TXBuff[6*i + 4] = TXBuff[6*i + 4] | (0x20);
    TXBuff[6*i + 4] = TXBuff[6*i + 4] | ((vx_value_uint & 0x1f0) >> 4);
    TXBuff[6*i + 5] = TXBuff[6*i + 5] | ((vx_value_uint & 0x0f) << 4);
    // vely
    if(vy < 0) TXBuff[6*i + 5] = TXBuff[6*i + 5] | (0x08);
    TXBuff[6*i + 5] = TXBuff[6*i + 5] | ((vy_value_uint & 0x1c0) >> 6);
    TXBuff[6*i + 6] = TXBuff[6*i + 6] | ((vy_value_uint & 0x3f) << 2);
    // w
    if(vr < 0) TXBuff[6*i + 6] = TXBuff[6*i + 6] | 0x02;
    TXBuff[6*i + 6] = TXBuff[6*i + 6] | ((w_value_uint & 0x100) >> 8);
    TXBuff[6*i + 7] = TXBuff[6*i + 7] | (w_value_uint & 0x0ff);

    // shoot power
    TXBuff[6*i + 8] = (quint8)power;

}
void encodeLegacy(const Udp24l01_Command& command, QByteArray& tx, int num) {
    // send back to vision module
    // num 0 ~ 3
    // id  0 ~ 15
    quint8 id = (quint8)command.id;
    qint16 vx = (qint16)(command.vx);
    qint16 vy = (qint16)(command.vy);
    qint16 ivr = (qint16)(command.vr);
    qint16 vr = abs(ivr)> 1011 ? (ivr > 0 ? 1 : -1)*(1011) : (ivr);
    qint16 abs_vx = std::abs(vx);
    qint16 abs_vy = std::abs(vy);
    qint16 abs_vr = std::abs(vr);
    bool kick = command.kick_mode;
    quint8 power = command.power;

    qint8 dribble = command.dribble > 0.5 ? 3 : 0;
    tx[0] = (tx[0]) | (1 << (3 - num));
    tx[num * 4 + 1] = ((quint8)kick << 6) | dribble << 4 | id;
    tx[num * 4 + 2] = (vx >> 8 & 0x80) | (abs_vx & 0x7f);
    tx[num * 4 + 3] = (vy >> 8 & 0x80) | (abs_vy & 0x7f);
    tx[num * 4 + 4] = (vr >> 8 & 0x80) | (abs_vr & 0x7f);
    tx[num  + 17] = (abs_vx >> 1 & 0xc0) | (abs_vy >> 3 & 0x30) | (abs_vr >> 7 & 0x0f);
    tx[num  + 21] = power;
}
}
