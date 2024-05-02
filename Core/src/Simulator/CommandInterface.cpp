﻿#include "CommandInterface.h"
#include "ServerInterface.h"
#include "OptionModule.h"
#include <iostream>
#include "staticparams.h"
#include "game_state.h"
#include "parammanager.h"
#include "staticparams.h"
#include <cmath>
#include <QUdpSocket>
#include "RobotSensor.h"
#include "VisionModule.h"
CCommandInterface* CCommandInterface::_instance = 0;

CCommandInterface::CCommandInterface(const COptionModule *pOption, QObject *parent)
    : pOption(pOption), QObject(parent),
      receiveSocket(nullptr), command_socket(nullptr) {
    bool isYellow = false, isRight = false;
    ZSS::ZParamManager::instance()->loadParam(SIM_PORT, "AlertPorts/SimPort", 20011);
    ZSS::ZParamManager::instance()->loadParam(SELF_PORT, "Ports/SimSelfPort", 30015);
    ZSS::ZParamManager::instance()->loadParam(CHIP_ANGLE, "Simulator/ChipAngle", 45);
    ZSS::ZParamManager::instance()->loadParam(isYellow, "ZAlert/IsYellow", false);
    ZSS::ZParamManager::instance()->loadParam(isRight,"ZAlert/IsRight",false);

    ZSS::ZParamManager::instance()->loadParam(DEBUG_CMD,"Debug/DeviceCmd",false);
    TEAM = isYellow ? PARAM::YELLOW : PARAM::BLUE;
    SIDE = isRight ? PARAM::Field::POS_SIDE_RIGHT : PARAM::Field::POS_SIDE_LEFT;
    command_socket = new QUdpSocket();
    receiveSocket = new QUdpSocket();
    receiveSocket->bind(QHostAddress::AnyIPv4, ZSS::Athena::CONTROL_BACK_RECEIVE[TEAM], QUdpSocket::ShareAddress);
    _thread = new std::thread([&] {receiveInformation();});
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
void CCommandInterface::setKick(int num, double kp, double cp, bool direct_kick_no_calibration, double direct_kick_power) {
    int number = num;
    if (number < 0 || number > PARAM::Field::MAX_PLAYER - 1) {
        //std::cout << "Robot Number Error in Simulator setKick" << std::endl;
        return;
    }
    commands[number].flat_kick = kp;
    commands[number].chip_kick = cp;
    commands[number].direct_kick_no_calibration = direct_kick_no_calibration;
    commands[number].direct_kick_power = direct_kick_power;
}

void CCommandInterface::setNeedReport(int num, bool needReport){
    int number = num;
    if (number < 0 || number > PARAM::Field::MAX_PLAYER - 1) {
        //std::cout << "Robot Number Error in Simulator setKick" << std::endl;
        return;
    }
    commands[number].need_report = needReport;
}
void CCommandInterface::placeRobot(int num, double x, double y, double dir){
    std::scoped_lock lock(__placement_mutex);
    if (num < 0 || num > PARAM::Field::MAX_PLAYER - 1) {
        return;
    }
    auto robot = __robots_command.mutable_replacement()->add_robots();
    robot->set_x(x*SIDE);
    robot->set_y(y*SIDE);
    robot->set_dir(dir+(SIDE==PARAM::Field::POS_SIDE_LEFT?0:PARAM::Math::PI));
    robot->set_id(num);
    robot->set_yellowteam(TEAM == PARAM::YELLOW);
}
void CCommandInterface::placeBall(double x, double y, double vx, double vy){
    std::scoped_lock lock(__placement_mutex);
    auto ball = __robots_command.mutable_replacement()->mutable_ball();
    ball->set_x(x*SIDE);
    ball->set_y(y*SIDE);
    ball->set_vx(vx*SIDE);
    ball->set_vy(vy*SIDE);
}
void CCommandInterface::sendCommands() {
    std::scoped_lock lock(__placement_mutex);
    for (int i = 0; i < PARAM::Field::MAX_PLAYER; i++) {
        if(!VisionModule::Instance()->ourPlayer(i).Valid()){
            continue;
        }
        auto robot_command = __robots_command.add_command();
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
    if(DEBUG_CMD){
        static const int DATA_SIZE = 7;
        static const std::array<std::string, DATA_SIZE> titles{"BotID","Vel_X","Vel_Y","Vel_R","Dribb","KickM","Power"};
        static const int FONT_SIZE = 50;
        static const double DEBUG_TEXT_LENGTH = FONT_SIZE*3;
        static const double DEBUG_X = PARAM::Field::PITCH_LENGTH / 2 - 3 * FONT_SIZE * (PARAM::Field::MAX_PLAYER) - DEBUG_TEXT_LENGTH;
        static const double DEBUG_X_STEP = PARAM::Field::PITCH_LENGTH / 30;
        static const double DEBUG_Y = PARAM::Field::PITCH_WIDTH / 2 - 5*FONT_SIZE;
        static const double DEBUG_Y_STEP = -FONT_SIZE*2;
        std::string title_txt = "";
        for(auto title_str : titles){
            title_txt += fmt::format(" {:>6s}",title_str);
        }
        GDebugEngine::Instance()->gui_debug_msg_fix(CGeoPoint(DEBUG_X, DEBUG_Y),title_txt,COLOR_PURPLE,0,FONT_SIZE);
        for(int i=0;i<__robots_command.command_size();i++){
            auto&& cmd = __robots_command.command(i);
            std::array<std::string,DATA_SIZE> info_str{
                fmt::format("{:>7d}",cmd.robot_id()),
                fmt::format("{:>7.0f}",cmd.velocity_x()),
                fmt::format("{:>7.0f}",cmd.velocity_y()),
                fmt::format("{:>7.2f}",cmd.velocity_r()),
                fmt::format("{:>7.2f}",cmd.dribbler_spin()),
                fmt::format("{:>7s}",cmd.kick() ? "✓" : "✕"),
                fmt::format("{:>7.0f}",cmd.power())
            };
            std::array<bool,DATA_SIZE> info_check{1, std::abs(cmd.velocity_x())>1, std::abs(cmd.velocity_y())>1, std::abs(cmd.velocity_r())>0.01, cmd.dribbler_spin()>0.1, cmd.kick(), cmd.power()>100};
            std::string msg,msg_invalid;
            for(int j=0;j<info_str.size();j++){
                msg += fmt::format("{:>7s}",info_check[j] ? info_str[j] : "");
                msg_invalid += fmt::format("{:>7s}",info_check[j] ? "" : "--");
            }
            GDebugEngine::Instance()->gui_debug_msg_fix(CGeoPoint(DEBUG_X, DEBUG_Y + DEBUG_Y_STEP * (i+1)),msg,COLOR_PURPLE,0,FONT_SIZE);
            GDebugEngine::Instance()->gui_debug_msg_fix(CGeoPoint(DEBUG_X, DEBUG_Y + DEBUG_Y_STEP * (i+1)),msg_invalid,COLOR_GRAY,0,FONT_SIZE);
        }
    }
    int size = __robots_command.ByteSizeLong();
    QByteArray data(size, 0);
    __robots_command.SerializeToArray(data.data(), size);
    command_socket->writeDatagram(data.data(), size, QHostAddress(ZSS::LOCAL_ADDRESS), ZSS::Athena::CONTROL_SEND[TEAM]);
    __robots_command.Clear();
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
