#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include "singleton.hpp"
#include "grSim_Packet.pb.h"
#include <QUdpSocket>
#include "zsplugin.hpp"
class CSimulator : public QObject, public ZSPlugin
{
    Q_OBJECT
public:
    explicit CSimulator(QObject *parent = nullptr);
    void setBall(double x,double y,double vx = 0,double vy = 0);
    void setRobot(double x,double y,int id,bool team,double dir = 0,bool turnon = true);
//    void moveinRobot(int id,bool team);
//    void moveoutRobot(int id,bool team);
    void controlRobot(int num,bool team);
    void run(){}
signals:

public slots:

private:
//    QUdpSocket sendSocket;
    void send(grSim_Packet*);
};
typedef Singleton<CSimulator> Simulator;
#endif // SIMULATOR_H
