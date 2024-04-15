#include <fmt/core.h>
#include <algorithm>
#include "VisionModule.h"
#include "DribbleStatus.h"
#include <CommandFactory.h>
#include "RobotCapability.h"
#include "parammanager.h"
#include "CircleRun.h"
namespace {
    double MAX_ACC = 6000;//mm/s^2
    double MAX_ROT_ACC = 50;
    bool DEBUG = false;
}
CCircleRun::CCircleRun(){
    ZSS::ZParamManager::instance()->loadParam(MAX_ACC,"CircleRun/MAX_ACC",6000);
    ZSS::ZParamManager::instance()->loadParam(MAX_ROT_ACC,"CircleRun/MAX_ROT_ACC",50);
    ZSS::ZParamManager::instance()->loadParam(DEBUG,"CircleRun/DEBUG",false);
}

void CCircleRun::plan(const CVisionModule* pVision){
    return ;
}
CPlayerCommand* CCircleRun::execute(const CVisionModule* pVision){
    const int vecNumber = task().executor;
    const CGeoPoint center = task().player.pos; // rotate center in robot coordinate
    const double targetRotVel = task().player.rotvel; // omega

    const PlayerVisionT& me = pVision->ourPlayer(vecNumber);
    const double meRotVel = me.RotVel();
    const CVector meVel = me.Vel().rotate(-(me.Dir()-me.RotVel()/PARAM::Vision::FRAME_RATE));
    const CVector me2center = center - CGeoPoint(0,0);
    const double targetVelMod = me2center.mod() * targetRotVel;
    const double targetVelDir = me2center.rotate(-PARAM::Math::PI/2).dir();
    const CVector targetVel = Utils::Polar2Vector(targetVelMod, targetVelDir);
    
    const CVector velDiff = targetVel - meVel;

    const CVector dv = Utils::Polar2Vector(std::min(velDiff.mod(), MAX_ACC/PARAM::Vision::FRAME_RATE), velDiff.dir());

    const CVector localVel = meVel + dv;

    const double limitTargetRotVel = std::abs(targetVelMod > 0.1) ? targetRotVel*localVel.mod()/targetVelMod : targetRotVel;
    const double dRotVel = std::clamp(limitTargetRotVel - meRotVel, -MAX_ROT_ACC/PARAM::Vision::FRAME_RATE, MAX_ROT_ACC/PARAM::Vision::FRAME_RATE);
    const double rotVel = meRotVel + dRotVel;

    const bool needDribble = task().player.flag & PlayerStatus::DRIBBLING;
    auto dribblePower = DribbleStatus::Instance()->getDribbleCommand(vecNumber);
    if (needDribble) {
        dribblePower = DRIBBLE_HIGHEST;
    }
    GDebugEngine::instance()->gui_debug_msg(me.RawPos(), fmt::format("localVel: ({:.2f}, {:.2f}), rotVel: {:.2f}", localVel.x(), localVel.y(), rotVel), COLOR_GREEN);
    GDebugEngine::instance()->gui_debug_line(me.RawPos(), me.RawPos() + meVel.rotate(me.Dir()), COLOR_PURPLE);
    GDebugEngine::instance()->gui_debug_line(me.RawPos(), me.RawPos() + targetVel.rotate(me.Dir()), COLOR_RED);
    GDebugEngine::instance()->gui_debug_line(me.RawPos(), me.RawPos() + localVel.rotate(me.Dir()), COLOR_BLUE);
    // GDebugEngine::instance()->gui_debug_line(me.RawPos(), me.RawPos() + me2center.rotate(me.Dir()), COLOR_GREEN);
    GDebugEngine::instance()->gui_debug_x(me.RawPos() + me2center.rotate(me.Dir()), COLOR_GREEN);
    GDebugEngine::instance()->gui_debug_msg(me.RawPos() + me2center.rotate(me.Dir()), fmt::format("v:{:.1f},tv:{:.1f}",meVel.mod(),localVel.mod()),COLOR_GREEN);
    return CmdFactory::Instance()->newCommand(CPlayerSpeedV2(vecNumber, localVel.x(), localVel.y(), rotVel, dribblePower));
}