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
    void setGrsimInterfaceIndex(const int index);
    int getGrsimInterfaceIndex() {return grsimInterfaceIndex;}
    int getFPS(int);
    const RobotCommands getCommands(int t) {
        return commandBuffer[t];
    }
    void clearCommands() {
        commandBuffer[PARAM::BLUE] = RobotCommands();
        commandBuffer[PARAM::YELLOW] = RobotCommands();
    }
  public slots:
    void receiveCommand(int);
    void sendCommand(int, int);
  private:
    QUdpSocket sendSockets, receiveSocket[PARAM::TEAMS];
    QStringList networkInterfaceNames;
    QStringList networkInterfaceReadableNames;
    RobotCommands commandBuffer[PARAM::TEAMS];
    int networkInterfaceIndex;
    int grsimInterfaceIndex;
    bool isSimulation;
};
typedef Singleton<Communicator> ZCommunicator;
#endif // COMMUNICATOR_H
