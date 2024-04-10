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
    template<typename T>
    T clip(T x, T min, T max){
        return std::max(min, std::min(max, x));
    }
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


    // stupid version of getballPos
    CGeoPoint bestPos = BallSpeedModel::instance()->poseForTime(1.0).Pos();
    for(double dist = 0; dist < 3000; dist += 100){
        auto pos = ballPos + Utils::Polar2Vector(dist, ballVelDir);
        double t1 = predictedTimeWithRawVel(me, pos);
        double t2 = BallSpeedModel::Instance()->timeForDist(dist);
        GDebugEngine::Instance()->gui_debug_x(pos,COLOR_GREEN);
        GDebugEngine::Instance()->gui_debug_msg(pos, fmt::format("t:{:.2f},{:.2f}", t1, t2), COLOR_GREEN);
        if(t1 < t2 || t1 < 0.1){
            bestPos = pos;
            break;
        }
    }
    const auto getBallPos = bestPos; // TODO : replace with GetBallPos after skillutils

    const CGeoSegment ballRunningSeg(ballPos, ballStopPose.Pos());
    const auto me2segTime = predictedTimeWithRawVel(me, projectionRobotPos);
    const auto ball2segTime = BallSpeedModel::Instance()->timeForDist(ballPos.dist(projectionMousePos));
    const bool canWaitForBall = ballRunningSeg.IsPointOnLineOnSegment(projectionMousePos) && me2segTime < ball2segTime;
    const auto predictPos = ballVelMod > 300 ? getBallPos : ballPos;
    const double ballVel_ball2Target_ad = ballVelMod > 300 ? angleDiff(ballVelDir, (target - ballPos).dir()) : 180;
    const bool angleCanTouch = std::abs(ballVel_ball2Target_ad) > 100 / 180.0 * PARAM::Math::PI;

    const CVector targetRunVel = canWaitForBall ? CVector(0, 0) : Utils::Polar2Vector(200, ballVelDir);
    const CGeoPoint targetMousePos = canWaitForBall ? projectionMousePos : predictPos;
    const double targetRunDir = (useInter || !angleCanTouch) ? Utils::Normalize(ballVelDir + PARAM::Math::PI) : (target - targetMousePos).dir();
    const CGeoPoint targetRunPos = targetMousePos + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER, targetRunDir + PARAM::Math::PI);

    // add avoid ball flag
    const auto me2target = targetRunPos - me.Pos();
    const auto me2TargetSeg = CGeoSegment(me.Pos(), targetRunPos);  
    const auto runTarget2kickTarget = target - targetRunPos;

    const auto BALL_STATIC = ballVelMod < 300;
    const auto runTargetDiff = angleDiff(me2target.dir(), runTarget2kickTarget.dir());
    const auto needAvoidStatic = BALL_STATIC && runTargetDiff;
    const auto avoidDistStatic = needAvoidStatic ? 30.0 : 0.0;

    const auto diff4avoid_ball = std::abs(angleDiff(me2target.dir(), (targetMousePos - ballPos).dir()));
    const auto needAvoidDynamic = !BALL_STATIC && me2TargetSeg.IsPointOnLineOnSegment(ballPos) && diff4avoid_ball < 90 / 180.0 * PARAM::Math::PI;
    const auto avoidDistDynamic = needAvoidDynamic ? 3*clip(90-diff4avoid_ball *180.0 / PARAM::Math::PI,0.0,90.0) : 0;

    double avoid_dist = 0;
    if (toBallDist > 120 && (needAvoidStatic || needAvoidDynamic)){
        taskFlag |= PlayerStatus::DODGE_BALL;
        avoid_dist = std::max(avoidDistStatic, avoidDistDynamic);
    }

    TaskT newTask(task());
    newTask.player.pos = targetRunPos;
    newTask.player.angle = targetRunDir;
    newTask.player.vel = targetRunVel;
    newTask.player.flag = taskFlag;
    newTask.ball.avoid_dist = 300;
    setSubTask("SmartGoto", newTask);

    if(DEBUG_SWITCH){
        auto endPos = ballPos + Utils::Polar2Vector(ballVelMod,ballVelDir);
        GDebugEngine::Instance()->gui_debug_line(ballPos,endPos,4);
        GDebugEngine::Instance()->gui_debug_msg(targetRunPos, fmt::format("TVel:{:.0f},me2SegT:{:3.1f},b2SegT:{:3.1f}", targetRunVel.mod(), me2segTime, ball2segTime));
        GDebugEngine::Instance()->gui_debug_msg(targetRunPos+CVector(0,120), fmt::format("modeDif:{:.1f}", ballVel_ball2Target_ad / PARAM::Math::PI * 180.0));
    }

    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
}
