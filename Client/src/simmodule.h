#ifndef SIMMODULE_H
#define SIMMODULE_H
#include <QObject>
#include <QMutex>
#include <QUdpSocket>
#include "singleton.hpp"
#include "zss_cmd.pb.h"
#include "zsplugin.hpp"
namespace ZSS{
class SimModule : public QObject, public ZSPlugin{
    Q_OBJECT
public:
    SimModule(QObject *parent = 0);
    ~SimModule();
    bool connectSim(bool);
    bool disconnectSim(bool);
    void sendSim(int t, ZSS::Protocol::Robots_Command& command);
    void run(){}
private slots:
    void readBlueData();
    void readYellowData();
private:
    QByteArray tx;
    QByteArray rx;
    // QUdpSocket sendSocket;
    QString receiveAddress;
    QUdpSocket blueReceiveSocket;
    QUdpSocket yellowReceiveSocket;
    QMutex robotInfoMutex;
    QUdpSocket command_socket;
signals:
    void receiveSimInfo(int,int);
};
typedef Singleton<SimModule> ZSimModule;
}
#endif
