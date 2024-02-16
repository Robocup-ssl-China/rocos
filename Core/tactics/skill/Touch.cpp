#include "Touch.h"
#include "parammanager.h"
#include "WorldDefine.h"
#include "VisionModule.h"
#include "staticparams.h"
namespace{
    const CGeoPoint THEIR_GOAL = CGeoPoint(PARAM::Field::PITCH_LENGTH/2,0);
    bool DEBUG_SWITCH;
}
CTouch::CTouch(){
    ZSS::ZParamManager::instance()->loadParam(DEBUG_SWITCH,"Debug/Touch",false);
}
void CTouch::plan(const CVisionModule* pVision){
    if (pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
    }

    const CGeoPoint target = task().player.pos;
    const bool useInter = task().player.is_specify_ctrl_method;
    const int runner = task().executor;
    auto taskFlag = task().player.flag;
    const PlayerVisionT& me = pVision->ourPlayer(runner);
    const auto mousePos = me.Pos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me.Dir());
    const MobileVisionT& ball = pVision->ball();
    const double ballVelDir = ball.Vel().dir();
    const CGeoPoint& ballPos = ball.RawPos();
    const CGeoLine ballVelLine(ballPos, ballVelDir);
    const double ballVelMod = ball.Vel().mod();
    const CGeoPoint projectionPos = ballVelLine.projection(mousePos);
    const double toBallDist = (mousePos - ballPos).mod();
    CGeoPoint targetPos;
    double targetDir;
    if(ballVelMod < 300){
        targetDir = (target - ballPos).dir();
        targetPos = ballPos + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,targetDir + PARAM::Math::PI);

    }else{
        targetDir = useInter ? Utils::Normalize(ballVelDir + PARAM::Math::PI) : (target - mousePos).dir();
        targetPos = projectionPos + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,targetDir + PARAM::Math::PI);
    }

    // add avoid ball flag
    if(toBallDist>200){
        taskFlag |= PlayerStatus::DODGE_BALL;
    }


    TaskT newTask(task());
    newTask.player.pos = targetPos;
    newTask.player.angle = targetDir;
    newTask.player.flag = taskFlag;
    // setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(newTask));
    setSubTask("SmartGoto", newTask);

    if(DEBUG_SWITCH){
        auto endPos = ballPos + Utils::Polar2Vector(ballVelMod,ballVelDir);
        GDebugEngine::Instance()->gui_debug_line(ballPos,endPos,4);
    }

    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
}
