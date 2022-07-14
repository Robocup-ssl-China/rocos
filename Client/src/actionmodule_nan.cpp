#include "actionmodule.h"
#include "actionmodule.h"
#include "parammanager.h"
#include "globaldata.h"
#include <QtSerialPort/QSerialPortInfo>
#include "crc.h"
#include <thread>
#include <chrono>
#include <QtDebug>
namespace ZSS {
namespace{
    const int TRANSMIT_PACKET_SIZE = 25;
    const int TRANS_FEEDBACK_SIZE = 25;
    auto zpm = ZSS::ZParamManager::instance();
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
    void encodeLegacy(const NJ_Command&,QByteArray&,int);
    quint8 kickStandardization(quint8,bool,quint8);

    NJ_Command NJ_CMDS[PARAM::ROBOTMAXID];
}
ActionModuleSerialVersion::ActionModuleSerialVersion(QObject *parent) : QObject(parent) {
    updatePortsList();
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    zpm->loadParam(frequency,"Alert/Frequency",8);
    tx.resize(TRANSMIT_PACKET_SIZE);
    rx.resize(TRANS_FEEDBACK_SIZE);
    tx[0] = 0x40;
    connect(&serial, &QSerialPort::readyRead, this, &ActionModuleSerialVersion::readData);
}
ActionModuleSerialVersion::~ActionModuleSerialVersion(){
    closeSerialPort();
}
QStringList& ActionModuleSerialVersion::updatePortsList(){
    this->ports.clear();
    const auto& ports = QSerialPortInfo::availablePorts();
    for(const auto &port : ports){
        this->ports.append(port.portName());
    }
    if(this->ports.size() > 0)
        serial.setPortName(this->ports[this->ports.size() - 1]);
    return this->ports;
}
void ActionModuleSerialVersion::setMedusaSettings(bool color,bool side){
    _color = color ? 1 : 0;// 1 for Yellow , 0 for Blue
    _side = side ? 1 : 0;// 1 for right , 0 for left
}
bool ActionModuleSerialVersion::init(){
    if (serial.isOpen()) {
        serial.close();
    }
    if (serial.open(QIODevice::ReadWrite)) {
        qDebug() << "SerialPort connected... : " << serial.portName();
        sendStartPacket();
        return true;
    }
    qDebug() << "SerialPort connect failed... : " << serial.portName();
    return false;
}
void ActionModuleSerialVersion::sendStartPacket(){
    QByteArray startPacket1(TRANSMIT_PACKET_SIZE,0);
    QByteArray startPacket2(TRANSMIT_PACKET_SIZE,0);
    startPacket1[0] = (char)0xff;
    startPacket1[1] = (char)0xb0;
    startPacket1[2] = (char)0x01;
    startPacket1[3] = (char)0x02;
    startPacket1[4] = (char)0x03;
    startPacket1[TRANSMIT_PACKET_SIZE - 1] = CCrc8::calc((unsigned char*)(startPacket1.data()), TRANSMIT_PACKET_SIZE - 1);

    startPacket2[0] = (char)0xff;
    startPacket2[1] = (char)0xb0;
    startPacket2[2] = (char)0x04;
    startPacket2[3] = (char)0x05;
    startPacket2[4] = (char)0x06;
    startPacket2[5] = 0x10 + frequency;
    startPacket2[TRANSMIT_PACKET_SIZE - 1] = CCrc8::calc((unsigned char*)(startPacket2.data()), TRANSMIT_PACKET_SIZE - 1);
    serial.write(startPacket1);
    serial.flush();
    if(serial.waitForBytesWritten(2000)){
        if(serial.waitForReadyRead(2000)){
            serial.readAll();
            while (serial.waitForReadyRead(10))
                serial.readAll();
        }
    }else{
        qDebug() << "Start packet write timeout!";
    }
    serial.write(startPacket2);
    serial.flush();
}
bool ActionModuleSerialVersion::changePorts(int portNum){
    if(portNum < ports.size() && portNum >= 0){
        serial.setPortName(ports[portNum]);
        return true;
    }
    serial.setPortName("");
    return false;
}
bool ActionModuleSerialVersion::changeFrequency(int frequency){
    if(frequency >= 0 && frequency <= 15){
        this->frequency = frequency;
        zpm->changeParam("Alert/Frequency",frequency);
        return true;
    }
    return false;
}
void ActionModuleSerialVersion::sendLegacy(const ZSS::Protocol::Robots_Command& commands){
    int size = commands.command_size();
    for(int i=0;i<size;i++){
        auto& command = commands.command(i);
        int id = command.robot_id();
        if(id >= 0 && id < PARAM::ROBOTMAXID){
            NJ_CMDS[id].valid = true;
            NJ_CMDS[id].id = id;
            NJ_CMDS[id].vx = command.velocity_x()/10.0;
            NJ_CMDS[id].vy = -command.velocity_y()/10.0;
            NJ_CMDS[id].vr = -command.velocity_r()*40;
            NJ_CMDS[id].dribble = command.dribbler_spin();
            NJ_CMDS[id].power = command.power()/10.0;
            NJ_CMDS[id].kick_mode = command.kick();
        }
    }

    int count = 0;
    tx.fill(0x00);
    tx[0] = 0xff;
    tx[21] = ((this->frequency&0x0f)<<4) | 0x07;

    for(int i=0;i<PARAM::ROBOTMAXID;i++){
        if(NJ_CMDS[i].valid){
            if(count == 3){
                serial.write(this->tx.data(),TRANSMIT_PACKET_SIZE);
                serial.flush();
                std::this_thread::sleep_for(std::chrono::milliseconds(4));
                qDebug() << tx.toHex();
                tx.fill(0x00);
                tx[0] = 0xff;
                tx[21] = ((this->frequency&0x0f)<<4) | 0x07;
                count = 0;
            }
            encodeLegacy(NJ_CMDS[i],this->tx,count++);
        }
    }
    serial.write(this->tx.data(),TRANSMIT_PACKET_SIZE);
    serial.flush();
    qDebug() << tx.toHex();

    for(int i=0;i<PARAM::ROBOTMAXID;i++){
        NJ_CMDS[i].valid = false;
    }

}
bool ActionModuleSerialVersion::openSerialPort(){
    if (serial.open(QIODevice::ReadWrite)) {
        qDebug() << "SerialPort connected... : " << serial.portName();
        return true;
    }
    qDebug() << "SerialPort connect failed... : " << serial.portName();
    return false;
}
bool ActionModuleSerialVersion::closeSerialPort(){
    if (serial.isOpen()) {
        serial.close();
        qDebug() << "SerialPort Disconnected... : " << serial.portName();
        return true;
    }
    return false;
}
void ActionModuleSerialVersion::readData(){
    rx = serial.readAll();
    auto& data = rx;
    int id = 0;
    bool infrared = false;
    bool flat = false;
    bool chip = false;
    int battery = 0;
    int capacitance = 0;
    if(data[0] == (char)0xff){
        if(data[1] == (char)0x02){
            id       = (quint8)data[2] - 1;//old protocal
            infrared = (quint8)data[3] & 0x40;
            flat     = (quint8)data[3] & 0x20;
            chip     = (quint8)data[3] & 0x10;
            GlobalData::instance()->robotInfoMutex.lock();
            GlobalData::instance()->robotInformation[_color][id].infrared = infrared;
            GlobalData::instance()->robotInformation[_color][id].flat = flat;
            GlobalData::instance()->robotInformation[_color][id].chip = chip;
            GlobalData::instance()->robotInfoMutex.unlock();
            emit receiveRobotInfo(_color, id);
            qDebug() << id << ' ' << infrared << ' ' << flat << ' ' << chip << ' ' << battery << ' ' << capacitance;
        }
//        else if(data[1] == (char)0x01){
//            id          = (quint8)data[2];
//            battery     = (quint8)data[3];
//            capacitance = (quint8)data[4];
//        }
    }

}
namespace{
void encodeLegacy(const NJ_Command& command,QByteArray& tx,int num){
//    // num 0 ~ 3
//    // id  0 ~ 11
//    int id = command.robot_id();
//    // velocity  m/s -> cm/s
//    qint16 vx = (qint16)(command.velocity_x()*100);
//    qint16 vy = (qint16)(command.velocity_y()*100);
//    qint16 vr = (qint16)(command.velocity_r()*40);
//    qint16 abs_vx = std::abs(vx);
//    qint16 abs_vy = std::abs(vy);
//    qint16 abs_vr = std::abs(vr);
//    // flat&chip m/s -> cm/s
//    // kick   1 : chip   0 : flat
//    bool kick = command.kick();
//    quint8 power = kickStandardization(id,kick,(quint8)(command.power()*100));
//    // dribble -1 ~ +1 -> -3 ~ +3
//    qint8 dribble = (qint8)(command.dribbler_spin()*3.1);
//    // TODO
//    // TXBuff[0] = TXBuff[0] | (1 << (3-i));
//    tx[0] = (tx[0])|(1<<(3-num));
//    tx[num*4 + 1] = (dribble&0x80)|((quint8)kick<<6)|(dribble<<4 & 0x30)|id;
//    tx[num*4 + 2] = (vx>>8 & 0x80)|(abs_vx & 0x7f);
//    tx[num*4 + 3] = (vy>>8 & 0x80)|(abs_vy & 0x7f);
//    tx[num*4 + 4] = (vr>>8 & 0x80)|(abs_vr & 0x7f);
//    tx[num  + 17] = (abs_vx>>1 & 0xc0)|(abs_vy>>3 & 0x30)|(abs_vr>>7 & 0x0f);
//    tx[num  + 21] = power;

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
    quint8 p = command.power > 0.01 ? std::max(10.0, std::min((double)power, 127.0)) : 0;
    TXBuff[6*i + 8] = p;

}
quint8 kickStandardization(quint8 id,bool mode,quint8 power){
    // TODO
    return power;
}
} // namespace ZSS::anonymous
}

