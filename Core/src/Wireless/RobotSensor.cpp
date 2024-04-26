#include "RobotSensor.h"
CRobotSensor::CRobotSensor(){
    ZSS::ZParamManager::instance()->loadParam(DEBUG_DEVICE,"Debug/DeviceMsg",false);
}
void CRobotSensor::Update(const CVisionModule* pVision){
    static const int MAX_FRARED = 999;
    static const int FONT_SIZE = 50;
    static const double DEBUG_TEXT_LENGTH = FONT_SIZE*6;
    static const double DEBUG_X = PARAM::Field::PITCH_LENGTH / 2 - 3 * FONT_SIZE * (PARAM::Field::MAX_PLAYER) - DEBUG_TEXT_LENGTH;
    static const double DEBUG_X_STEP = PARAM::Field::PITCH_LENGTH / 30;
    static const double DEBUG_Y = -PARAM::Field::PITCH_WIDTH / 2 + 5*FONT_SIZE;
    static const double DEBUG_Y_STEP = PARAM::Field::PITCH_WIDTH / 45;
    if(DEBUG_DEVICE){
        const int DATA_SIZE = 5;
        std::array<std::string,DATA_SIZE> text{"INFR","FLAT","CHIP","INFON","INFOFF"};
        std::array<std::string,DATA_SIZE> msgs;
        std::array<std::string,DATA_SIZE> msgs2;
        for(int i=0;i<PARAM::Field::MAX_PLAYER;i++){
            robotMsg[i]._mutex.lock();
            auto& info = robotMsg[i];
            std::array<int,DATA_SIZE> info_data{info.infrared,info.flat_kick,info.chip_kick,info.infraredOn,info.infraredOff};
            std::array<bool,DATA_SIZE> info_check{info.infrared,info.flat_kick>0,info.chip_kick>0,info.infraredOn!=0,info.infraredOff<=30};
            for(int j=0;j<info_data.size();j++){
                msgs[j] += info_check[j] ? fmt::format("{:4s}","") : fmt::format("{:4s}","  --");
                msgs2[j] += info_check[j] ? fmt::format("{:4d}",info_data[j]) : fmt::format("{:4s}","");
            }
            robotMsg[i]._mutex.unlock();
        }
        for(int i=0;i<DATA_SIZE;i++){
            GDebugEngine::Instance()->gui_debug_msg_fix(CGeoPoint(DEBUG_X, DEBUG_Y + DEBUG_Y_STEP * i),fmt::format("{:6s}: ",text[i]),COLOR_GRAY,0,FONT_SIZE);
            GDebugEngine::Instance()->gui_debug_msg_fix(CGeoPoint(DEBUG_X+DEBUG_TEXT_LENGTH,DEBUG_Y + DEBUG_Y_STEP * i),msgs[i],COLOR_GRAY,0,FONT_SIZE);
            GDebugEngine::Instance()->gui_debug_msg_fix(CGeoPoint(DEBUG_X+DEBUG_TEXT_LENGTH,DEBUG_Y + DEBUG_Y_STEP * i),msgs2[i],COLOR_GREEN,0,FONT_SIZE);
        }
    }
    for(int i=0;i<PARAM::Field::MAX_PLAYER;i++){
        robotMsg[i]._mutex.lock();
        if(robotMsg[i].chip_kick > 0)
            robotMsg[i].chip_kick--;
        if(robotMsg[i].flat_kick > 0)
            robotMsg[i].flat_kick--;
        if(robotMsg[i].infrared){
            if(robotMsg[i].infraredOff != 0) robotMsg[i].dribblePoint = pVision->ourPlayer(i).Pos();
            robotMsg[i].infraredOn = robotMsg[i].infraredOn >= MAX_FRARED ? MAX_FRARED : robotMsg[i].infraredOn + 1;
            robotMsg[i].infraredOff = 0;
        }
        else{
            robotMsg[i].infraredOn = 0;
            robotMsg[i].infraredOff = robotMsg[i].infraredOff >= MAX_FRARED ? MAX_FRARED : robotMsg[i].infraredOff + 1;
        }
        robotMsg[i]._mutex.unlock();
    }
}
