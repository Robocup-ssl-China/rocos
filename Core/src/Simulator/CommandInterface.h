/************************************************************************/
/* @Brief: Robot commands unified interfaces to simulation and real     */
/* @Author: Wang Yunkai                                                 */
/* @Date: 2018.11.13                                                    */
/************************************************************************/

#ifndef __COMMAND_INTERFACE_H__
#define __COMMAND_INTERFACE_H__

#include <singleton.hpp>
#include <thread>
#include <mutex>
#include "staticparams.h"
#include <QObject>
#include <OptionModule.h>
#include "zss_cmd.pb.h"
#include "grSim_Replacement.pb.h"

class QUdpSocket;

struct RobotCommand{
    double velocity_x = 0;
    double velocity_y = 0;
    double velocity_r = 0;
    double flat_kick = 0;
    double chip_kick = 0;
    double dribble_spin = 0;
    bool use_dir = false;
    bool need_report = false;
    bool direct_kick_no_calibration=false;
    double direct_kick_power=0;
    RobotCommand() = default;
};

class CCommandInterface : public QObject{
    Q_OBJECT
private:
    CCommandInterface(const COptionModule *pOption, QObject *parent = nullptr);
    ~CCommandInterface(void);
public:
    static CCommandInterface* instance(const COptionModule *pOption=nullptr);
    static void destruct();
    void setSpeed(int num, double dribble, double vx, double vy, double vr);
    void setKick(int num, double kp, double cp, bool direct_kick_no_calibration=false, double direct_kick_power=0);
    void setNeedReport(int num, bool needReport);
    void placeRobot(int num, double x, double y, double dir);
    void placeBall(double x, double y, double vx, double vy);
    void sendCommands();
private slots:
    void receiveInformation();
private:
    static CCommandInterface* _instance;
    RobotCommand commands[PARAM::Field::MAX_PLAYER];
    const COptionModule *pOption;
    QUdpSocket *command_socket;
    QUdpSocket *receiveSocket;
private:
    int SIM_PORT;
    int SELF_PORT;
    int CHIP_ANGLE;
    int TEAM;
    int SIDE;
    bool DEBUG_CMD;
    std::mutex __placement_mutex;
    ZSS::Protocol::Robots_Command __robots_command;
    std::thread *_thread;
};

#endif
