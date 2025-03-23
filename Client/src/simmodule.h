#ifndef SIMMODULE_H
#define SIMMODULE_H
#include <QObject>
#include <QMutex>
#include <QUdpSocket>
#include <mutex>
#include "staticparams.h"
#include "zos/utils/singleton.h"
#include "zos/core.h"
#include "zss_cmd.pb.h"
#include "grSim_Packet.pb.h"
namespace ZSS{
class SimModule : public QObject, public Singleton<SimModule>{
    Q_OBJECT
public:
    SimModule(QObject *parent = 0);
    ~SimModule();
    bool connectSim(bool);
    bool disconnectSim(bool);
    void sendSim(int t, ZSS::Protocol::Robots_Command& command);
    void resetSimCommand();
private:
    void getRobotStatus(const zos::Data& data, int TEAM);

    QMutex robotInfoMutex;
    zos::Subscriber<2> s_blue_status;
    zos::Subscriber<2> s_yellow_status;
    zos::Publisher p_sim_packet;

    grSim_Packet grsim_packet[PARAM::TEAMS];
    grSim_Commands *grsim_commands[PARAM::TEAMS];
    grSim_Robot_Command *grsim_robots[PARAM::TEAMS][PARAM::ROBOTNUM];
    std::mutex _sim_pack_mutex;
signals:
    void receiveSimInfo(int,int);
};
}
#endif
