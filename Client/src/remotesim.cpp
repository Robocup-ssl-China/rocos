#include <thread>
#include "remotesim.h"
#include "globaldata.h"
#include "communicator.h"
#include "staticparams.h"
#include "grSim_Packet.pb.h"
#include "grSim_Commands.pb.h"
#include "networkinterfaces.h"

namespace ZSS {

grSim_Packet grsim_packet_yellow;
grSim_Commands *grsim_commands_yellow;
grSim_Robot_Command *grsim_robots_yellow[PARAM::ROBOTNUM];

grSim_Packet grsim_packet_blue;
grSim_Commands *grsim_commands_blue;
grSim_Robot_Command *grsim_robots_blue[PARAM::ROBOTNUM];

std::thread* blueReceiveThread = nullptr;
std::thread* yellowReceiveThread = nullptr;

bool trans_dribble(double dribble) {
    return dribble>1;
}

double trans_length(double v) {
    return v / 1000.0;
}

double trans_vr(double v) {
    return v;/// 40.0; from angel to 1/40 rad
}

RemoteSim::RemoteSim(QObject *parent) : QObject(parent)
{
    grsim_commands_yellow = grsim_packet_yellow.mutable_commands();
    grsim_commands_blue = grsim_packet_blue.mutable_commands();
    for (int i = 0; i < PARAM::ROBOTNUM; i++) {
        grsim_robots_yellow[i] = grsim_commands_yellow->add_robot_commands();
        grsim_robots_blue[i] = grsim_commands_blue->add_robot_commands();
    }
    for (int i = 0; i < PARAM::ROBOTNUM; i++) {
        grsim_robots_yellow[i]->set_id(i);
        grsim_robots_yellow[i]->set_kickspeedx(0);
        grsim_robots_yellow[i]->set_kickspeedz(0);
        grsim_robots_yellow[i]->set_velnormal(0);
        grsim_robots_yellow[i]->set_veltangent(0);
        grsim_robots_yellow[i]->set_velangular(0);
        grsim_robots_yellow[i]->set_spinner(false);
        grsim_robots_yellow[i]->set_wheelsspeed(false);
    }
    for (int i = 0; i < PARAM::ROBOTNUM; i++) {
        grsim_robots_blue[i]->set_id(i);
        grsim_robots_blue[i]->set_kickspeedx(0);
        grsim_robots_blue[i]->set_kickspeedz(0);
        grsim_robots_blue[i]->set_velnormal(0);
        grsim_robots_blue[i]->set_veltangent(0);
        grsim_robots_blue[i]->set_velangular(0);
        grsim_robots_blue[i]->set_spinner(false);
        grsim_robots_blue[i]->set_wheelsspeed(false);
    }
    for(int i = 0; i < PARAM::TEAMS; i++) {
        if(connectSim(i)){
            switch (i) {
            case 0:
                blueReceiveThread = new std::thread([=] {readBlueData();});
                blueReceiveThread->detach();
                break;
            case 1:
                yellowReceiveThread = new std::thread([=] {readYellowData();});
                yellowReceiveThread->detach();
                break;
            }
        }
    }
    int index = ZCommunicator::instance()->getGrsimInterfaceIndex();
    RemoteAddress = ZNetworkInterfaces::instance()->getGrsimInterfaces()[index];
}

bool RemoteSim::disconnectSim(bool color) {
    if(color) {
        yellowReceiveSocket.disconnectFromHost();
    } else {
        blueReceiveSocket.disconnectFromHost();
    }
    return true;
}

RemoteSim::~RemoteSim() {
    yellowReceiveSocket.abort();
    blueReceiveSocket.abort();
}

bool RemoteSim::connectSim(bool color) {
    if(color) {
        if(yellowReceiveSocket.bind(QHostAddress::AnyIPv4, ZSS::Sim::YELLOW_STATUS_PORT, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
            qDebug() << "Yellow connect successfully!!!";
            return true;
        }
        return false;
    }
    if(blueReceiveSocket.bind(QHostAddress::AnyIPv4, ZSS::Sim::BLUE_STATUS_PORT, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        qDebug() << "Blue connect successfully!!!";
        return true;
    }
    return false;
}

void RemoteSim::readBlueData() {
    qDebug() << "Reading Blue Data!";
    QByteArray datagram;
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
        while(blueReceiveSocket.state() == QUdpSocket::BoundState && blueReceiveSocket.hasPendingDatagrams()) {
            ZSS::Protocol::Robots_Status robotsPacket;
            datagram.resize(blueReceiveSocket.pendingDatagramSize());
            blueReceiveSocket.readDatagram(datagram.data(), datagram.size());
            robotsPacket.ParseFromArray(datagram, datagram.size());

            for (int i = 0; i < robotsPacket.robots_status_size(); ++i) {
                int id = robotsPacket.robots_status(i).robot_id();
                bool infrared = robotsPacket.robots_status(i).infrared();
                bool isFlatKick = robotsPacket.robots_status(i).flat_kick();
                bool isChipKick = robotsPacket.robots_status(i).chip_kick();
                robotInfoMutex.lock();
                GlobalData::instance()->robotInformation[PARAM::BLUE][id].infrared = infrared;
                GlobalData::instance()->robotInformation[PARAM::BLUE][id].flat = isFlatKick;
                GlobalData::instance()->robotInformation[PARAM::BLUE][id].chip = isChipKick;
                robotInfoMutex.unlock();
                ZCommunicator::instance()->sendCommand(PARAM::BLUE, id);
//                emit receiveRemoteInfo(PARAM::BLUE, id);
            }
        }
    }
}

void RemoteSim::readYellowData() {
    qDebug() << "Reading Yellow Data!";
    QByteArray datagram;
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
        while(yellowReceiveSocket.state() == QUdpSocket::BoundState && yellowReceiveSocket.hasPendingDatagrams()) {
            ZSS::Protocol::Robots_Status robotsPacket;
            datagram.resize(yellowReceiveSocket.pendingDatagramSize());
            yellowReceiveSocket.readDatagram(datagram.data(), datagram.size());
            robotsPacket.ParseFromArray(datagram, datagram.size());

            for (int i = 0; i < robotsPacket.robots_status_size(); ++i) {
                int id = robotsPacket.robots_status(i).robot_id();
                bool infrared = robotsPacket.robots_status(i).infrared();
                bool isFlatKick = robotsPacket.robots_status(i).flat_kick();
                bool isChipKick = robotsPacket.robots_status(i).chip_kick();
                robotInfoMutex.lock();
                GlobalData::instance()->robotInformation[PARAM::YELLOW][id].infrared = infrared;
                GlobalData::instance()->robotInformation[PARAM::YELLOW][id].flat = isFlatKick;
                GlobalData::instance()->robotInformation[PARAM::YELLOW][id].chip = isChipKick;
                robotInfoMutex.unlock();
                ZCommunicator::instance()->sendCommand(PARAM::YELLOW, id);
//                emit receiveRemoteInfo(PARAM::YELLOW, id);
            }
        }
    }
}

void RemoteSim::sendSim(int t, ZSS::Protocol::Robots_Command& command) {
    if(t != 0 && t != 1) {
        qDebug() << "Team ERROR in Simmodule !";
        return;
    }
    grSim_Packet& grsim_packet = (t == 0 ? grsim_packet_blue : grsim_packet_yellow);
    grSim_Commands * grsim_commands = (t == 0 ? grsim_commands_blue : grsim_commands_yellow);
    grSim_Robot_Command **grsim_robots = (t == 0 ? grsim_robots_blue : grsim_robots_yellow);
    grsim_commands->set_isteamyellow(t == 0 ? false : true);
    grsim_commands->set_timestamp(0);
    int command_size = command.command_size();
    for (int i = 0; i < command_size; i++) {
        auto commands = command.command(i);
        auto id = commands.robot_id();
        grsim_robots[id]->set_id(id);
        grsim_robots[id]->set_wheelsspeed(false);
        //set flatkick or chipkick
        if (!commands.kick()) {
            grsim_robots[id]->set_kickspeedz(0);
            grsim_robots[id]->set_kickspeedx(trans_length(commands.power()));
        } else {
            double radian = ZSS::Sim::CHIP_ANGLE * ZSS::Sim::PI / 180.0;
            double vx = sqrt(trans_length(commands.power()) * ZSS::Sim::G / 2.0 / tan(radian));
            double vz = vx * tan(radian);
            grsim_robots[id]->set_kickspeedz(vx);
            grsim_robots[id]->set_kickspeedx(vz);
        }

        //set velocity and dribble
        double vx = commands.velocity_x();
        double vy = commands.velocity_y();
        double vr = commands.velocity_r();
        grsim_robots[id]->set_veltangent(trans_length(vx));
        grsim_robots[id]->set_velnormal(trans_length(vy));
        grsim_robots[id]->set_velangular(trans_vr(vr));
        grsim_robots[id]->set_spinner(trans_dribble(commands.dribbler_spin()));
    }
    int size = grsim_packet.ByteSizeLong();
    QByteArray data(size, 0);
    grsim_packet.SerializeToArray(data.data(), data.size());
    command_socket.writeDatagram(data, size, QHostAddress(RemoteAddress), ZSS::Athena::SIM_SEND);
//    qDebug() << RemoteAddress << ZSS::Athena::SIM_SEND;
    for (int i = 0; i < PARAM::ROBOTNUM; i++) {
        grsim_robots[i]->set_id(i);
        grsim_robots[i]->set_kickspeedx(0);
        grsim_robots[i]->set_kickspeedz(0);
        grsim_robots[i]->set_velnormal(0);
        grsim_robots[i]->set_veltangent(0);
        grsim_robots[i]->set_velangular(0);
        grsim_robots[i]->set_spinner(false);
        grsim_robots[i]->set_wheelsspeed(false);
    }
}

}


