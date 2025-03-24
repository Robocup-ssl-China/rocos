#include <fmt/core.h>
#include <algorithm>
#include "VisionModule.h"
#include "parammanager.h"
#include "RobotSensor.h"
#include <DribbleStatus.h>
#include "PlaceBall.h"
namespace {
bool DEBUG = false;
CGeoPoint ballLast = CGeoPoint(0,0);
bool verbose = true;
}
CPlaceBall::CPlaceBall(){
}

void CPlaceBall::plan(const CVisionModule* pVision){
    // if (pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE * 0.1) {
    // }
    const int vecNumber = task().executor;
    const CGeoPoint target = task().player.pos;
    const auto& ball = pVision->ball();
    const PlayerVisionT& me = pVision->ourPlayer(vecNumber);
    bool frared = RobotSensor::Instance()->IsInfraredOn(vecNumber);
    const int fraredOn = RobotSensor::Instance()->infraredOn(vecNumber);
    const int fraredOff = RobotSensor::Instance()->infraredOff(vecNumber);
    CGeoPoint ballPos = ball.Valid() ? ball.Pos() : ballLast;
    CVector me2ball = ballPos - me.Pos();
    CVector target2me = me.Pos() - target;
    CGeoPoint meTarget = target + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_SIZE, target2me.dir());
    static double medirstatic = me.Dir();
    bool valid = ((ball.Pos() - target).mod() < 130);
    bool ballArrived = (ballPos - target).mod() < 100 and (ball.Vel()).mod() < 200;

    GDebugEngine::Instance()->gui_debug_x(target, COLOR_ORANGE);
    GDebugEngine::Instance()->gui_debug_msg(target, fmt::format("bArr:{}", ballArrived).c_str(), COLOR_ORANGE);

    if (!ball.Valid()){
        ballPos = (!frared) ? ballLast : me.Pos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_FRONT_TO_CENTER, me.Dir());
    }
    if (ball.Valid() || fraredOn>30) ballLast = ballPos;

    TaskT newTask(task());
    /*********************** set subTask ********************/
    // if(!Utils::IsInField(ballPos, 2.5 * PARAM::Vehicle::V2::PLAYER_SIZE)){//ball out of field
    //     if(fraredOn < 60){//not have ball
    //         if(goBackBall){
    //             CGeoPoint gotoPoint = Utils::MakeInField(ballPos, 2*PARAM::Vehicle::V2::PLAYER_SIZE);
    //             if((me.RawPos() - gotoPoint).mod() < 2 * 10){
    //                 goBackBall = false;
    //             }
    //             if(verbose) GDebugEngine::Instance()->gui_debug_msg(me.Pos()+ Utils::Polar2Vector(DEBUG_TEXT_HIGH, -PARAM::Math::PI/1.5), "GoBackBall", COLOR_WHITE);
    //             setSubTask(PlayerRole::makeItGoto(vecNumber, gotoPoint, me2ball.dir(), PlayerStatus::ALLOW_DSS|PlayerStatus::NOT_AVOID_PENALTY));
    //         }
    //         else{
    //             if(verbose) GDebugEngine::Instance()->gui_debug_msg(me.Pos()+ Utils::Polar2Vector(DEBUG_TEXT_HIGH, -PARAM::Math::PI/1.5), "GotoBall", COLOR_WHITE);
    //             setSubTask(PlayerRole::makeItGoto(vecNumber, ballPos, me2ball.dir(), PlayerStatus::ALLOW_DSS|PlayerStatus::NOT_AVOID_PENALTY));
    //         }
    //     }
    //     else{//have ball
    //         if(verbose) GDebugEngine::Instance()->gui_debug_msg(me.Pos()+ Utils::Polar2Vector(DEBUG_TEXT_HIGH, -PARAM::Math::PI/1.5), "MakeInField", COLOR_WHITE);
    //         setSubTask(PlayerRole::makeItGoto(vecNumber, Utils::MakeInField(ballPos, 3*PARAM::Vehicle::V2::PLAYER_SIZE), me.Dir(), CVector(0, 0), 0, 3000, 5, 3000, 5, PlayerStatus::ALLOW_DSS|PlayerStatus::NOT_AVOID_PENALTY));
    //     }
    // }
    // else{
        if((fraredOff > 10 && !ballArrived))  /*&& !lastInfered*/ /*&& !WorldModel::Instance()->getBPFinish()*/{//not have ball
            if(verbose) GDebugEngine::Instance()->gui_debug_msg(me.Pos()+ Utils::Polar2Vector(200, -PARAM::Math::PI/1.5), "GetBall", COLOR_WHITE);
            setSubTask("Touch",newTask);
        }else{//have ball
            double me_to_target_anglediff = abs(Utils::Normalize(me.Dir() - (target - me.Pos()).dir()));
            if (ballArrived){
                CVector ball2me = me.RawPos() - ballPos;
                CGeoPoint go_out_pos = target + Utils::Polar2Vector(3.0 * PARAM::Vehicle::V2::PLAYER_SIZE, ball2me.dir());
                newTask.player.pos = go_out_pos;
                newTask.player.angle = ball2me.dir();
                newTask.player.max_acceleration = 2000;
                newTask.player.max_speed = 1000;
                newTask.player.max_rot_acceleration = 3;
                newTask.player.max_rot_speed = 3;
                newTask.player.flag = PlayerStatus::AVOID_STOP_BALL_CIRCLE | PlayerStatus::NOT_AVOID_PENALTY;
                setSubTask("SmartGoto", newTask);
                // setSubTask(PlayerRole::makeItGoto(vecNumber, go_out_pos, ball2me.dir(), CVector(0, 0), 0, 300, 0, 100, 0, PlayerStatus::AVOID_STOP_BALL_CIRCLE | PlayerStatus::NOT_AVOID_PENALTY));
            }else if (me_to_target_anglediff > M_PI / 5){
                medirstatic = (target - me.Pos()).dir();
                newTask.player.pos = me.Pos();
                newTask.player.angle = medirstatic;
                newTask.player.max_speed = 2000;
                newTask.player.max_rot_acceleration = 3;
                newTask.player.max_rot_speed = 2;
                setSubTask("SmartGoto", newTask);
                // setSubTask(PlayerRole::makeItGoto(vecNumber, me.Pos(), medirstatic, CVector(0, 0), 0, 600, 0, 100, 0, PlayerStatus::ALLOW_DSS|PlayerStatus::NOT_AVOID_PENALTY));
            }else{
                newTask.player.pos = meTarget;
                newTask.player.angle = medirstatic;
                newTask.player.max_rot_acceleration = 3;
                newTask.player.max_rot_speed = 3;
                if((ballPos - target).mod() < 300)
                    newTask.player.flag = PlayerStatus::ALLOW_DSS|PlayerStatus::NOT_AVOID_PENALTY;
                setSubTask("SmartGoto", newTask);
                // setSubTask(PlayerRole::makeItGoto(vecNumber, carPos, medirstatic, CVector(0, 0), 0, 600, 8, 500, 5, PlayerStatus::ALLOW_DSS|PlayerStatus::NOT_AVOID_PENALTY));
            }
        }
    // }

    /****************** other ***************/
    if ((me2ball.mod() < 500 || frared) && !ballArrived) {
        DribbleStatus::Instance()->setDribbleCommand(vecNumber, 2);
    }else {
        DribbleStatus::Instance()->setDribbleCommand(vecNumber, 0);
    }
    _lastCycle = pVision->getCycle();
    return Skill::plan(pVision);
}