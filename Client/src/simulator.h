#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include "zos/utils/singleton.h"
#include "grSim_Packet.pb.h"
#include <QUdpSocket>
#include "zos/core.h"
class Simulator : public Singleton<Simulator>{
public:
    explicit Simulator();
    void setBall(double x,double y,double vx = 0,double vy = 0);
    void setRobot(double x,double y,int id,bool team,double dir = 0,bool turnon = true);
    void controlRobot(int num,bool team);
private:
    void send(grSim_Packet*);
    zos::Publisher p_sim_packet;
    QUdpSocket sendSocket;
};
#endif // SIMULATOR_H
