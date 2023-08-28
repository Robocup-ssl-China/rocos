#include "udpwifiobject.h"

namespace  {
    int BLUE = 0;
    int YELLOW = 1;
}

udpwifiobject::udpwifiobject(QObject *parent):QObject(parent)
{
    _controlId = 0;
    num = 0;
    updateInterfaces();
    resetWifiCommInterface(0);
}

void udpwifiobject::updateInterfaces(){
    this->interfaces.clear();
    for(auto interface : QNetworkInterface::allInterfaces()){
        interfaces.append(interface.humanReadableName());
    }
}

void udpwifiobject::resetWifiCommInterface(int _interface_index){
    if(_n_last_interface == _interface_index){
        return;
    }
    _n_last_interface = _interface_index;
    _n_mcPort = 13134;
    _n_mcAddr = "225.225.225.225";
    _n_recvPort = 14134;
    _n_sendPort = 14234;
    if(_n_binded){
        disconnect(&_n_recvMcSocket);
        disconnect(&_n_recvSocket);
        _n_recvMcSocket.abort();
        _n_recvSocket.abort();
        for(int i=0;i<100;i++){
            _n_address[i] = "";
        }
    }
    auto res = _n_recvMcSocket.bind(QHostAddress::AnyIPv4,_n_mcPort,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    res = res && _n_recvMcSocket.joinMulticastGroup(QHostAddress(_n_mcAddr),getFromIndex(_interface_index));
    connect(&_n_recvMcSocket, SIGNAL(readyRead()), this, SLOT(n_mcRecvData()), Qt::DirectConnection);
    if(res){
        qDebug() << "****** UDP_WIFI  Multicast start receive ! - " << getFromIndex(_interface_index).humanReadableName() << "******";
    }else{
        qDebug() << "****** UDP_WIFI  Multicast failed !!! - " << getFromIndex(_interface_index).humanReadableName() << "******";
    }
    res = res && _n_recvSocket.bind(QHostAddress::AnyIPv4,_n_recvPort,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(&_n_recvSocket, SIGNAL(readyRead()), this, SLOT(n_recvData()), Qt::DirectConnection);
    if(res){
        qDebug() <<"****** UDP_WIFI NormalRecv start receive ! ******";
    }else{
        qDebug() <<"****** UDP_WIFI NormalRecv failed !!! ******";
    }
    _n_binded = true;
}

void udpwifiobject::n_mcRecvData(){
    static QByteArray datagram;
    ZSS::New::Multicast_Status mc_status;
    while(_n_recvMcSocket.hasPendingDatagrams()){
        QHostAddress source;
        datagram.resize(_n_recvMcSocket.pendingDatagramSize());
        _n_recvMcSocket.readDatagram(datagram.data(), datagram.size(), &source);
        auto res = mc_status.ParseFromArray(datagram.data(),datagram.size());
        if(!res){
            continue;
        }
        //qDebug() << QString::fromStdString(mc_status.DebugString());
        int id = mc_status.robot_id();
        int team = -1;
        if(mc_status.team() == ZSS::New::Team::BLUE){
            team = BLUE;
        }
        else if(mc_status.team() == ZSS::New::Team::YELLOW){
            team = YELLOW;
        }else{
            continue;
        }
        if(team >= 0){
            const auto ip = source.toString();//QString::fromStdString(mc_status.ip());
            if (ip != QString::fromStdString(mc_status.ip())){
                qDebug() << "return ip in protobuf not equal to real ip : " << ip << QString::fromStdString(mc_status.ip()) << team << id;
            }
            if(id <0 || id >= 16){
                qDebug() << "wifi multicast msg [id]("<< id <<") not correct" << team << ip;
                continue;
            }
            if(team == YELLOW){
                _n_address[id+16] = ip;
            }else{
                _n_address[id] = ip;
            }
            bool flag = true;
            for(int i = 0;i < num; i++){
                if(currentinterfaces[i] == ip) flag = false;
            }
            if(flag){
                carid[num] = id;
                currentinterfaces.append(ip);
                num++;
            }
            if(num >= 32) num = 31;
        }
    }
}

void udpwifiobject::n_recvData(){
    static QByteArray datagram;
    ZSS::New::Robot_Status pb_status;
    while(_n_recvSocket.hasPendingDatagrams()){
        QHostAddress source;
        datagram.resize(_n_recvSocket.pendingDatagramSize());
        _n_recvSocket.readDatagram(datagram.data(), datagram.size(), &source);
        auto res = pb_status.ParseFromArray(datagram.data(),datagram.size());
        if(!res){
            continue;
        }
        int team = -1;
        if(pb_status.team() == ZSS::New::Team::BLUE){
            team = BLUE;
        }
        else if(pb_status.team() == ZSS::New::Team::YELLOW){
            team = YELLOW;
        }else{
            continue;
        }
        int id = pb_status.robot_id();
        if(source.toString() != _n_address[team][id]){
            //qDebug() << "!!!!!!!!!! got robot_status but ip not correct with multicast result";
            continue;
        }
////        qDebug() << QString::fromStdString(pb_status.DebugString());
//        QDateTime UTC(QDateTime::currentDateTimeUtc());
//        GlobalData::instance()->robotInfoMutex.lock();

//        // filter for infrared sensor , should remove if rpi version fix the bugs
//        if(pb_status.infrared() > 5){// 5ms
//            GlobalData::instance()->robotInformation[team][id].infrared = true;
//        }else if(pb_status.infrared() <= 0){
//            GlobalData::instance()->robotInformation[team][id].infrared = false;
//        }
// //       qDebug() <<"id:" << id << "infrared:" << GlobalData::instance()->robotInformation[team][id].infrared;
////        qDebug() << id << pb_status.infrared();
//        GlobalData::instance()->robotInformation[team][id].flat = pb_status.flat_kick()<=60&&pb_status.flat_kick()>=0; // 13ms*5
//        GlobalData::instance()->robotInformation[team][id].chip = pb_status.chip_kick()<=60&&pb_status.chip_kick()>=0;
//        last_receive_time[team][id] = UTC.toMSecsSinceEpoch();
//        GlobalData::instance()->robotInfoMutex.unlock();
//        emit receiveRobotInfo(team, id);
    }
}

void udpwifiobject::sendCommand(){
    ZSS::New::Robot_Command cmd;
    cmd.set_robot_id(this->_controlId);
    qDebug() << this->_controlIP;
    qDebug() << this->_controlId;
    cmd.set_cmd_type(ZSS::New::Robot_Command_CmdType::Robot_Command_CmdType_CMD_VEL);
    auto cmd_vel = cmd.mutable_cmd_vel();
    cmd_vel->set_velocity_x(this->velX/150.0);
    cmd_vel->set_velocity_y(this->velY/150.0);
    cmd_vel->set_velocity_r(this->velR/25.0);
    cmd.set_kick_mode(this->shootMode ? ZSS::New::Robot_Command_KickMode::Robot_Command_KickMode_CHIP : ZSS::New::Robot_Command_KickMode::Robot_Command_KickMode_KICK);
    cmd.set_desire_power(this->shoot ? (this->shootPowerLevel) : 0);
    cmd.set_kick_discharge_time(this->shoot ? (this->shootPowerLevel * 50.0) : 0);
    cmd.set_dribble_spin(this->dribble ? (this->dribbleLevel > 0.5 ? 3 : 0) : 0);
    std::string data = cmd.SerializeAsString();
    _n_sendSocket.writeDatagram(data.c_str(), data.size(), QHostAddress(_controlIP), _n_sendPort);
}
