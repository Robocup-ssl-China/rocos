#include "CommandInterface.h"
#include "ServerInterface.h"
#include "OptionModule.h"
#include <iostream>
#include "grSim_Packet.pb.h"
#include "zss_cmd.pb.h"
#include "staticparams.h"
#include "game_state.h"
#include "parammanager.h"
#include "staticparams.h"
#include <cmath>
#include <QUdpSocket>
#include "RobotSensor.h"
#include "VisionModule.h"
#include <thread>
CCommandInterface* CCommandInterface::_instance = 0;
namespace {
int SIM_PORT = 0;
int SELF_PORT = 0;
int CHIP_ANGLE = 1;
int TEAM;
ZSS::Protocol::Robots_Command robots_command;
std::thread *_thread = nullptr;
}

CCommandInterface::CCommandInterface(const COptionModule *pOption, QObject *parent)
    : pOption(pOption), QObject(parent),
      receiveSocket(nullptr), command_socket(nullptr) {
    bool isYellow = false;
    ZSS::ZParamManager::instance()->loadParam(SIM_PORT, "AlertPorts/SimPort", 20011);
    ZSS::ZParamManager::instance()->loadParam(SELF_PORT, "Ports/SimSelfPort", 30015);
    ZSS::ZParamManager::instance()->loadParam(CHIP_ANGLE, "Simulator/ChipAngle", 45);
    ZSS::ZParamManager::instance()->loadParam(isYellow, "ZAlert/IsYellow", false);
    TEAM = isYellow ? PARAM::YELLOW : PARAM::BLUE;
    command_socket = new QUdpSocket();
    receiveSocket = new QUdpSocket();
    receiveSocket->bind(QHostAddress::AnyIPv4, ZSS::Athena::CONTROL_BACK_RECEIVE[TEAM], QUdpSocket::ShareAddress);
    _thread = new std::thread([ = ] {receiveInformation();});
    _thread->detach();
}

CCommandInterface::~CCommandInterface(void) {
    //delete _pServerIf;
}

void CCommandInterface::setSpeed(int num, double dribble, double vx, double vy, double vr) {
    int number = num;
    if (number < 0 || number > PARAM::Field::MAX_PLAYER - 1) {
        //std::cout << "Robot Number Error in Simulator setSpeed" << number<< std::endl;
        return;
    }
    commands[number].dribble_spin = dribble;
    commands[number].velocity_x = vx;
    commands[number].velocity_y = vy;
    commands[number].velocity_r = vr;

}
void CCommandInterface::setKick(int num, double kp, double cp) {
    int number = num;
    if (number < 0 || number > PARAM::Field::MAX_PLAYER - 1) {
        //std::cout << "Robot Number Error in Simulator setKick" << std::endl;
        return;
    }
    commands[number].flat_kick = kp;
    commands[number].chip_kick = cp;
}

void CCommandInterface::setNeedReport(int num, bool needReport){
    int number = num;
    if (number < 0 || number > PARAM::Field::MAX_PLAYER - 1) {
        //std::cout << "Robot Number Error in Simulator setKick" << std::endl;
        return;
    }
    commands[number].need_report = needReport;
}

void CCommandInterface::sendCommands() {
//    GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-400,-200),"COMMAND_VALID : ",COLOR_GRAY);
    for (int i = 0; i < PARAM::Field::MAX_PLAYER; i++) {
//        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-216+i*13,-200),VisionModule::Instance()->ourPlayer(i).Valid()?"1":"0",COLOR_GRAY);
        if(!VisionModule::Instance()->ourPlayer(i).Valid()){
            continue;
        }
        auto robot_command = robots_command.add_command();
        robot_command->set_robot_id(i);
        robot_command->set_velocity_x(commands[i].velocity_x);
        robot_command->set_velocity_y(commands[i].velocity_y);
        robot_command->set_velocity_r(commands[i].velocity_r);
        robot_command->set_dribbler_spin(commands[i].dribble_spin);
        robot_command->set_use_dir(commands[i].use_dir);
        robot_command->set_need_report(commands[i].need_report);
        if(commands[i].dribble_spin >=1){
            GDebugEngine::Instance()->gui_debug_arc(VisionModule::Instance()->ourPlayer(i).RawPos(),5,0,360,COLOR_BLACK);
        }
        if(commands[i].chip_kick < 0.001) { //flat kick
            robot_command->set_kick(false);
            robot_command->set_power(commands[i].flat_kick);
        } else {
            robot_command->set_kick(true);
            robot_command->set_power(commands[i].chip_kick);
        }
    }
    int size = ::robots_command.ByteSizeLong();
    QByteArray data(size, 0);
    ::robots_command.SerializeToArray(data.data(), size);
    command_socket->writeDatagram(data.data(), size, QHostAddress(ZSS::LOCAL_ADDRESS), ZSS::Athena::CONTROL_SEND[TEAM]);
    ::robots_command.Clear();
    memset(commands,0,sizeof(RobotCommand)*PARAM::Field::MAX_PLAYER);
}

void CCommandInterface::receiveInformation() {
    ZSS::Protocol::Robot_Status robot_status;
    QByteArray datagram;
    QHostAddress address;
    quint16 udp_port;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        while (receiveSocket->state() == QUdpSocket::BoundState && receiveSocket->hasPendingDatagrams()) {
            datagram.resize(receiveSocket->pendingDatagramSize());
            receiveSocket->readDatagram(datagram.data(), datagram.size(), &address, &udp_port);
            robot_status.ParseFromArray(datagram, datagram.size());
            auto&& id = robot_status.robot_id();
            if(!(id >= 0 && id < PARAM::Field::MAX_PLAYER)) {
                qDebug() << "ERROR received error robot id in command interface." << id;
                continue;
            }
            auto& msg = RobotSensor::Instance()->robotMsg[id];
            msg._mutex.lock();
            msg.infrared = robot_status.infrared();
            msg.chip_kick = robot_status.chip_kick() ? 5 : (msg.chip_kick);
            msg.flat_kick = robot_status.flat_kick() ? 5 : (msg.flat_kick);
            msg._mutex.unlock();
//            qDebug() << address << udp_port << id << msg.infrared
//                     << msg.chip_kick
//                     << msg.flat_kick;
        }
    }
}

CCommandInterface* CCommandInterface::instance(const COptionModule *pOption) {
    if(_instance == 0)
        _instance = new CCommandInterface(pOption);
    return _instance;
}

void CCommandInterface::destruct() {
    if(_instance)
        delete _instance;
}
