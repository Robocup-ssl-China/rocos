#include "communicator.h"
#include "staticparams.h"
#include <QNetworkInterface>
#include <QHostAddress>
#include "zss_cmd.pb.h"
#include "actionmodule.h"
#include "simmodule.h"
#include "parammanager.h"
#include "remotesim.h"
#include "globaldata.h"
#include "globalsettings.h"
#include <mutex>
#include <thread>
namespace {
int fps[2] = {0, 0};
std::mutex m_fps;
std::thread* receiveThread[PARAM::TEAMS];
bool NoVelY = true;
}
int Communicator::getFPS(int t) {
    int res = 0;
    m_fps.lock();
    res = fps[t];
    fps[t] = 0;
    m_fps.unlock();
    return res;
}

void Communicator::setGrsimInterfaceIndex(const int index) {
    grsimInterfaceIndex = index;
}

Communicator::Communicator(QObject *parent) : QObject(parent) {
    ZSS::ZParamManager::instance()->loadParam(NoVelY, "Lesson/NoVelY", false);
    if (grsimInterfaceIndex == 0){
        qDebug() << "connect sim";
        QObject::connect(ZSS::ZSimModule::instance(), SIGNAL(receiveSimInfo(int, int)), this, SLOT(sendCommand(int, int)),Qt::DirectConnection);
    }
//    QObject::connect(ZSS::ZRemoteSimModule::instance(), SIGNAL(receiveRemoteInfo(int, int)), this, SLOT(sendCommand(int, int)),Qt::DirectConnection);
    QObject::connect(ZSS::NActionModule::instance(), SIGNAL(receiveRobotInfo(int, int)), this, SLOT(sendCommand(int, int)),Qt::DirectConnection);
    for(int i = 0; i < PARAM::TEAMS; i++) {
//        connect(&receiveSocket[i], &QUdpSocket::readyRead, [ = ]() {
//            receiveCommand(i);
//        });
        if(connectMedusa(i)) {
            receiveThread[i] = new std::thread([ = ] {receiveCommand(i);});
            receiveThread[i]->detach();
        }
    }
}
Communicator::~Communicator() {
    sendSockets.disconnectFromHost();
    for(int i = 0; i < PARAM::TEAMS; i++) {
        receiveSocket[i].abort();
    }
}
bool Communicator::connectMedusa(int t) {
    ZSS::ZParamManager::instance()->loadParam(isSimulation, "Alert/IsSimulation", false);
    if (receiveSocket[t].bind(QHostAddress::AnyIPv4, ZSS::Athena::CONTROL_SEND[t], QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        return true;
    }
    qDebug() << "Bind ERROR";
    disconnectMedusa(t);
    return false;
}
bool Communicator::disconnectMedusa(int t) {
    ZSS::ZParamManager::instance()->loadParam(isSimulation, "Alert/IsSimulation", false);
    receiveSocket[t].disconnectFromHost();
    return true;
}
void Communicator::reloadSimulation(){
    ZSS::ZParamManager::instance()->loadParam(isSimulation, "Alert/IsSimulation", false);
}
void Communicator::receiveCommand(int t) {
    QByteArray datagram;
    while(true) {
        std::this_thread::sleep_for(std::chrono::microseconds(500));
        if (receiveSocket[t].state() == QUdpSocket::BoundState && receiveSocket[t].hasPendingDatagrams()) {
            m_fps.lock();
            fps[t]++;
            m_fps.unlock();
            datagram.resize(receiveSocket[t].pendingDatagramSize());
            receiveSocket[t].readDatagram(datagram.data(), datagram.size());
            ZSS::Protocol::Robots_Command commands;
            commands.ParseFromArray(datagram, datagram.size());
            commandBuffer[t].valid = true;
            for(int i = 0; i < commands.command_size(); i++) {
                auto& command = commands.command(i);
				auto vy = NoVelY ? 0.0f : command.velocity_y();
                RobotSpeed rs(command.velocity_x(), vy, command.velocity_r());
                commandBuffer[t].robotSpeed[command.robot_id()] = rs;
            }
            if(isSimulation) {
//                qDebug() << "simulation";
                if (grsimInterfaceIndex==0)
                    ZSS::ZSimModule::instance()->sendSim(t, commands);
                else
                    ZSS::ZRemoteSimModule::instance()->sendSim(t, commands);
            } else {
//                qDebug() << "realreal!";
                ZSS::ZActionModule::instance()->sendLegacy(t, commands);
                ZSS::NActionModule::instance()->sendLegacy(commands);
            }
        }
    }
}

void Communicator::sendCommand(int team, int id) {
//    qDebug() << "send";
    GlobalData::instance()->robotInfoMutex.lock();
    bool infrared = GlobalData::instance()->robotInformation[team][id].infrared;
    bool flat = GlobalData::instance()->robotInformation[team][id].flat;
    bool chip = GlobalData::instance()->robotInformation[team][id].chip;
    GlobalData::instance()->robotInfoMutex.unlock();

    ZSS::Protocol::Robot_Status robot_status;
    robot_status.set_robot_id(id);
    robot_status.set_infrared(infrared);
    robot_status.set_flat_kick(flat);
    robot_status.set_chip_kick(chip);
    int size = robot_status.ByteSizeLong();
    QByteArray datagram(size, 0);
    robot_status.SerializeToArray(datagram.data(), size);
    sendSockets.writeDatagram(datagram.data(), size, QHostAddress(ZSS::LOCAL_ADDRESS), ZSS::Athena::CONTROL_BACK_RECEIVE[team]);
}
