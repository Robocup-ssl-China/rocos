#include "Goalie.h"
#include "TaskMediator.h"
#include "WorldDefine.h"
#include "VisionModule.h"
#include "staticparams.h"
#include "parammanager.h"
#include "KickStatus.h"
namespace{
    bool DEBUG_SWITCH;
    const CGeoPoint OUR_GOAL(-PARAM::Field::PITCH_LENGTH/2,0);
    const CGeoPoint LEFT_GOAL_POST(-PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH/2 - 2);
    const CGeoPoint RIGHT_GOAL_POST(-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2 + 2);
    const CGeoSegment GOAL_LINE(LEFT_GOAL_POST,RIGHT_GOAL_POST);
    const static int MIN_X = -PARAM::Field::PITCH_LENGTH/2;
    const static int MAX_X = -PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::PENALTY_AREA_DEPTH;
    const static int MIN_Y = -PARAM::Field::PENALTY_AREA_WIDTH/2;
    const static int MAX_Y = PARAM::Field::PENALTY_AREA_WIDTH/2;

    double get_defence_direction(const CGeoPoint & pos){
        double leftPostToBallDir = (pos - LEFT_GOAL_POST).dir();
        double rightPostToBallDir = (pos - RIGHT_GOAL_POST).dir();
        if(DEBUG_SWITCH){
            GDebugEngine::Instance()->gui_debug_line(pos,LEFT_GOAL_POST,6);
            GDebugEngine::Instance()->gui_debug_line(pos,RIGHT_GOAL_POST,6);
        }
        return Utils::Normalize((leftPostToBallDir + rightPostToBallDir) / 2 + PARAM::Math::PI);
    }
    bool in_our_penalty(const CGeoPoint& pos,double padding){
        if(pos.x() > MIN_X + padding && pos.x() < MAX_X - padding && pos.y() > MIN_Y + padding && pos.y() < MAX_Y - padding)
            return true;
        return false;
    }
}
CGoalie::CGoalie():_state(CGoalie::NOTHING){
    ZSS::ZParamManager::instance()->loadParam(DEBUG_SWITCH,"Debug/Goalie",false);
}
void CGoalie::plan(const CVisionModule* pVision){
    if (pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
        _state = CGoalie::NOTHING;
    }
    const int vecNumber = task().executor;
    const PlayerVisionT& self = pVision->ourPlayer(vecNumber);
    auto ball = pVision->ball().Pos();
    CGeoEllipse stand_ellipse(OUR_GOAL,PARAM::Field::PENALTY_AREA_DEPTH/2,PARAM::Field::GOAL_WIDTH/2);
    CGeoLineEllipseIntersection stand_intersection(CGeoLine(ball,get_defence_direction(ball)),stand_ellipse);
    CGeoPoint stand_pos;
    if(stand_intersection.intersectant()){
        if(ball.dist2(stand_intersection.point1())<ball.dist2(stand_intersection.point2()))
            stand_pos = stand_intersection.point1();
        else
            stand_pos = stand_intersection.point2();
        if(stand_pos.x() < MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE)
            stand_pos.setX(MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE);
    }else{
        stand_pos = CGeoPoint(-PARAM::Field::PITCH_LENGTH/2+PARAM::Vehicle::V2::PLAYER_SIZE,0);
    }
    double stand_dir = (ball-stand_pos).dir();
    bool need_clear = in_our_penalty(ball,-16*10);

    CGeoPoint clear_pos = ball + Utils::Polar2Vector(80,(OUR_GOAL - ball).dir());

    // SAVE
    auto ball_vel = pVision->ball().Vel().mod();
    auto ball_vel_dir = pVision->ball().Vel().dir();
    double distance = ball_vel * ball_vel / 1600;
    CGeoSegment ball_line(ball,ball + Utils::Polar2Vector(distance,ball_vel_dir));
//    GDebugEngine::Instance()->gui_debug_line(ball, ball + Utils::Polar2Vector(99999,ball_vel_dir));
    CGeoLineLineIntersection danger_intersection(ball_line,GOAL_LINE);
    bool danger_to_our_goal = danger_intersection.Intersectant() &&
            ball_line.IsPointOnLineOnSegment(danger_intersection.IntersectPoint()) &&
            GOAL_LINE.IsPointOnLineOnSegment(danger_intersection.IntersectPoint()) &&
            (ball_vel_dir - (danger_intersection.IntersectPoint() - ball).dir()) < PARAM::Math::PI/18 &&
            ball.dist(OUR_GOAL) < PARAM::Field::PITCH_LENGTH/4.;
//    GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0, 0), QString("%1").arg(ball_line.IsPointOnLineOnSegment(danger_intersection.IntersectPoint())).toLatin1());
//    GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0, 100), QString("%1").arg(GOAL_LINE.IsPointOnLineOnSegment(danger_intersection.IntersectPoint())).toLatin1());
//    GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0, 200), QString("%1").arg((ball_vel_dir - (danger_intersection.IntersectPoint() - ball).dir()) < PARAM::Math::PI/18).toLatin1());

    TaskT newTask(task());

    if(danger_to_our_goal){
        _state = SAVE;
        // setSubTask(TaskFactoryV2::Instance()->Touch(newTask));
        setSubTask("Touch",newTask);
    }
    else if(need_clear){
        _state = CLEAR;
        newTask.player.pos = clear_pos;
        newTask.player.angle = stand_dir;
        DribbleStatus::Instance()->setDribbleCommand(vecNumber, 3);
        KickStatus::Instance()->setKick(vecNumber,8000);
        // setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(newTask));
        setSubTask("SmartGoto", newTask);
    }else{
        _state = STAND;
        newTask.player.pos = stand_pos;
        newTask.player.angle = stand_dir;
        // setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(newTask));
        setSubTask("SmartGoto", newTask);
    }
//    if(...)
//        setSubTask(...)
//    elseif(...)
//        setSubTask(...)

    if(DEBUG_SWITCH){
        switch(_state){
        case NOTHING:
            debug_state = "NOTHING";
            break;
        case STAND:
            debug_state = "NORMAL STAND";
            break;
        case CLEAR:
            debug_state = "CLEAR BALL";
            GDebugEngine::Instance()->gui_debug_x(clear_pos,0);
            GDebugEngine::Instance()->gui_debug_msg(clear_pos,"cpos",0);
            break;
        case SAVE:
            debug_state = "SAVE";
            break;
        default:
            debug_state = "ERROR STATE";
        }
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2),debug_state.c_str());
    }
    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
}
