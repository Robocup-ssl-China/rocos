#include "actionmodule.h"
#include "parammanager.h"
#include "globaldata.h"
#include "dealrobot.h"
#include "crc.h"
#include "messageinfo.h"
#include <chrono>
#include <thread>
#include <cmath>
#include <fmt/core.h>
#include <QtSerialPort/QSerialPortInfo>
#include "staticparams.h"
#include "networkinterfaces.h"
namespace ZSS {
namespace {
const int TRANSMIT_PACKET_SIZE = 25;
const int TRANS_FEEDBACK_SIZE = 26;
const int TRANSMIT_START_PACKET_SIZE = 6;
const int PORT_SEND = 1030;
const int PORT_RECEIVE = 1030;
const double FULL_BATTERY = 224.0;
const double LOW_BATTERY = 196.0;
const double FULL_CAPACITANCE = 254.0;
const double LOW_CAPACITANCE = 29.0;
auto zpm = ZSS::ZParamManager::instance();
// void encodeLegacy(const ZSS::Protocol::Robot_Command&, QByteArray&, int);
quint8 linearKickMapping(quint8 id, bool kick_mode, float power);
const QStringList radioSendAddress2choose=  {"10.12.225.240", "10.12.225.241","10.12.225.109","10.12.225.78"};
const QStringList radioReceiveAddress2choose =  {"10.12.225.240", "10.12.225.241","10.12.225.110","10.12.225.79"};
QString radioSendAddress[PARAM::TEAMS] = {radioSendAddress2choose[0],radioSendAddress2choose[1]};
QString radioReceiveAddress[PARAM::TEAMS] = {radioReceiveAddress2choose[0],radioReceiveAddress2choose[1]};
// int blue_sender_interface,blue_receiver_interface,yellow_sender_interface,yellow_receiver_interface;
// std::thread* receiveThread = nullptr;
bool IS_SIMULATION;

const int SERIAL_TRANSMIT_PACKET_SIZE = 25;
const int SERIAL_TRANS_FEEDBACK_SIZE = 25;
struct NJ_Command{
    float power;
    float dribble;
    float vx;
    float vy;
    float vr;
    int id;
    bool valid;
    bool kick_mode;
    bool use_dir;
    bool need_report;
    bool direct_kick_no_calibration;
    float direct_kick_power;
};
void encodeNJLegacy(const NJ_Command&,QByteArray&,int);
NJ_Command NJ_CMDS[PARAM::ROBOTMAXID];
}
// Action_Serial24L01::Action_Serial24L01(const Config& config,const __callback_type& f,QObject *parent):QObject(parent),_cb(f),_config(config){}
// Action_Serial24L01::~Action_Serial24L01(){
//     disconnect();
// }
// bool Action_Serial24L01::start(){
//     return false;
// }
// bool Action_Serial24L01::stop(){
//     return true;
// }
// bool Action_Serial24L01::send(const char* ptr,const size_t size){

// }
// bool sendConfigPacket(const char* ptr,const size_t size){
//     return true;
// }
// void Action_Serial24L01::receiveData(){
//     static QByteArray datagram;
// //    while (_recvSocket.hasPendingDatagrams()) {
// //        datagram.resize(_recvSocket.pendingDatagramSize());
// //        _recvSocket.readDatagram(datagram.data(), datagram.size());
// //        if(_cb){
// //            std::invoke(_cb,datagram.data(),datagram.size());
// //        }
// //    }
// }

// Action_UDP24L01::Action_UDP24L01(const Config& config,const __callback_type& f,QObject *parent):QObject(parent),_cb(f),_config(config){}
// Action_UDP24L01::~Action_UDP24L01(){
//     disconnect();
// }
// bool Action_UDP24L01::start(){
//     if(_recvSocket.bind(QHostAddress::AnyIPv4, _config.recv_port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
//         QObject::connect(&_recvSocket, SIGNAL(readyRead()), this, SLOT(receiveData()), Qt::DirectConnection);
//         return true;
//     }
//     qDebug() << "Bind Error in Action_UDP24L01 !";
//     return false;
// }
// bool Action_UDP24L01::stop(){
//     QObject::disconnect(&_recvSocket);
//     _recvSocket.abort();
//     _recvSocket.disconnectFromHost();
//     return true;
// }
// void Action_UDP24L01::reConfig(const Config& config){
//     _config = config;
//     // stop();
//     // start();
// }
// bool Action_UDP24L01::sendConfigPacket(const char* ptr,const size_t size){
//     _sendSocket.writeDatagram(ptr,size,QHostAddress(_config.send_ip),_config.send_port);
//     _sendSocket.writeDatagram(ptr,size,QHostAddress(_config.recv_ip),_config.recv_port);
// }
// bool Action_UDP24L01::send(const char* ptr,const size_t size){
//     _sendSocket.writeDatagram(ptr,size,QHostAddress(_config.send_ip),_config.send_port);
// }
// void Action_UDP24L01::receiveData(){
//     static QByteArray datagram;
//     while (_recvSocket.hasPendingDatagrams()) {
//         datagram.resize(_recvSocket.pendingDatagramSize());
//         _recvSocket.readDatagram(datagram.data(), datagram.size());
//         if(_cb){
//             std::invoke(_cb,datagram.data(),datagram.size());
//         }
//     }
// }
// ActionModule::ActionModule(QObject *parent) : QObject(parent), team{-1, -1} {
// //    blue_sender_interface = blue_receiver_interface = yellow_sender_interface = yellow_receiver_interface = 0;
//     tx.resize(TRANSMIT_PACKET_SIZE);
//     tx[0] = 0x40;
// //    QObject::connect(&receiveSocket, SIGNAL(readyRead()), this, SLOT(readData()), Qt::DirectConnection);
//     bool newType;
//     for(int i=0;i<PARAM::ROBOTNUM;i++){
//         zpm->loadParam(newType,QString("RobotProtocol/%1New?").arg(i, 2, 10, QChar('0')),false);
//         _protocolType[i] = newType? ProtocolType::UDP_24L01 : ProtocolType::Serial_24L01;
//     }
//     if(receiveSocket.bind(QHostAddress::AnyIPv4, PORT_RECEIVE, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
//         qDebug() << "****** start receive ! ******";
// //        receiveThread = new std::thread([ = ] {readData();});
// //        receiveThread->detach();
//     } else {
//         qDebug() << "Bind Error in action module !";
//     }
// }

// ActionModule::~ActionModule() {
//     sendSocket.disconnectFromHost();
//     receiveSocket.disconnectFromHost();
// }

// bool ActionModule::connectRadio(int id, int frq) {
//     bool color;
//     if(id >= 0 && id < PARAM::TEAMS) {
//         zpm->loadParam(color, "ZAlert/IsYellow", false);
//         team[id] = color ? PARAM::YELLOW : PARAM::BLUE;
//         qDebug() << "connectRadio : " << id << (color ? "YELLOW" : "BLUE") << frq;
//         sendStartPacket(id, frq);
//         return true;
//     } else
//         qDebug() << "ERROR id in connectRadio function!";
//     return false;
// }

// bool ActionModule::disconnectRadio(int id) {
//     if(id >= 0 && id < PARAM::TEAMS) {
//         team[id] = -1;
//         sendSocket.disconnectFromHost();
//         return true;
//     } else
//         qDebug() << "ERROR id in disconnectRadio function!";
//     return false;
// }
// void ActionModule::setSimulation(bool simulation){
//     IS_SIMULATION = simulation;
// }
// void ActionModule::sendStartPacket(int t, int frequency) {
//     // this 't' is id
//     QByteArray startPacketSend(TRANSMIT_START_PACKET_SIZE, 0);
//     QByteArray startPacketReceive(TRANSMIT_START_PACKET_SIZE, 0);
//     int freq_encode = 0;
//     if(frequency < 8){
//         freq_encode = frequency*4;
//     }else{
//         freq_encode = frequency*4+58;
//     }
//     startPacketSend[0] = (char)0xf0;
//     startPacketSend[1] = (char)(freq_encode & 0xff);
//     startPacketSend[2] = (char)(freq_encode & 0xff);
//     startPacketSend[3] = (char)0x01;
//     startPacketSend[4] = (char)0x02;
//     int temp_value = 0;
//     for(int i=0;i<TRANSMIT_START_PACKET_SIZE-1;i++){
//         temp_value += startPacketSend[i];
//     }
//     startPacketSend[TRANSMIT_START_PACKET_SIZE-1] = (char)(temp_value & 0xff);

//     startPacketReceive[0] = (char)0xf0;
//     startPacketReceive[1] = (char)(freq_encode & 0xff);
//     startPacketReceive[2] = (char)(freq_encode & 0xff);
//     startPacketReceive[3] = (char)0x02;
//     startPacketReceive[4] = (char)0x02;
//     temp_value = 0;
//     for(int i=0;i<TRANSMIT_START_PACKET_SIZE-1;i++){
//         temp_value += startPacketReceive[i];
//     }
//     startPacketReceive[TRANSMIT_START_PACKET_SIZE-1] = (char)(temp_value & 0xff);
//     sendSocket.writeDatagram(startPacketSend, TRANSMIT_START_PACKET_SIZE, QHostAddress(radioSendAddress[t]), PORT_SEND);
//     receiveSocket.writeDatagram(startPacketReceive, TRANSMIT_START_PACKET_SIZE, QHostAddress(radioReceiveAddress[t]), PORT_SEND);
//     std::this_thread::sleep_for(std::chrono::milliseconds(4));
//     qDebug() << "Frequency:" << frequency << " Send IP:" << radioSendAddress[t] << " Receive IP:" << radioReceiveAddress[t];
// }

// void ActionModule::sendLegacy(int t, const ZSS::Protocol::Robots_Command& commands) {
//     // this 't' is color
//     auto& socket = sendSocket;
//     int id = -1;
//     if(t == team[0])
//         id = 0;
//     else if(t == team[1])
//         id = 1;
//     else
//         return;
//     int size = commands.command_size();
//     int count = 0;
//     tx.fill(0x00);
//     tx[0] = 0x40;
//     for(int i = 0; i < size; i++) {
//         if(count == 4) {
//             socket.writeDatagram(tx.data(), TRANSMIT_PACKET_SIZE, QHostAddress(radioSendAddress[id]), PORT_SEND);
//             std::this_thread::sleep_for(std::chrono::microseconds(3000));
//             tx.fill(0x00);
//             tx[0] = 0x40;
//             count = 0;
//         }

//         auto& command = commands.command(i);
//         if(_protocolType[command.robot_id()] == ProtocolType::Serial_24L01){
//             continue;
//         }
//         encodeLegacy(command, this->tx, count++);
//     }
//     //qDebug() << "sendLegacy : " << (t ? "Yellow" : "Blue") << id << "size:" << size;
//     socket.writeDatagram(tx.data(), TRANSMIT_PACKET_SIZE, QHostAddress(radioSendAddress[id]), PORT_SEND);
// }

// void ActionModule::readData() {
//     static QHostAddress address;
//     static int color;
//     static int count[PARAM::TEAMS][PARAM::ROBOTNUM];
//     for (int color = PARAM::BLUE; color < PARAM::TEAMS; color++) {
//         for (int j = 0; j < PARAM::ROBOTNUM; j++ ) {
//             count[color][j] = 0;
//         }
//     }
//     while(true) {
//         std::this_thread::sleep_for(std::chrono::microseconds(500));
// //        ZSS::ZParamManager::instance()->loadParam(isSimulation, "Alert/IsSimulation", false);
//         if(!IS_SIMULATION) {
//             for (int color = PARAM::BLUE; color < PARAM::TEAMS; color++) {
//                 for (int j = 0; j < PARAM::ROBOTNUM; j++ ) {
//                     if (count[color][j]++ > 1000) {
//                         GlobalData::instance()->robotInfoMutex.lock();
//                         GlobalData::instance()->robotInformation[color][j].infrared = false;
//                         GlobalData::instance()->robotInformation[color][j].flat = false;
//                         GlobalData::instance()->robotInformation[color][j].chip = false;
//                         count[color][j] = 0;
// //                        qDebug() << "FUCK" << color << j;
//                         GlobalData::instance()->robotInfoMutex.unlock();
//                         emit receiveRobotInfo(color, j);
//                     }
//                 }
//             }
//         }
//         while (receiveSocket.state() == QUdpSocket::BoundState && receiveSocket.hasPendingDatagrams()) {
//             qDebug() << "receive data !!!";
//             auto msgInfo = (MessageInfo*)(MessageInfo::instance());
//             char newInfo = msgInfo->info() | 0x02;
//             //        msgInfo->setInfo(newInfo);
//             rx.resize(receiveSocket.pendingDatagramSize());
//             receiveSocket.readDatagram(rx.data(), rx.size(), &address);
//             color = (address.toString() == radioReceiveAddress[0]) ? team[0] : team[1];
//             if (color == -1) {
//                 qDebug() << "Receive Error Message from:" << address << "in actionmodule.cpp";
//                 break;
//             }
//             auto& data = rx;
//             int id = 0;
//             bool infrared = false;
//             bool flat = false;
//             bool chip = false;
//             int battery = 0;
//             int capacitance = 0;
//             short wheelVel[4] = {0};

//             if(data[0] == (char)0xff && data[1] == (char)0x02) {
//                 id       = (quint8)data[2] - 1;//real robot 1-12 -> 0-11
//                 infrared = (quint8)data[3] & 0x40;
//                 flat     = (quint8)data[3] & 0x20;
//                 chip     = (quint8)data[3] & 0x10;
//                 battery  = (quint8)data[4];
//                 capacitance = (quint8)data[5];
//                 wheelVel[0] = (quint16)(data[6] << 8) + data[7];
//                 wheelVel[1] = 1 + (short)~(data[8] << 8) + data[9];
//                 wheelVel[2] = 1 + (short)~(data[10] << 8) + data[11];
//                 wheelVel[3] = (quint16)(data[12] << 8) + data[13];

//                 GlobalData::instance()->robotInfoMutex.lock();
//                 count[color][id] = 0;
//                 GlobalData::instance()->robotInformation[color][id].infrared = infrared;
//                 GlobalData::instance()->robotInformation[color][id].flat = flat;
//                 GlobalData::instance()->robotInformation[color][id].chip = chip;
//                 GlobalData::instance()->robotInformation[color][id].battery = std::min(std::max((battery - LOW_BATTERY) / (FULL_BATTERY - LOW_BATTERY), 0.0), 1.0);
//                 GlobalData::instance()->robotInformation[color][id].capacitance = std::min(std::max((capacitance - LOW_CAPACITANCE) / (FULL_CAPACITANCE - LOW_CAPACITANCE), 0.0), 1.0);
//                 GlobalData::instance()->robotInfoMutex.unlock();
//                 emit receiveRobotInfo(color, id);
//             }
// //            qDebug() << rx.toHex();
//             qDebug() << color << id << infrared << flat << address;
//         }
//     }
// }
// void ActionModule::changeAddress(int team, int index){
//     radioSendAddress[team] = radioSendAddress2choose[index];
//     radioReceiveAddress[team] = radioReceiveAddress2choose[index];
// }
// QStringList ActionModule::getAllAddress(){
//     return radioSendAddress2choose;
// }
// QString ActionModule::getRealAddress(int index){
//     return radioSendAddress[index];
// }

ActionModuleSerialVersion::ActionModuleSerialVersion(QObject *parent) : QObject(parent) {
    bool newType;
    for(int i=0;i<PARAM::ROBOTNUM;i++){
        zpm->loadParam(newType,QString("RobotProtocol/%1New?").arg(i, 2, 10, QChar('0')),false);
        _protocolType[i] = newType? ProtocolType::UDP_24L01 : ProtocolType::Serial_24L01;
    }
    updatePortsList();
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    zpm->loadParam(frequency,"Alert/Frequency",8);
    tx.resize(SERIAL_TRANSMIT_PACKET_SIZE);
    rx.resize(SERIAL_TRANS_FEEDBACK_SIZE);
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
    QByteArray startPacket1(SERIAL_TRANSMIT_PACKET_SIZE,0);
    QByteArray startPacket2(SERIAL_TRANSMIT_PACKET_SIZE,0);
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
            if (command.has_direct_kick_no_calibration()){
                NJ_CMDS[id].direct_kick_no_calibration = command.direct_kick_no_calibration();
                NJ_CMDS[id].direct_kick_power = command.direct_kick_power();
            }
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
            encodeNJLegacy(NJ_CMDS[i],this->tx,count++);
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
    QByteArray pack = serial.readAll();
    if(pack.length() > 0 && int(pack[0]) == 0xff){
        rx = pack;
    }else{
        rx += pack;
    }
    if(rx.length() < 25){
        return;
    }
    qDebug() << "recv : 0x" << rx;
    auto& data = rx;
    int id = 0;
    bool infrared = false;
    bool flat = false;
    bool chip = false;
    int battery = 0;
    int capacitance = 0;
    if(data[0] == (char)0xff){
        if(data[1] == (char)0x02){
            id       = (quint8)data[2] + (int(data[23]) == 0?0:-1); // if old protocol data[23] should not be zero, 23 in new protocol is zero
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
    rx = "";
}

namespace {
// void encodeLegacy(const ZSS::Protocol::Robot_Command& command, QByteArray& tx, int num) {
//     // send back to vision module
//     // num 0 ~ 3
//     // id  0 ~ 15
//     quint8 id = (quint8)command.robot_id();
//     double origin_vx = command.velocity_x();
//     double origin_vy = command.velocity_y();
//     double origin_vr = command.velocity_r();
//     double dt = 1. / Athena::FRAME_RATE;
//     double theta = - origin_vr * dt;
//     CVector v(origin_vx, origin_vy);
//     v = v.rotate(theta);
//     if (fabs(theta) > 0.00001) {
//         //            if (i==0) cout << theta << " " <<vx << " "<< vy << " ";
//         v = v * theta / (2 * sin(theta / 2));
//         origin_vx = v.x();
//         origin_vy = v.y();
//         //            if (i==0) cout << vx << " "<< vy << " " << endl;
//     }


//     qint16 vx = (qint16)(origin_vx/10.0);
//     qint16 vy = (qint16)(origin_vy/10.0);
//     qint16 vr = (qint16)(origin_vr * 160);
//     qint16 abs_vx = std::abs(vx);
//     qint16 abs_vy = std::abs(vy);
//     qint16 abs_vr = std::abs(vr);
//     //if(abs(vx) > 0.1) qDebug() << "id: " << id<< "  "<<num*4 + 1<< "  "<<vx;
//     // flat&chip m/s -> cm/s
//     // kick   1 : chip   0 : flat`
//     bool kick = command.kick();
//     quint16 speed = command.power();
//     quint8 power = 0;
//     if(speed > 0.5) {
//         power = kickStandardization(id, kick, (quint16)(command.power()));
//     }
// //    qDebug() << "id: " << id << "power: " << power << "speed: " << command.power();
//     // dribble -1 ~ +1 -> -3 ~ +3
//     qint8 dribble = command.dribbler_spin() > 0.5 ? 3 : 0;
//     tx[0] = (tx[0]) | (1 << (3 - num));
//     tx[num * 4 + 1] = ((quint8)kick << 6) | dribble << 4 | id;
//     tx[num * 4 + 2] = (vx >> 8 & 0x80) | (abs_vx & 0x7f);
//     tx[num * 4 + 3] = (vy >> 8 & 0x80) | (abs_vy & 0x7f);
//     tx[num * 4 + 4] = (vr >> 8 & 0x80) | (abs_vr & 0x7f);
//     tx[num  + 17] = (abs_vx >> 1 & 0xc0) | (abs_vy >> 3 & 0x30) | (abs_vr >> 7 & 0x0f);
//     tx[num  + 21] = power;
// }

// quint8 kickStandardization(quint8 id, bool mode, quint16 power) {
//     //if(power > 1) qDebug() << "id: "<<id<<" power: "<<power;
//     double new_power = 0;
//     double a, b, c;
//     double min_power, max_power;
//     QString key = "";

//     float ratio = mode ? 1 : 0.00001;
//     key = QString("Robot%1/%2_A").arg(id).arg(mode ? "CHIP" : "FLAT");
//     KParamManager::instance()->loadParam(a, key, 0);
//     key = QString("Robot%1/%2_B").arg(id).arg(mode ? "CHIP" : "FLAT");
//     KParamManager::instance()->loadParam(b, key, 1);
//     key = QString("Robot%1/%2_C").arg(id).arg(mode ? "CHIP" : "FLAT");
//     KParamManager::instance()->loadParam(c, key, 0);
//     key = QString("Robot%1/%2_MIN").arg(id).arg(mode ? "CHIP" : "FLAT");
//     KParamManager::instance()->loadParam(min_power, key, 15);
//     key = QString("Robot%1/%2_MAX").arg(id).arg(mode ? "CHIP" : "FLAT");
//     KParamManager::instance()->loadParam(max_power, key, 70);
//     new_power = (int)( ratio * a * power * power + b * power + c);
//     new_power = std::max(min_power, std::min(new_power, max_power));
//     new_power = std::max(10.0, std::min(new_power, 127.0));
//     return (quint8)new_power;
// }
double velRegulation(const int num,const double v){
    QString key = QString("Robot%1/vel").arg(num);
    double ratio = 1.0;
    KParamManager::instance()->loadParam(ratio, key, 1.0);
    return v*ratio;
}
void encodeNJLegacy(const NJ_Command& command,QByteArray& tx,int num){

    auto& TXBuff = tx;
    int i = num;
    int real_num = command.id;
    qint16 vx = (qint16)(command.vx);
    qint16 vy = (qint16)(command.vy);
    qint16 ivr = (qint16)(command.vr);
    qint16 vr = abs(ivr)> 511 ? (ivr > 0 ? 1 : -1)*(511) : (ivr);

    vx = velRegulation(real_num,vx);
    vy = velRegulation(real_num,vy);
    vr = velRegulation(real_num,vr);

    bool kick_mode = command.kick_mode;
    qint16 dribble = (qint16)(command.dribble*3+0.4);
    if(dribble > 3) dribble = 3;
    if(dribble < 0) dribble = 0;
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
    quint8 p = 0;
    if (command.direct_kick_no_calibration){
        p = command.direct_kick_power;
    }else{
        p = command.power > 0.01 ? linearKickMapping(real_num,kick_mode,command.power) : 0;
    }
    TXBuff[6*i + 8] = p & 0x0ff;

}
// id - 0 ~ 15
// kick_mode - 1 : chip 0 : flat
// power FLAT : cm/s
// power CHIP : cm of the first 
quint8 linearKickMapping(quint8 id,bool mode,float power){
    if(power < 0.5)
        return 0;

    double mapped_power = 0;
    double B, C;
    double MIN_POWER, MAX_POWER;
    QString MODE = mode ? "CHIP" : "FLAT";
    QString key = "";
    key = QString("Robot%1/%2_B").arg(id).arg(MODE);
    KParamManager::instance()->loadParam(B, key, 1);
    key = QString("Robot%1/%2_C").arg(id).arg(MODE);
    KParamManager::instance()->loadParam(C, key, 0);
    key = QString("Robot%1/%2_MIN").arg(id).arg(MODE);
    KParamManager::instance()->loadParam(MIN_POWER, key, 5);
    key = QString("Robot%1/%2_MAX").arg(id).arg(MODE);
    KParamManager::instance()->loadParam(MAX_POWER, key, 126);
    mapped_power = std::max(MIN_POWER, std::min(B * power + C, MAX_POWER));

    qDebug() << "mapping kp : " << QString::fromStdString(fmt::format("ID:{},MODE:{},POWER:{:.0f},MAPPED:{}",id,MODE.toStdString(),power,mapped_power));

    return (quint8)std::max(0.0, std::min(mapped_power, 127.0));

}
} // namespace ZSS::anonymous
}
