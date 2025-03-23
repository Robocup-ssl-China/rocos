#include "simmodule.h"
#include "parammanager.h"
#include "globaldata.h"
#include "zss_cmd.pb.h"
#include "grSim_Packet.pb.h"
#include "messageinfo.h"
#include "staticparams.h"
#include <chrono>
#include <thread>
#include <cmath>
#include <geometry.h>
#include <qdebug.h>
#include "sim/sslworld.h"
#include "visionmodule.h"
namespace ZSS {
namespace {
bool trans_dribble(double dribble) {
    return dribble>0.5;
}
double trans_length(double v) {
    return v / 1000.0;
}
double trans_vr(double v) {
    return v;/// 40.0; from angel to 1/40 rad
}
}
SimModule::SimModule(QObject *parent)
    : QObject(parent)
    , p_sim_packet("sim_packet")
    , s_blue_status("blue_status", [this](const zos::Data& data){getRobotStatus(data, PARAM::BLUE);})
    , s_yellow_status("yellow_status", [this](const zos::Data& data){getRobotStatus(data, PARAM::YELLOW);}) {
    SSLWorld::instance()->p_blue_status.link(&s_blue_status);
    SSLWorld::instance()->p_yellow_status.link(&s_yellow_status);
    p_sim_packet.link(&SSLWorld::instance()->s_sim_packet);
    resetSimCommand();
}

SimModule::~SimModule() {
}

bool SimModule::connectSim(bool color) {
    return false;
}

bool SimModule::disconnectSim(bool color) {
    return true;
}

void SimModule::getRobotStatus(const zos::Data& data, int TEAM){
    ZSS::Protocol::Robots_Status robotsPacket;
    robotsPacket.ParseFromArray(data.data(), data.size());
    for (int i = 0; i < robotsPacket.robots_status_size(); ++i) {
        int id = robotsPacket.robots_status(i).robot_id();
        bool infrared = robotsPacket.robots_status(i).infrared();
        bool isFlatKick = robotsPacket.robots_status(i).flat_kick();
        bool isChipKick = robotsPacket.robots_status(i).chip_kick();
        robotInfoMutex.lock();
        GlobalData::instance()->robotInformation[TEAM][id].infrared = infrared;
        GlobalData::instance()->robotInformation[TEAM][id].flat = isFlatKick;
        GlobalData::instance()->robotInformation[TEAM][id].chip = isChipKick;
        robotInfoMutex.unlock();
        qDebug() << "id: " << id << "  infrared: " << infrared << "  flat: " << isFlatKick << "  chip: " << isChipKick;
        emit receiveSimInfo(TEAM, id);
    }
}

void SimModule::sendSim(int t, ZSS::Protocol::Robots_Command& command) {
    static zos::Data data;
    if(!VisionModule::instance()->_running) return;
    std::scoped_lock lock(_sim_pack_mutex);
    assert (t>=PARAM::BLUE&&t<=PARAM::YELLOW);
    int command_size = command.command_size();
    for (int i = 0; i < command_size; i++) {
        auto commands = command.command(i);
        auto id = commands.robot_id();
        grsim_robots[t][id]->set_id(id);
        grsim_robots[t][id]->set_wheelsspeed(false);
        //set flatkick or chipk    ick
        if (!commands.kick()) {
            grsim_robots[t][id]->set_kickspeedz(0);
            grsim_robots[t][id]->set_kickspeedx(trans_length(commands.power()));
        } else {
            double radian = ZSS::Sim::CHIP_ANGLE * ZSS::Sim::PI / 180.0;
            double vx = sqrt(trans_length(commands.power()) * ZSS::Sim::G / 2.0 / tan(radian));
            double vz = vx * tan(radian);
            grsim_robots[t][id]->set_kickspeedz(vx);
            grsim_robots[t][id]->set_kickspeedx(vz);
        }
        //set velocity and dribble
        double vx = commands.velocity_x();
        double vy = commands.velocity_y();
        double vr = commands.velocity_r();
        double dt = 1. / Athena::FRAME_RATE;
        double theta = - vr * dt;
        CVector v(vx, vy);
        v = v.rotate(theta);
        if (fabs(theta) > 0.00001) {
            v = v * theta / (2 * sin(theta / 2));
            vx = v.x();
            vy = v.y();
        }

        grsim_robots[t][id]->set_veltangent(trans_length(vx));
        grsim_robots[t][id]->set_velnormal(trans_length(vy));
        grsim_robots[t][id]->set_velangular(trans_vr(vr));
        grsim_robots[t][id]->set_spinner(trans_dribble(commands.dribbler_spin()));
    }
    int size = grsim_packet[t].ByteSizeLong();
    data.resize(size);
    grsim_packet[t].SerializeToArray(data.ptr(), size);
    p_sim_packet.publish(data);
    for (int i = 0; i < PARAM::ROBOTNUM; i++) {
        grsim_robots[t][i]->set_id(i);
        grsim_robots[t][i]->set_kickspeedx(0);
        grsim_robots[t][i]->set_kickspeedz(0);
        grsim_robots[t][i]->set_velnormal(0);
        grsim_robots[t][i]->set_veltangent(0);
        grsim_robots[t][i]->set_velangular(0);
        grsim_robots[t][i]->set_spinner(false);
        grsim_robots[t][i]->set_wheelsspeed(false);
    }
}

void SimModule::resetSimCommand(){
    static zos::Data data;
    std::scoped_lock lock(_sim_pack_mutex);
    for (int i=0; i<PARAM::TEAMS; i++) {
        grsim_packet[i].Clear();
        grsim_commands[i] = grsim_packet[i].mutable_commands();
        grsim_commands[i]->set_timestamp(0);
        grsim_commands[i]->set_isteamyellow(i);
        for (int j = 0; j < PARAM::ROBOTNUM; j++) {
            grsim_robots[i][j] = grsim_commands[i]->add_robot_commands();
            grsim_robots[i][j]->set_id(i);
            grsim_robots[i][j]->set_kickspeedx(0);
            grsim_robots[i][j]->set_kickspeedz(0);
            grsim_robots[i][j]->set_velnormal(0);
            grsim_robots[i][j]->set_veltangent(0);
            grsim_robots[i][j]->set_velangular(0);
            grsim_robots[i][j]->set_spinner(false);
            grsim_robots[i][j]->set_wheelsspeed(false);
        }
        int size = grsim_packet[i].ByteSizeLong();
        data.resize(size);
        grsim_packet[i].SerializeToArray(data.ptr(), size);
        p_sim_packet.publish(data);
    }
}

}

