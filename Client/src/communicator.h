#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <QObject>
#include <QMutex>
#include <QUdpSocket>
#include <singleton.hpp>
#include "staticparams.h"
#include "globaldata.h"
class Communicator : public QObject {
    Q_OBJECT
  public:
    explicit Communicator(QObject *parent = nullptr);
    ~Communicator();
    bool connectMedusa(int);
    bool disconnectMedusa(int);
    void reloadSimulation();
    int getFPS(int);
    const RobotCommands getCommands(int t) {
        return commandBuffer[t];
    }
    void clearCommands() {
        commandBuffer[PARAM::BLUE] = RobotCommands();
        commandBuffer[PARAM::YELLOW] = RobotCommands();
    }
  private slots:
    void receiveCommand(int);
    void sendCommand(int, int);
  private:
    QUdpSocket sendSockets, receiveSocket[PARAM::TEAMS];
    QStringList networkInterfaceNames;
    QStringList networkInterfaceReadableNames;
    RobotCommands commandBuffer[PARAM::TEAMS];
    int networkInterfaceIndex;
    QMutex robotInfoMutex;
    bool isSimulation;
};
typedef Singleton<Communicator> ZCommunicator;
#endif // COMMUNICATOR_H
