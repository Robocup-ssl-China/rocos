#ifndef REFEREEBOX_H
#define REFEREEBOX_H

#include <QObject>
#include <QUdpSocket>
#include <QMutex>
/*
//enum SSL_Referee_Command {
//  SSL_Referee_Command_HALT = 0,
//  SSL_Referee_Command_STOP = 1,
//  SSL_Referee_Command_NORMAL_START = 2,
//  SSL_Referee_Command_FORCE_START = 3,
//  SSL_Referee_Command_PREPARE_KICKOFF_YELLOW = 4,
//  SSL_Referee_Command_PREPARE_KICKOFF_BLUE = 5,
//  SSL_Referee_Command_PREPARE_PENALTY_YELLOW = 6,
//  SSL_Referee_Command_PREPARE_PENALTY_BLUE = 7,
//  SSL_Referee_Command_DIRECT_FREE_YELLOW = 8,
//  SSL_Referee_Command_DIRECT_FREE_BLUE = 9,
//  SSL_Referee_Command_INDIRECT_FREE_YELLOW = 10,
//  SSL_Referee_Command_INDIRECT_FREE_BLUE = 11,
//  SSL_Referee_Command_TIMEOUT_YELLOW = 12,
//  SSL_Referee_Command_TIMEOUT_BLUE = 13,
//  SSL_Referee_Command_GOAL_YELLOW = 14,
//  SSL_Referee_Command_GOAL_BLUE = 15
//};
*/
class RefBoxCommand : public QObject{
    Q_OBJECT
public:
    enum State {
        HALT = 0,
        STOP_GAME = 1,
        NORMAL_START = 2,
        FORCE_START = 3,
        KICKOFF_YELLOW = 4,
        KICKOFF_BLUE = 5,
        PENALTY_YELLOW = 6,
        PENALTY_BLUE = 7,
        DIRECT_KICK_YELLOW = 8,
        DIRECT_KICK_BLUE = 9,
        INDIRECT_KICK_YELLOW = 10,
        INDIRECT_KICK_BLUE = 11,
        TIMEOUT_YELLOW = 12,
        TIMEOUT_BLUE = 13,
        GOAL_YELLOW = 14,
        GOAL_BLUE = 15,
        BALL_PLACEMENT_YELLOW = 16,
        BALL_PLACEMENT_BLUE = 17
    };
    Q_ENUMS(State)
};

class GameState : public QObject{
    Q_OBJECT
public:
    enum State {
        HALTED,
        STOPPED,
        RUNNING,
        PREPARE_KICKOFF,
        PREPARE_PENALTY,
        TIMEOUT_YELLOW,
        TIMEOUT_BLUE,
    };
    Q_ENUMS(State)
};

class GameStage : public QObject{
    Q_OBJECT
public:
    enum Stage {
        Pregame,
        First_half,
        Half_time,
        PreSecond_half,
        Second_half,
        PreOvertime1,
        Overtime1,
        PreOvertime2,
        Overtime2,
        Penalty_shootout,
    };
    Q_ENUMS(Stage)
};

class RefereeBox : public QObject
{
    Q_OBJECT
public:
    explicit RefereeBox(QObject *parent = 0);
    ~RefereeBox();
    Q_INVOKABLE void changeCommand(int command);
    Q_INVOKABLE void setNextCommand(int command){nextCommand = command;}
    Q_INVOKABLE void multicastCommand();
signals:

public slots:
private:
    int port;
    quint32 commandCounter;
    QUdpSocket sendSocket;
    int currentCommand;
    int nextCommand;
    QMutex commandMutex;
};

#endif // REFEREEBOX_H
