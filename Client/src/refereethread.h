#ifndef REFEREETHREAD_H
#define REFEREETHREAD_H
#include <QThread>
#include <QTcpSocket>
#include "singleton.hpp"
#include"ssl_game_controller_team.pb.h"
#include "google/protobuf/util/delimited_message_util.h"
class CRefereeThread: public QThread {
    Q_OBJECT
  public:
    CRefereeThread();
    ~CRefereeThread();
    bool connectTcp(QHostAddress address, quint16 port);
    bool disconnectTCP();
    bool registerTeam();
    bool requestKeeper (int keeper_num);
    bool advantageChoice(bool _stop);
    ControllerToTeam receiveControllerReply();
    void run();
  private:
    QTcpSocket* client;

};
typedef Singleton <CRefereeThread> RefereeThread;
#endif // REFEREETHREAD_H
