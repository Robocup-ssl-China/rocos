#include <QUdpSocket>
#include "staticparams.h"
#include "RefereeBoxIf.h"
#include <iostream>
#include "playmode.h"
#include "ssl_referee.pb.h"
#include "parammanager.h"
#include "GDebugEngine.h"
#include <thread>
#include <fmt/format.h>
namespace {
    int REFEREE_PORT = 10003;
    struct sCMD_TYPE {
        char cmd;
        unsigned int step;
    };

    struct stGamePacket {
        char cmd;
        unsigned char cmd_counter;
        unsigned char goals_blue;
        unsigned char goals_yellow;
        unsigned short time_remaining;
    };
    int VECTOR = 1;
}

CRefereeBoxInterface::~CRefereeBoxInterface() {
    stop();
}

CRefereeBoxInterface::CRefereeBoxInterface():_playMode(PMNone) {
//    ZSS::ZParamManager::instance()->loadParam(REFEREE_PORT,"AlertPorts/RefereePort",10003);
    ZSS::ZParamManager::instance()->loadParam(REFEREE_PORT,"AlertPorts/ZSS_RefereePort",39991);
    bool isSimulation;
    bool isYellow;
    bool isRight;
    ZSS::ZParamManager::instance()->loadParam(isSimulation,"Alert/IsSimulation",false);
    ZSS::ZParamManager::instance()->loadParam(isYellow,"ZAlert/IsYellow",false);
    ZSS::ZParamManager::instance()->loadParam(isRight,"ZAlert/IsRight",false);
    VECTOR = isRight ? -1 : 1;

    if(isSimulation && isYellow)
        REFEREE_PORT += 1; // use to fight against in simulation mode

    receiveSocket.bind(QHostAddress::AnyIPv4,REFEREE_PORT, QUdpSocket::ShareAddress);
    receiveSocket.joinMulticastGroup(QHostAddress(ZSS::REF_ADDRESS)); // receive Athena ref, need to change ZSS_ADDRESS
}

ZSS_THREAD_FUNCTION void CRefereeBoxInterface::start() {
    _alive = true;
    try {
            auto threadCreator = std::thread([=]{receivingLoop();});
            threadCreator.detach();
    }
    catch(std::exception e) {
        std::cout << "Error: Can't start RefereeBox Interface thread" << std::endl;
    }
}

void CRefereeBoxInterface::stop() {
    _alive = false;
}

void CRefereeBoxInterface::receivingLoop() {
    Referee ssl_referee;
    QByteArray datagram;
    while( _alive ) {
        while (receiveSocket.state() == QUdpSocket::BoundState && receiveSocket.hasPendingDatagrams()) {
            datagram.resize(receiveSocket.pendingDatagramSize());
            receiveSocket.readDatagram(datagram.data(), datagram.size());
            ssl_referee.ParseFromArray((void*)datagram.data(), datagram.size());
//            unsigned long long packet_timestamp = ssl_referee.packet_timestamp();
//            const auto& stage = ssl_referee.stage();
            const auto& command = ssl_referee.command();
            unsigned long command_counter = ssl_referee.command_counter();
//            unsigned long long command_timestamp = ssl_referee.command_timestamp();
            const auto& yellow = ssl_referee.yellow();
            const auto& blue = ssl_referee.blue();
            if(ssl_referee.has_next_command()){
                _next_command = ssl_referee.next_command();
            }else{
                _next_command = 0;
            }
            long long stage_time_left = 0;
            if (ssl_referee.has_stage_time_left())
            {
                stage_time_left = ssl_referee.stage_time_left();
            }
            char cmd;
            struct stGamePacket pCmd_temp;
            unsigned char cmd_index = 0;
            switch(command) {
            case 0: cmd = 'H'; break; // Halt
            case 1: cmd = 'S'; break; // Stop
            case 2: cmd = ' '; break; // Normal start (Ready)
            case 3: cmd = 's'; break; // Force start (Start)
            case 4: cmd = 'k'; break; // Kickoff Yellow
            case 5: cmd = 'K'; break; // Kickoff Blue
            case 6: cmd = 'p'; break; // Penalty Yellow
            case 7: cmd = 'P'; break; // Penalty Blue
            case 8: cmd = 'f'; break; // Direct Yellow
            case 9: cmd = 'F'; break; // Direct Blue
            case 10: cmd = 'i'; break; // Indirect Yellow
            case 11: cmd = 'I'; break; // Indirect Blue
            case 12: cmd = 't'; break; // Timeout Yellow
            case 13: cmd = 'T'; break; // Timeout Blue
            case 14: cmd = 'g'; break; // Goal Yellow
            case 15: cmd = 'G'; break; // Goal Blue
            case 16: cmd = 'b'; break; // Ball Placement Yellow
            case 17: cmd = 'B'; break; // Ball Placement Blue
            default:
                std::cout << "refereebox is fucked !!!!! command : " << command << std::endl;
                cmd = 'H';break;
            }
            // Get Ball Placement Position
            if(command == 16 || command == 17){
            //change ssl_vision_xy to zjunlict_vision_xy
                refereeMutex.lock();
                _ballPlacementX = VECTOR * ssl_referee.designated_position().x();
                _ballPlacementY = VECTOR * ssl_referee.designated_position().y();
                CGeoPoint point(_ballPlacementX,_ballPlacementY);
                refereeMutex.unlock();
                GDebugEngine::Instance()->gui_debug_x(point,COLOR_WHITE);
                GDebugEngine::Instance()->gui_debug_msg(point,"BP_Point",COLOR_WHITE);
                GDebugEngine::Instance()->gui_debug_arc(point,15,0,360,COLOR_WHITE);
            }
            if(command)
                pCmd_temp.cmd = cmd;
            pCmd_temp.cmd_counter = command_counter;
            pCmd_temp.goals_blue = blue.score();
            pCmd_temp.goals_yellow = yellow.score();
            pCmd_temp.time_remaining = (unsigned short)(stage_time_left / 1000000);

            GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0,PARAM::Field::PITCH_WIDTH/2),fmt::format("ref_cmd : {}-{}",command,Referee_Command_Name(command)),COLOR_RED,0,130);
            static unsigned char former_cmd_index = 0;
            cmd_index = pCmd_temp.cmd_counter;
            if (cmd_index != former_cmd_index) {
                former_cmd_index = cmd_index;    // 更新上一次指令得标志值
                PlayMode pmode = PMNone;
                for( int pm = PMStop; pm <= PMNone; ++pm ) {
                    if( playModePair[pm].ch == cmd ) {
                        // 寻找匹配的指令名字
                        pmode = playModePair[pm].mode;
                        std::cout << " pm : " << pm << std::endl;
                        break;
                    }
                }
                if( pmode != PMNone ) {
                    refereeMutex.lock();
                    _playMode = pmode;
                    _blueGoalNum = (int)pCmd_temp.goals_blue;
                    _yellowGoalNum = (int)pCmd_temp.goals_yellow;
                    _blueGoalie = (int)blue.goalkeeper();
                    _yellowGoalie = (int)yellow.goalkeeper();
                    _remainTime = pCmd_temp.time_remaining;
                    refereeMutex.unlock();
                    std::cout << "Protobuf Protocol: RefereeBox Command : " << cmd << " what : " << playModePair[pmode].what << std::endl;
                    std::cout << "Stage_time_left : "<< _remainTime << " Goals for blue : "<< (int)(pCmd_temp.goals_blue)
                    << " Goals for yellow : "<< (int)(pCmd_temp.goals_yellow) << std::endl;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
