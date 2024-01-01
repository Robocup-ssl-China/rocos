#include <QtDebug>
#include <QElapsedTimer>
#include "refereebox.h"
#include "parammanager.h"
#include "staticparams.h"
#include "ssl_referee.pb.h"
#include "globalsettings.h"
void multicastCommand(int state);

RefereeBox::RefereeBox(QObject *parent) : QObject(parent),currentCommand(GameState::HALTED),nextCommand(GameState::HALTED){
    ZSS::ZParamManager::instance()->loadParam(port,"AlertPorts/ZSS_RefereePort",39991);
    sendSocket.setSocketOption(QAbstractSocket::MulticastTtlOption, 1);
    commandCounter = 1;
}
RefereeBox::~RefereeBox(){
}
void RefereeBox::changeCommand(int command){
    commandMutex.lock();
    currentCommand = command;
    commandCounter++;
    commandMutex.unlock();
}
void RefereeBox::multicastCommand(){
    Referee ssl_referee;
    ssl_referee.set_packet_timestamp(0);//todo
    ssl_referee.set_stage(Referee_Stage_NORMAL_FIRST_HALF);//todo
    ssl_referee.set_stage_time_left(0);//todo

    commandMutex.lock();
    ssl_referee.set_command(Referee_Command(currentCommand));
    ssl_referee.set_command_counter(commandCounter);//todo
    commandMutex.unlock();

    ssl_referee.set_command_timestamp(0);//todo
    Referee_TeamInfo *yellow = ssl_referee.mutable_yellow();
    yellow->set_name("ZJUNlict");//todo
    yellow->set_score(0);//todo
    yellow->set_red_cards(0);//todo
    //yellow->set_yellow_card_times(0,0); //todo
    yellow->set_yellow_cards(0);//todo
    yellow->set_timeouts(0.0);//todo
    yellow->set_timeout_time(0.0);//todo
    yellow->set_goalkeeper(0); //todo
    Referee_TeamInfo *blue = ssl_referee.mutable_blue();
    blue->set_name("ZJUNlict");
    blue->set_score(0);//todo
    blue->set_red_cards(0);//todo
    //blue->set_yellow_card_times(0,0); //todo
    blue->set_yellow_cards(0);//todo
    blue->set_timeouts(0.0);//todo
    blue->set_timeout_time(0.0);//todo
    blue->set_goalkeeper(0); //todo
    Referee_Point *point = ssl_referee.mutable_designated_position();
    point->set_x(GlobalSettings::instance()->ballPlacementX);
    point->set_y(GlobalSettings::instance()->ballPlacementY);
    // add test for next_command
    ssl_referee.set_next_command(Referee_Command(nextCommand));
    int size = ssl_referee.ByteSizeLong();
    QByteArray buffer(size,0);
    ssl_referee.SerializeToArray(buffer.data(), buffer.size());
    sendSocket.writeDatagram(buffer.data(), buffer.size(),
                                 QHostAddress(ZSS::REF_ADDRESS), port);
    sendSocket.writeDatagram(buffer.data(), buffer.size(),
                                 QHostAddress(ZSS::REF_ADDRESS), port+1);
}
