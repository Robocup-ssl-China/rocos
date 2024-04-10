#include <fmt/core.h>
#include "Touch.h"
#include "parammanager.h"
#include "WorldDefine.h"
#include "VisionModule.h"
#include "staticparams.h"
#include "geomcalc.h"
#include "BallSpeedModel.h"
#include "CMmotion.h"
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
    const auto mouseVec = Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER, me.Dir());
    const auto mousePos = me.Pos() + mouseVec;
    const MobileVisionT& ball = pVision->ball();
    const double ballVelDir = ball.Vel().dir();
    const CGeoPoint& ballPos = ball.RawPos();
    const CGeoLine ballVelLine(ballPos, ballVelDir);
    const double ballVelMod = ball.Vel().mod();
    const CGeoPoint projectionMousePos = ballVelLine.projection(mousePos);
    const CGeoPoint projectionRobotPos = projectionMousePos + mouseVec*-1;
    const auto mouse2Ball = ballPos - mousePos;
    const double toBallDist = mouse2Ball.mod();
    // predict ball pos
    const auto ballStopPose = BallSpeedModel::instance()->poseForTime(9999);
    const auto getBallPos = BallSpeedModel::instance()->poseForTime(1.0).Pos(); // TODO : replace with GetBallPos after skillutils
    const CGeoSegment ballRunningSeg(ballPos, ballStopPose.Pos());
    const auto me2segTime = predictedTimeWithRawVel(me, projectionRobotPos);
    const auto ball2segTime = BallSpeedModel::Instance()->timeForDist(ballPos.dist(projectionMousePos));
    const bool canWaitForBall = ballRunningSeg.IsPointOnLineOnSegment(projectionMousePos) && me2segTime < ball2segTime;
    const auto predictPos = ballVelMod > 100 ? getBallPos : ballPos;

    CGeoPoint targetRunPos;
    double targetRunDir;
    CVector targetRunVel;
    if (!canWaitForBall){
        targetRunDir = (target - predictPos).dir();
        targetRunPos = predictPos + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER - 5, targetRunDir + PARAM::Math::PI);
        targetRunVel = Utils::Polar2Vector(300, mouse2Ball.dir());
    }
    else{
        targetRunDir = useInter ? Utils::Normalize(ballVelDir + PARAM::Math::PI) : (target - mousePos).dir();
        targetRunPos = projectionMousePos + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER-5,targetRunDir + PARAM::Math::PI);
    }

    const auto me2target = targetRunPos - me.Pos();
    const auto runTarget2kickTarget = target - targetRunPos;
    // add avoid ball flag
    auto angle_diff = angleDiff(me2target.dir(), runTarget2kickTarget.dir());
    GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-800,-800),fmt::format("anglediff  {:.2f}",angle_diff/PARAM::Math::PI*180.0));
    if (toBallDist > 150 && std::abs(angle_diff) > (100.0) / 180.0 * PARAM::Math::PI)
    {
        taskFlag |= PlayerStatus::DODGE_BALL;
    }

    TaskT newTask(task());
    newTask.player.pos = targetRunPos;
    newTask.player.angle = targetRunDir;
    newTask.player.vel = targetRunVel;
    newTask.player.flag = taskFlag;
    setSubTask("SmartGoto", newTask);

    if(DEBUG_SWITCH){
        auto endPos = ballPos + Utils::Polar2Vector(ballVelMod,ballVelDir);
        GDebugEngine::Instance()->gui_debug_line(ballPos,endPos,4);
        GDebugEngine::Instance()->gui_debug_msg(targetRunPos, fmt::format("TVel:{},me2SegT:{:3.1f},b2SegT:{:3.1f}", targetRunVel.mod(), me2segTime, ball2segTime));
    }

    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
}
