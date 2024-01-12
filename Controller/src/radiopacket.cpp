#include "radiopacket.h"
#include "crc.h"
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
    void encodeLegacy(const NJ_Command& command,QByteArray& tx,int num);
}
RadioPacket::RadioPacket(QSerialPort* serialPtr)
    : startPacket1(TRANSMIT_PACKET_SIZE,0)
    , startPacket2(TRANSMIT_PACKET_SIZE,0)
    , transmitPacket(TRANSMIT_PACKET_SIZE,0)
    , serialPtr(serialPtr), frequency(0)
    , shoot(false), ctrl(false), shootMode(false), robotID(0)
    , velX(0), velY(0), velR(0)
    , ctrlPowerLevel(2), shootPowerLevel(0)
    , packageType(0x40),gameStatus(0x00){

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
    encode();
}

bool RadioPacket::sendStartPacket(){
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
bool RadioPacket::sendCommand(){
    static int times = 0;
    static QElapsedTimer timer;
    if(times == 0) timer.start();
    if(serialPtr != NULL){
        encode();
//        qDebug() << "0x" << transmitPacket.toHex();
        //transmitPacket是包含命令的包
        transmitPacket.data();
        serialPtr->write((transmitPacket.data()),TRANSMIT_PACKET_SIZE);
        serialPtr->flush();
//        sendSocket.writeDatagram(transmitPacket.data(), 25 ,QHostAddress("10.12.225.78"),1030);
        return true;
    }
    return false;
}

bool RadioPacket::encode(){
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

    encodeLegacy(NJ_CMD,tx,0);

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
namespace{
void encodeLegacy(const NJ_Command& command,QByteArray& tx,int num){
    auto& TXBuff = tx;
    int i = num;
    int real_num = command.id;
    qint16 vx = (qint16)(command.vx);
    qint16 vy = (qint16)(command.vy);
    qint16 ivr = (qint16)(command.vr);
    qint16 vr = abs(ivr)> 511 ? (ivr > 0 ? 1 : -1)*(511) : (ivr);
    qint16 power = (qint16)(command.power);
    bool kick_mode = command.kick_mode;
    qint16 dribble = (qint16)(command.dribble+0.1);
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
}
