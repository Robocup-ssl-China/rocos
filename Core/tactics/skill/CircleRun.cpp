#include <fmt/core.h>
#include <algorithm>
#include "VisionModule.h"
#include "DribbleStatus.h"
#include <CommandFactory.h>
#include "RobotCapability.h"
#include "CircleRun.h"
namespace {
    const double MAX_ACC = 6000;//mm/s^2
    const double MAX_ROT_ACC = 50;
}
void CCircleRun::plan(const CVisionModule* pVision){
    return ;
}
CPlayerCommand* CCircleRun::execute(const CVisionModule* pVision){
    const int vecNumber = task().executor;
    const CGeoPoint center = task().player.pos; // rotate center in robot coordinate
    const double targetRotVel = task().player.rotvel; // omega

    const PlayerVisionT& me = pVision->ourPlayer(vecNumber);
    const CVector meVel = me.Vel().rotate(-me.Dir());
    const double meRotVel = me.RotVel();
    const double dRotVel = std::clamp(targetRotVel - meRotVel, -MAX_ROT_ACC/PARAM::Vision::FRAME_RATE, MAX_ROT_ACC/PARAM::Vision::FRAME_RATE);
    const double rotVel = meRotVel + dRotVel;
    const CVector me2center = center - CGeoPoint(0,0);
    const double targetVelMod = me2center.mod() * rotVel;
    const double targetVelDir = me2center.rotate(-std::copysign(PARAM::Math::PI/2, targetRotVel)).dir();
    const CVector targetVel = Utils::Polar2Vector(targetVelMod, targetVelDir);
    
    const CVector velDiff = targetVel - meVel;

    const CVector dv = Utils::Polar2Vector(std::min(velDiff.mod(), MAX_ACC/PARAM::Vision::FRAME_RATE), velDiff.dir());

    const CVector localVel = meVel + dv;

    const bool needDribble = task().player.flag & PlayerStatus::DRIBBLING;
    auto dribblePower = DribbleStatus::Instance()->getDribbleCommand(vecNumber);
    if (needDribble) {
        dribblePower = DRIBBLE_HIGHEST;
    }
    GDebugEngine::instance()->gui_debug_msg(me.Pos(), fmt::format("localVel: ({:.2f}, {:.2f}), rotVel: {:.2f}", localVel.x(), localVel.y(), rotVel), COLOR_GREEN);
    GDebugEngine::instance()->gui_debug_line(me.Pos(), me.Pos() + localVel.rotate(me.Dir()), COLOR_GREEN);
    GDebugEngine::instance()->gui_debug_line(me.Pos(), me.Pos() + me2center.rotate(me.Dir()), COLOR_GREEN);
    GDebugEngine::instance()->gui_debug_x(me.Pos() + me2center.rotate(me.Dir()), COLOR_GREEN);
    return CmdFactory::Instance()->newCommand(CPlayerSpeedV2(vecNumber, localVel.x(), localVel.y(), rotVel, dribblePower));
}