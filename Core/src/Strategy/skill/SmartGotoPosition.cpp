#include "SmartGotoPosition.h"
#include <PathPlanner.h>
#include <utils.h>
#include <TaskMediator.h>
//#include <LeavePenaltyArea.h>
#include "PlayInterface.h"
#include <GDebugEngine.h>
#include <RobotCapability.h>
#include <CommandFactory.h>
#include <RefereeBoxIf.h>
#include <ControlModel.h>
#include <iostream>
#include "WorldModel.h"
#include "parammanager.h"
#include "CMmotion.h"
#include <iostream>

/************************************************************************/
/*                                                                      */
/************************************************************************/
namespace{
    /// 调试开关
    bool VERBOSE_MODE = false;
    bool DRAW_TARGET = false;
    bool USE_DEBUG = false;

    const double SlowFactor = 0.5;
    const double FastFactor = 1.2;
    /// 底层运动控制参数 ： 默认增大平动的控制性能
    double MAX_TRANSLATION_SPEED = 4000;		// [unit : cm/s]
    double MAX_TRANSLATION_ACC = 6000;		// [unit : cm/s2]
    double MAX_ROTATION_SPEED = 5;			// [unit : rad/s]
    double MAX_ROTATION_ACC = 15;			// [unit : rad/s2]
    double TRANSLATION_ACC_LIMIT = 10000;
    double MAX_TRANSLATION_DEC = 6500;

    /// 守门员专用
    double MAX_TRANSLATION_SPEED_GOALIE;
    double MAX_TRANSLATION_ACC_GOALIE;
    double MAX_TRANSLATION_DEC_GOALIE;
    double MAX_ROTATION_ACC_GOALIE;
    double MAX_ROTATION_SPEED_GOALIE;

    /// 后卫专用
    double MAX_TRANSLATION_SPEED_BACK;
    double MAX_TRANSLATION_ACC_BACK;
    double MAX_TRANSLATION_DEC_BACK;
    double MAX_ROTATION_ACC_BACK;
    double MAX_ROTATION_SPEED_BACK;

    /// 路径规划
    int PATH_PLAN_TYPE = 0;  /* RRT = 0,GEO = 1,ASTAR = 2,STUPID = 3, BANGBANG = 4, PFAD = 5,DELAUNAY = 6,NEWGEO = 7,*/

    const double safeVelFactorFront = 15;
    const double safeVelFactorBack = 20;

    bool DRAW_TRAJ = false;		//Draw
    bool DRAW_PREDICT_POS = false;
    bool DRAW_TMP_ADDED = false;
    bool DRAW_OBSTACLES = false;

    /// related to rrt
    double startToRotateToTargetDirDist = 100;
    PathList middleList;
    CGeoPoint lastPoint[PARAM::Field::MAX_PLAYER];
    CGeoPoint lastFinalPoint[PARAM::Field::MAX_PLAYER];
    CGeoPoint veryStart[PARAM::Field::MAX_PLAYER];
    bool isExecuting[PARAM::Field::MAX_PLAYER];
    const double TEAMMATE_AVOID_DIST = PARAM::Vehicle::V2::PLAYER_SIZE + 40.0f; // 2014/03/13 修改，为了减少stop的时候卡住的概率 yys
    const double OPP_AVOID_DIST = PARAM::Vehicle::V2::PLAYER_SIZE + 55.0f;
    const double BALL_AVOID_DIST = PARAM::Field::BALL_SIZE + 50.0f;
}
using namespace PARAM::Vehicle::V2;

/************************************************************************/
/*                       CSmartGotoPositionV2                           */
/************************************************************************/
/// 构造函数 ： 参数初始化
CSmartGotoPositionV2::CSmartGotoPositionV2()
{
    ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_SPEED,"CGotoPositionV2/MNormalSpeed",3000);
    ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_ACC,"CGotoPositionV2/MNormalAcc",4500);
    ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_DEC,"CGotoPositionV2/MNormalDec",4500);
    ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_SPEED_BACK,"CGotoPositionV2/MBackSpeed",3000);
    ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_ACC_BACK,"CGotoPositionV2/MBackAcc",4500);
    ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_DEC_BACK,"CGotoPositionV2/MBackDec",4500);

    ZSS::ZParamManager::instance()->loadParam(MAX_ROTATION_ACC_BACK,"CGotoPositionV2/MBackRotateAcc",15);
    ZSS::ZParamManager::instance()->loadParam(MAX_ROTATION_SPEED_BACK,"CGotoPositionV2/MBackRotateSpeed",15);

    ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_SPEED_GOALIE,"CGotoPositionV2/MGoalieSpeed",3000);
    ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_ACC_GOALIE,"CGotoPositionV2/MGoalieAcc",4500);
    ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_DEC_GOALIE,"CGotoPositionV2/MGoalieDec",4500);

    ZSS::ZParamManager::instance()->loadParam(MAX_ROTATION_ACC_GOALIE,"CGotoPositionV2/MGoalieRotateAcc",150);
    ZSS::ZParamManager::instance()->loadParam(MAX_ROTATION_SPEED_GOALIE,"CGotoPositionV2/MGoalieRotateSpeed",150);

    ZSS::ZParamManager::instance()->loadParam(MAX_ROTATION_SPEED,"CGotoPositionV2/RotationSpeed",15);
    ZSS::ZParamManager::instance()->loadParam(MAX_ROTATION_ACC,"CGotoPositionV2/RotationAcc",15);
    ZSS::ZParamManager::instance()->loadParam(DRAW_TARGET,"Debug/SmartTargetPos",true);
    ZSS::ZParamManager::instance()->loadParam(DRAW_OBSTACLES, "Debug/DrawObst", false);
    ZSS::ZParamManager::instance()->loadParam(PATH_PLAN_TYPE,"CSmartGotoV2/PathPlanType",0);
    // std::cout << "debug111 ctor of smartgoto" << std::endl;
}

/// 输出流 ： 调试显示
void CSmartGotoPositionV2::toStream(std::ostream& os) const
{
    os << "Smart going to " << task().player.pos;
}

/// 规划接口
void CSmartGotoPositionV2::plan(const CVisionModule* pVision)
{
    if ((lastFinalPoint[task().executor] - task().player.pos).mod() > 50) {
        lastPoint[task().executor] = task().player.pos; /// added by gjy 5.29
        lastFinalPoint[task().executor] = task().player.pos;
        veryStart[task().executor] = pVision->ourPlayer(task().executor).Pos();
        isExecuting[task().executor] = false;
    }
    /************************************************************************/
    /*              任务参数解析                                             */
	/************************************************************************/
	playerFlag = task().player.flag;
	const int vecNumber = task().executor;
	const PlayerVisionT& self = pVision->ourPlayer(vecNumber);

    const bool isGoalie = vecNumber == TaskMediator::Instance()->goalie();
    const bool isBack = (vecNumber == TaskMediator::Instance()->leftBack())
                        || (vecNumber == TaskMediator::Instance()->rightBack());
    bool avoidBallCircle = (WorldModel::Instance()->CurrentRefereeMsg() == "GameStop") || (WorldModel::Instance()->CurrentRefereeMsg() == "TheirKickOff");
    CGeoPoint myPos = self.Pos();
    CGeoPoint finalTargetPos = task().player.pos;

    GDebugEngine::Instance()->gui_debug_x(finalTargetPos, COLOR_RED);
    const bool needBreakRotate = (playerFlag & PlayerStatus::BREAK_THROUGH);
    bool onlyPlanInCircle = task().player.path_plan_in_circle;
    CGeoPoint planCircleCenter = task().player.path_plan_circle_center;
    double planCircleRadius = task().player.path_plan_circle_radius;
    if(onlyPlanInCircle && !Utils::IsInField(planCircleCenter, -400)) {
        cout << "Oh shit!!! PathPlan circle center not initialized!!! ---SmartGotoPosition.cpp" << endl;
        onlyPlanInCircle = false;
    }  else if(onlyPlanInCircle && planCircleRadius < 1e-8) {
        cout << "Oh shit!!! PathPlan circle radius is too small!!! ---SmartGotoPosition.cpp" << endl;
        onlyPlanInCircle = false;
    }

    _capability = setCapability(pVision);

    /************************************************************************/
    /* 任务参数解析                                                         */
    /************************************************************************/

    /// 路径规划
    double buffer = 25;
    PlanType play_type = (PlanType)PATH_PLAN_TYPE;
    if (self.Vel().mod() < 2000) {
        buffer = 25;
    }
    else if (self.Vel().mod() < 3500) {
        buffer = 25 + (self.Vel().mod() - 2000) / 500;
    }
    else {
        buffer = 55;
    }
    if (self.Vel().mod() < 1500 && (self.Pos() - finalTargetPos).mod() < 1000) {
        buffer /= 2;
    }
    if (isGoalie ||
            isBack ||
            WorldModel::Instance()->CurrentRefereeMsg() == "OurTimeout" ||
            playerFlag & PlayerStatus::BREAK_THROUGH) {
        buffer = 0;
        play_type = RRT;
    }

    double avoidLength = PARAM::Vehicle::V2::PLAYER_SIZE + buffer;

    obstacles obs(avoidLength);
    if(!(playerFlag & PlayerStatus::BREAK_THROUGH))
        obs.addObs(pVision, task(), DRAW_OBSTACLES, OPP_AVOID_DIST, TEAMMATE_AVOID_DIST, std::max(task().ball.avoid_dist, BALL_AVOID_DIST));
    else
        obs.addObs(pVision, task(), DRAW_OBSTACLES, PARAM::Vehicle::V2::PLAYER_SIZE + PARAM::Field::BALL_SIZE + 20, PARAM::Vehicle::V2::PLAYER_SIZE + PARAM::Field::BALL_SIZE + 20.0, PARAM::Field::BALL_SIZE);

    if(WorldModel::Instance()->CurrentRefereeMsg() == "TheirBallPlacement") {
        CGeoPoint segP1 = pVision->ball().Pos(), segP2(RefereeBoxInterface::Instance()->getBallPlacementPosX(), RefereeBoxInterface::Instance()->getBallPlacementPosY());
        bool addObs;
        finalTargetPos = finalPointBallPlacement(myPos, finalTargetPos, segP1, segP2, avoidLength, addObs);
        if(addObs)
            obs.add_long_circle(vector2f(segP1.x(), segP1.y()), vector2f(segP2.x(), segP2.y()), vector2f(0.0, 0.0), 500.0, 1, true);
    }

    //处理无效目标点:在禁区内、在车身内、在场地外
    validateFinalTarget(finalTargetPos, myPos, avoidLength, isGoalie, avoidBallCircle, obs, onlyPlanInCircle, planCircleCenter, planCircleRadius);

    validateStartPoint(myPos, avoidLength, isGoalie, obs);
    GDebugEngine::Instance()->gui_debug_x(finalTargetPos, COLOR_YELLOW);
    GDebugEngine::Instance()->gui_debug_x(myPos, COLOR_PURPLE);

    CGeoPoint middlePoint = finalTargetPos; // 初始点
    /// modified the condition on which rrt would be executed 2018.5.29 gjy
    state start;
    start.pos = vector2f(myPos.x(), myPos.y());
    state middle;
    middle.pos = vector2f(lastPoint[vecNumber].x(), lastPoint[vecNumber].y());
    state final;
    final.pos = vector2f(finalTargetPos.x(), finalTargetPos.y());


    TaskT newTask(task());
    newTask.player.pos = finalTargetPos;

    if(self.Pos().dist(finalTargetPos) < startToRotateToTargetDirDist && needBreakRotate)
        newTask.player.flag &= (!PlayerStatus::BREAK_THROUGH);

    double arrivedDist = self.Vel().mod() * 0.1 + 50;
    if (obs.check(start, final)) { // the robot can directly head to the final pos
        middlePoint = finalTargetPos;
    }
    else if (obs.check(start, middle) && (lastPoint[vecNumber] - self.Pos()).mod() > arrivedDist) { // keep the pos last cycle
        middlePoint = lastPoint[vecNumber];
    }
    else { // replan the path
        CPathPlanner planner(pVision, newTask, obs, play_type, myPos, onlyPlanInCircle, planCircleCenter, planCircleRadius);
        middleList = planner.getPath();
        if (middleList.size() > 0) { // 规划成功
            CGeoPoint plannedPoint(*(planner.getPath().begin()));
            middlePoint = plannedPoint;
        }
    }

    if(!isGoalie && !(playerFlag & PlayerStatus::NOT_AVOID_PENALTY)) {
        middlePoint = dealPlanFail(self.Pos(), middlePoint, avoidLength);
    }

    newTask.player.pos = middlePoint;
    if(middlePoint.dist(task().player.pos) > 1e-8) {
        newTask.player.vel = CVector(0.0, 0.0);
    }
    lastPoint[vecNumber] = middlePoint;


    if(USE_DEBUG) cout << "[SmartGotoPosition.cpp] GotoPosition" << endl;
    // setSubTask(TaskFactoryV2::Instance()->GotoPosition(newTask));
    setSubTask("Goto", newTask);

    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
    return ;
}

double CSmartGotoPositionV2::getMiddleVel(const CVisionModule *pVision, CGeoPoint middleProj) {
    int vecNum = task().executor;
    CGeoSegment start2FinalLine = CGeoSegment(veryStart[vecNum], lastFinalPoint[vecNum]);
    if (start2FinalLine.IsPointOnLineOnSegment(middleProj)) {
        double velMod;
        double middleDist = (middleProj - veryStart[vecNum]).mod();
        double finalDist = (lastFinalPoint[vecNum] - veryStart[vecNum]).mod();
        double triangleDist = pow(_capability.maxSpeed, 2) / _capability.maxAccel;
        if (finalDist < triangleDist) {
            double maxVel = sqrt(_capability.maxAccel * finalDist);
            triangleDist = pow(maxVel, 2) / _capability.maxAccel;
            velMod = middleDist < triangleDist / 2 ? sqrt(2 * _capability.maxAccel * middleDist) :
                                                     sqrt(2 * _capability.maxAccel * (finalDist - middleDist));
        }
        else {
            double flatDist = finalDist - triangleDist;
            velMod = middleDist < flatDist + triangleDist / 2 ?
                        _capability.maxSpeed : sqrt(2 * _capability.maxAccel * (triangleDist / 2));
        }
        return velMod * 0.3;
    }
    else {
        return 0;
    }
}

PlayerCapabilityT CSmartGotoPositionV2::setCapability(const CVisionModule* pVision) {
    const int vecNumber = task().executor;
    const int playerFlag = task().player.flag;
    PlayerCapabilityT capability;

    // Traslation 确定平动运动参数
    if (vecNumber == TaskMediator::Instance()->goalie()) {
        capability.maxSpeed = MAX_TRANSLATION_SPEED_GOALIE;
        capability.maxAccel = MAX_TRANSLATION_ACC_GOALIE;
        capability.maxDec = MAX_TRANSLATION_DEC_GOALIE;
        // Rotation	  确定转动运动参数
        capability.maxAngularSpeed = MAX_ROTATION_SPEED_GOALIE;
        capability.maxAngularAccel = MAX_ROTATION_ACC_GOALIE;
        capability.maxAngularDec = MAX_ROTATION_ACC_GOALIE;
    }
    else if (TaskMediator::Instance()->leftBack() != 0 && vecNumber == TaskMediator::Instance()->leftBack()
        || TaskMediator::Instance()->rightBack() != 0 && vecNumber == TaskMediator::Instance()->rightBack()
        || TaskMediator::Instance()->singleBack() != 0 && vecNumber == TaskMediator::Instance()->singleBack()
        || TaskMediator::Instance()->sideBack() != 0 && vecNumber == TaskMediator::Instance()->sideBack()) {
        capability.maxSpeed = MAX_TRANSLATION_SPEED_BACK;
        capability.maxAccel = MAX_TRANSLATION_ACC_BACK;
        capability.maxDec = MAX_TRANSLATION_DEC_BACK;
        // Rotation	  确定转动运动参数
        capability.maxAngularSpeed = MAX_ROTATION_SPEED_BACK;
        capability.maxAngularAccel = MAX_ROTATION_ACC_BACK;
        capability.maxAngularDec = MAX_ROTATION_ACC_BACK;
    }
    else {
        capability.maxSpeed = MAX_TRANSLATION_SPEED;
        capability.maxAccel = MAX_TRANSLATION_ACC;
        capability.maxDec = MAX_TRANSLATION_DEC;
        // Rotation	  确定转动运动参数
        capability.maxAngularSpeed = MAX_ROTATION_SPEED;
        capability.maxAngularAccel = MAX_ROTATION_ACC;
        capability.maxAngularDec = MAX_ROTATION_ACC;
    }

    if (task().player.max_acceleration > 1e-8) { // 2014-03-26 修改, 因为double数不能进行相等判断
        capability.maxAccel = task().player.max_acceleration > TRANSLATION_ACC_LIMIT ? TRANSLATION_ACC_LIMIT : task().player.max_acceleration;
        capability.maxDec = capability.maxAccel;

    }
    /// added rotation acceleration limit by gjy 2018.6.9
    if (task().player.max_rot_acceleration > 1e-8) {
        capability.maxAngularAccel = task().player.max_rot_acceleration;
        capability.maxAngularDec = task().player.max_rot_acceleration;
    }
    if (task().player.max_speed > 1e-8) {
        capability.maxSpeed = task().player.max_speed;
    }
    if (task().player.max_rot_speed > 1e-8) {
        capability.maxAngularSpeed = task().player.max_rot_speed;
    }
    return capability;
}

void CSmartGotoPositionV2::validateFinalTarget(CGeoPoint &finalTarget, CGeoPoint myPos, double avoidLength, bool isGoalie, bool avoidBallCircle, obstacles& obs, bool planInCircle, CGeoPoint circleCenter, double circleRadius) {
    if(!(playerFlag & PlayerStatus::NOT_AVOID_PENALTY)) {
        finalTarget = Utils::MakeInField(finalTarget, -avoidLength * 2);
        if(!isGoalie && Utils::InOurPenaltyArea(finalTarget, avoidLength))
            finalTarget = Utils::MakeOutOfOurPenaltyArea(finalTarget, avoidLength);
        if(Utils::InTheirPenaltyArea(finalTarget, avoidLength))
            finalTarget = Utils::MakeOutOfTheirPenaltyArea(finalTarget, avoidLength);
    }
    if(planInCircle && circleCenter.dist(finalTarget) > circleRadius) {
        CVector targetDir = finalTarget - circleCenter;
        finalTarget = circleCenter + targetDir / targetDir.mod() * (circleRadius - 1e-8);
    }
    int obsNum = obs.get_num();
    state goal;

    goal.pos = vector2f(finalTarget.x(), finalTarget.y());

    for(int i = 0; i < obsNum; i++) {
        if(obs.obs[i].type != OBS_RECTANGLE && !obs.obs[i].check(goal)) {
            if(obs.obs[i].type == OBS_CIRCLE) {
                if(!planInCircle) {
                    if((avoidBallCircle && obs.obs[i].radius == 500) || finalTarget.dist(myPos) > (avoidLength + obs.obs[i].radius) * 1.2) {
                        finalTarget = Utils::MakeOutOfCircle(CGeoPoint(obs.obs[i].seg_start.x, obs.obs[i].seg_start.y), obs.obs[i].radius, finalTarget, avoidLength);
                    }
                    else {
                        finalTarget = Utils::MakeOutOfCircle(CGeoPoint(obs.obs[i].seg_start.x, obs.obs[i].seg_start.y), PARAM::Vehicle::V2::PLAYER_FRONT_TO_CENTER, finalTarget, PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER);
                    }
                } else {
                    CVector centerToTarget = finalTarget - circleCenter;
                    CVector centerToObs = CGeoPoint(obs.obs[i].seg_start.x, obs.obs[i].seg_start.y) - circleCenter;
                    double rotateDir = centerToTarget.dir() - centerToObs.dir();
                    rotateDir /= fabs(rotateDir);
                    double rotateStep = asin(obs.obs[i].radius / centerToObs.mod());
                    finalTarget = circleCenter + centerToTarget.rotate(rotateStep);
                }
            }
            else {
                if((avoidBallCircle && obs.obs[i].radius == 500) || finalTarget.dist(myPos) > (avoidLength + obs.obs[i].radius) * 1.2){
                    finalTarget = Utils::MakeOutOfLongCircle(CGeoPoint(obs.obs[i].seg_start.x, obs.obs[i].seg_start.y), CGeoPoint(obs.obs[i].seg_end.x, obs.obs[i].seg_end.y), obs.obs[i].radius, finalTarget, avoidLength);
                } else {
                    finalTarget = Utils::MakeOutOfLongCircle(CGeoPoint(obs.obs[i].seg_start.x, obs.obs[i].seg_start.y), CGeoPoint(obs.obs[i].seg_end.x, obs.obs[i].seg_end.y), PARAM::Vehicle::V2::PLAYER_FRONT_TO_CENTER, finalTarget, PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER);
                }
            }
        }
    }

}

void CSmartGotoPositionV2::validateStartPoint(CGeoPoint &startPoint, double avoidLength, bool isGoalie, obstacles &obs) {
    if(!(playerFlag & PlayerStatus::NOT_AVOID_PENALTY)) {
        if(!isGoalie && Utils::InOurPenaltyArea(startPoint, avoidLength))
            startPoint = Utils::MakeOutOfOurPenaltyArea(startPoint, avoidLength);
        if(Utils::InTheirPenaltyArea(startPoint, avoidLength))
            startPoint = Utils::MakeOutOfTheirPenaltyArea(startPoint, avoidLength);
    }

    int obsNum = obs.get_num();
    state goal;
    goal.pos = vector2f(startPoint.x(), startPoint.y());
    for(int i = 0; i < obsNum; i++) {
        if(obs.obs[i].type != OBS_RECTANGLE && !obs.obs[i].check(goal)) {
            if(obs.obs[i].type == OBS_CIRCLE) {
                startPoint = Utils::MakeOutOfCircle(CGeoPoint(obs.obs[i].seg_start.x, obs.obs[i].seg_start.y), obs.obs[i].radius,  startPoint, avoidLength);
            }
            else {
                startPoint = Utils::MakeOutOfLongCircle(CGeoPoint(obs.obs[i].seg_start.x, obs.obs[i].seg_start.y), CGeoPoint(obs.obs[i].seg_end.x, obs.obs[i].seg_end.y), obs.obs[i].radius, startPoint, avoidLength);
            }
        }
    }
}

//考虑到点后的速度对末位置的漂移影响； 速度在垂直于轨迹线方向对初始位置的影响
CVector CSmartGotoPositionV2::validateFinalVel(const bool isGoalie, const CGeoPoint& startPos,const CGeoPoint &targetPos, const CVector &targetVel) {

    if(targetVel.mod() < 1e-8) return CVector(0.0, 0.0);
    double distToStop, velMod, adjustStep, factorFront, factorBack, distToTarget, maxFinalSpeedAccordingToDist;
    CGeoPoint stopPosFront, stopPosBack;
    CVector velNew = targetVel;
    CVector velDir = targetVel / targetVel.mod();
    distToTarget = startPos.dist(targetPos);

    if(distToTarget < 1e-8 || _capability.maxAccel < 1e-8)
        return CVector(0.0, 0.0);
    maxFinalSpeedAccordingToDist = sqrt(2 * _capability.maxAccel * distToTarget * 0.5);
    if(maxFinalSpeedAccordingToDist > _capability.maxSpeed)
        maxFinalSpeedAccordingToDist = _capability.maxSpeed;


    if(targetVel.mod() > maxFinalSpeedAccordingToDist)
        velNew = velDir * maxFinalSpeedAccordingToDist;
    velMod = velNew.mod();
    factorFront = safeVelFactorFront;
    factorBack = safeVelFactorBack;
    adjustStep = 50;

    if(_capability.maxDec < 1e-8) return CVector(0.0, 0.0);
    distToStop = pow(targetVel.mod(), 2) / (2.0 * _capability.maxDec);
    stopPosFront = targetPos + velDir * distToStop * factorFront;
    stopPosBack = targetPos + velDir * (-distToStop) * factorBack;

    while(!Utils::IsInField(stopPosFront, PARAM::Vehicle::V2::PLAYER_SIZE) ||
          Utils::InTheirPenaltyArea(stopPosFront, PARAM::Vehicle::V2::PLAYER_SIZE) ||
          (!isGoalie && Utils::InOurPenaltyArea(stopPosFront, PARAM::Vehicle::V2::PLAYER_SIZE)) ||
          !Utils::IsInField(stopPosBack, PARAM::Vehicle::V2::PLAYER_SIZE) ||
            Utils::InTheirPenaltyArea(stopPosBack, PARAM::Vehicle::V2::PLAYER_SIZE) ||
            (!isGoalie && Utils::InOurPenaltyArea(stopPosBack, PARAM::Vehicle::V2::PLAYER_SIZE))) {
        velMod -= adjustStep;
        if(velMod < 0) return CVector(0.0, 0.0);
        velNew = velDir * velMod;
        if(_capability.maxSpeed > 1e-8 && safeVelFactorFront > 1.0)
            factorFront = velNew.mod() * (safeVelFactorFront - 1.0) / _capability.maxSpeed + 1.0;
        if(_capability.maxSpeed > 1e-8 && safeVelFactorBack > 1.0)
            factorBack = velNew.mod() * (safeVelFactorBack - 1.0) / _capability.maxSpeed + 1.0;
        factorFront = (factorFront >= 1.0) ? factorFront : 1.0;
        factorBack = (factorBack >= 1.0) ? factorBack : 1.0;
        if(_capability.maxDec < 1e-8) return CVector(0.0, 0.0);
        distToStop = pow(velNew.mod(), 2) / (2.0 * _capability.maxDec);
        stopPosFront = targetPos + velDir * distToStop * factorFront;
        stopPosBack = targetPos + velDir * (-distToStop) * factorBack;
    }
    return velNew;
}

CGeoPoint CSmartGotoPositionV2::dealPlanFail(CGeoPoint startPoint, CGeoPoint nextPoint, double avoidLength) {

    CGeoRectangle theirPenalty(CGeoPoint(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH, -PARAM::Field::PENALTY_AREA_WIDTH/2), CGeoPoint(PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::PENALTY_AREA_WIDTH / 2));
    CGeoRectangle ourPenalty(CGeoPoint(-PARAM::Field::PITCH_LENGTH / 2, -PARAM::Field::PENALTY_AREA_WIDTH / 2), CGeoPoint(-PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH, PARAM::Field::PENALTY_AREA_WIDTH / 2));
    CGeoLine pathLine(startPoint, nextPoint);
    CGeoLineRectangleIntersection theirInter(pathLine, theirPenalty);
    CGeoLineRectangleIntersection ourInter(pathLine, ourPenalty);
    CGeoPoint nextPointNew = nextPoint;
    CGeoPoint candidate11 = CGeoPoint(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH - avoidLength - OPP_AVOID_DIST, PARAM::Field::PENALTY_AREA_WIDTH / 2 + avoidLength + OPP_AVOID_DIST),
            candidate12  = CGeoPoint(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH - avoidLength - OPP_AVOID_DIST, PARAM::Field::PENALTY_AREA_WIDTH / 2 - avoidLength - OPP_AVOID_DIST),
            candidate21 = CGeoPoint(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH - avoidLength - OPP_AVOID_DIST, -PARAM::Field::PENALTY_AREA_WIDTH / 2 - avoidLength - OPP_AVOID_DIST),
            candidate22 = CGeoPoint(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH - avoidLength - OPP_AVOID_DIST, -PARAM::Field::PENALTY_AREA_WIDTH / 2 + avoidLength + OPP_AVOID_DIST),
            candidate31 = CGeoPoint(-PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH + avoidLength + OPP_AVOID_DIST, PARAM::Field::PENALTY_AREA_WIDTH / 2 + avoidLength + OPP_AVOID_DIST),
            candidate32 = CGeoPoint(-PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH + avoidLength + OPP_AVOID_DIST, PARAM::Field::PENALTY_AREA_WIDTH / 2 - avoidLength - OPP_AVOID_DIST),
            candidate41 = CGeoPoint(-PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH + avoidLength + OPP_AVOID_DIST, -PARAM::Field::PENALTY_AREA_WIDTH / 2 - avoidLength - OPP_AVOID_DIST),
            candidate42 = CGeoPoint(-PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH + avoidLength + OPP_AVOID_DIST, -PARAM::Field::PENALTY_AREA_WIDTH / 2 + avoidLength + OPP_AVOID_DIST);
//    GDebugEngine::Instance()->gui_debug_x(candidate11);
//    GDebugEngine::Instance()->gui_debug_x(candidate21);
//    GDebugEngine::Instance()->gui_debug_x(candidate31);
//    GDebugEngine::Instance()->gui_debug_x(candidate41);
//    GDebugEngine::Instance()->gui_debug_x(candidate12);
//    GDebugEngine::Instance()->gui_debug_x(candidate22);
//    GDebugEngine::Instance()->gui_debug_x(candidate32);
//    GDebugEngine::Instance()->gui_debug_x(candidate42);
//    GDebugEngine::Instance()->gui_debug_msg(candidate11, QString("11").toLatin1());
//    GDebugEngine::Instance()->gui_debug_msg(candidate12, QString("12").toLatin1());
//    GDebugEngine::Instance()->gui_debug_msg(candidate21, QString("21").toLatin1());
//    GDebugEngine::Instance()->gui_debug_msg(candidate22, QString("22").toLatin1());
//    GDebugEngine::Instance()->gui_debug_msg(candidate31, QString("31").toLatin1());
//    GDebugEngine::Instance()->gui_debug_msg(candidate32, QString("32").toLatin1());
//    GDebugEngine::Instance()->gui_debug_msg(candidate41, QString("41").toLatin1());
//    GDebugEngine::Instance()->gui_debug_msg(candidate42, QString("42").toLatin1());

    if(theirInter.intersectant() || ourInter.intersectant()) {
//        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0.0, 40.0), QString("pass penalty").toLatin1());
        if(!((startPoint.y() > PARAM::Field::PENALTY_AREA_WIDTH / 2 && nextPoint.y() > PARAM::Field::PENALTY_AREA_WIDTH / 2 ) ||
             (startPoint.y() < -PARAM::Field::PENALTY_AREA_WIDTH / 2 && nextPoint.y() < -PARAM::Field::PENALTY_AREA_WIDTH / 2) ||
             (startPoint.y() < PARAM::Field::PENALTY_AREA_WIDTH / 2 && startPoint.y() > -PARAM::Field::PENALTY_AREA_WIDTH / 2 && nextPoint.y() < PARAM::Field::PENALTY_AREA_WIDTH / 2 && nextPoint.y() > -PARAM::Field::PENALTY_AREA_WIDTH / 2))) {

            if(startPoint.x() > PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH) {
                if(startPoint.y() > PARAM::Field::PENALTY_AREA_WIDTH / 2) {
                    nextPointNew = candidate11;
                    if(nextPoint.dist(candidate12) < nextPoint.dist(candidate12) && startPoint.dist(candidate11) < 5)
                        nextPointNew = candidate12;
                } else {
                    nextPointNew = candidate21;
                    if(nextPoint.dist(candidate22) < nextPoint.dist(candidate22) && startPoint.dist(candidate21) < 5)
                        nextPointNew = candidate22;
                }

            }
            else if(startPoint.x() < -PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH) {
                if(startPoint.y() > PARAM::Field::PENALTY_AREA_WIDTH / 2) {
                    nextPointNew = candidate31;
                    if(nextPoint.dist(candidate32) < nextPoint.dist(candidate32) && startPoint.dist(candidate31) < 5)
                        nextPointNew = candidate32;
                }
                else {
                    nextPointNew = candidate41;
                    if(nextPoint.dist(candidate42) < nextPoint.dist(candidate42) && startPoint.dist(candidate41) < 5)
                        nextPointNew = candidate42;
                }
            }
            else if(nextPoint.x() > PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH) {
                if(nextPoint.y() > PARAM::Field::PENALTY_AREA_WIDTH / 2) {
                    nextPointNew = candidate12;
                    if(nextPoint.dist(candidate11) < nextPoint.dist(candidate12) && startPoint.dist(candidate12) < 5)
                        nextPointNew = candidate11;
                }
                else {
                    nextPointNew = candidate22;
                    if(nextPoint.dist(candidate21) < nextPoint.dist(candidate22) && startPoint.dist(candidate22) < 5)
                        nextPointNew = candidate21;
                }
            }
            else if(nextPoint.x() < -PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH) {
                if(nextPoint.y() > PARAM::Field::PENALTY_AREA_WIDTH / 2) {
                    nextPointNew = candidate32;
                    if(nextPoint.dist(candidate31) < nextPoint.dist(candidate32) && startPoint.dist(candidate32) < 5)
                        nextPointNew = candidate31;
                }
                else {
                    nextPointNew = candidate42;
                    if(nextPoint.dist(candidate41) < nextPoint.dist(candidate42) && startPoint.dist(candidate42) < 5)
                        nextPointNew = candidate41;
                }
            }
        }
    }

    return nextPointNew;
}

CGeoPoint CSmartGotoPositionV2::finalPointBallPlacement(const CGeoPoint &startPoint, const CGeoPoint &target, const CGeoPoint &segPoint1, const CGeoPoint &segPoint2, const double avoidLength, bool& addObs) {
    CGeoPoint targetNew = target;
    CGeoSegment obsSegment(segPoint1, segPoint2);
    double obsRadius = 50.0;
    CVector segVec = segPoint1 - segPoint2;
    CVector adjustDir = segVec.rotate(PARAM::Math::PI / 2).unit();
    double adjustStep = 5.0;
    addObs = false;

    ///情况1：自己和目标都在区域内，把自己移出区域并作为新的目标点；
    if(obsSegment.dist2Point(startPoint) < obsRadius + avoidLength && obsSegment.dist2Point(target) < obsRadius + avoidLength) {
        GDebugEngine::Instance()->gui_debug_msg(startPoint, QString("Me In, Target In").toLatin1(), COLOR_ORANGE);
        targetNew = startPoint;
        while (obsSegment.dist2Point(targetNew) < obsRadius + avoidLength)
            targetNew = targetNew + adjustDir * adjustStep;
        if(targetNew.dist(startPoint) > obsRadius + avoidLength)
            targetNew = targetNew + (-adjustDir) * (2 * (obsRadius + avoidLength + adjustStep));
        return targetNew;
    }

    ///情况2： 自己在区域外，目标在区域内，将目标移动到己方区域边界
    if(obsSegment.dist2Point(startPoint) > obsRadius + avoidLength && obsSegment.dist2Point(target) < obsRadius + avoidLength) {
        GDebugEngine::Instance()->gui_debug_msg(startPoint, QString("Me Out, Target In").toLatin1(), COLOR_ORANGE);
        targetNew = target;
        while(obsSegment.dist2Point(targetNew) < obsRadius + avoidLength)
            targetNew = targetNew + adjustDir * adjustStep;
        if(obsSegment.dist2Segment(CGeoSegment(startPoint, targetNew)) < obsRadius + avoidLength)
            targetNew = targetNew + (-adjustDir) * (2 * (obsRadius + avoidLength + adjustStep));
        return targetNew;
    }

    ///情况3：自己和目标点都在区域外，且分居区域两侧，将自己尽可能向靠近目标点的区域外同侧移动
    if(obsSegment.dist2Point(startPoint) > obsRadius + avoidLength && obsSegment.dist2Point(target) > obsRadius + avoidLength && obsSegment.dist2Segment(CGeoSegment(startPoint, target)) < obsRadius + avoidLength) {
        GDebugEngine::Instance()->gui_debug_msg(startPoint, QString("Me Out, Target Out").toLatin1(), COLOR_ORANGE);
        targetNew = startPoint;
        CVector adjustDir2 = segVec.unit();
        CVector me2Target = target - startPoint;
        if(fabs(fabs(Utils::Normalize(me2Target.dir() - adjustDir2.dir())) - PARAM::Math::PI / 2) < PARAM::Math::PI / 9) return target;
        if(adjustDir2 * me2Target < 0)
            adjustDir2 = -adjustDir2;
        while(me2Target * adjustDir2 > 0) {
            targetNew = targetNew + adjustDir2 * adjustStep;
            me2Target = target - targetNew;

        }
        if(obsSegment.dist2Point(targetNew) > obsRadius + avoidLength + adjustStep) {
            if(obsSegment.dist2Point(targetNew + adjustDir) < obsSegment.dist2Point(targetNew))
                targetNew = targetNew + adjustDir * (obsSegment.dist2Point(targetNew) - obsRadius - avoidLength - adjustStep);
            else
                targetNew = targetNew + (-adjustDir) * (obsSegment.dist2Point(targetNew) - obsRadius - avoidLength - adjustStep);
        }
        addObs = true;
        return targetNew;
    }

    ///情况4：自己在区域内，目标在区域外，新目标点为目标点向自己方向移动的区域边界对应点
    if(obsSegment.dist2Point(startPoint) < obsRadius + avoidLength && obsSegment.dist2Point(target) > obsRadius + avoidLength) {
        GDebugEngine::Instance()->gui_debug_msg(startPoint, QString("Me In, Target Out").toLatin1(), COLOR_ORANGE);
        targetNew = target;
        CVector adjustDir2 = segVec.unit();
        CVector target2Me = startPoint - targetNew;
        if(fabs(fabs(Utils::Normalize(target2Me.dir() - adjustDir2.dir())) - PARAM::Math::PI / 2) < PARAM::Math::PI / 9) return target;
        if(adjustDir2 * target2Me < 0)
            adjustDir2 = -adjustDir2;
        while(target2Me * adjustDir2 > 0) {
            targetNew = targetNew + adjustDir2 * adjustStep;
            target2Me = startPoint - targetNew;
        }
        if(obsSegment.dist2Point(targetNew) > obsRadius + avoidLength + adjustStep) {
            if(obsSegment.dist2Point(targetNew + adjustDir) < obsSegment.dist2Point(targetNew))
                targetNew = targetNew + adjustDir * (obsSegment.dist2Point(targetNew) - obsRadius - avoidLength - adjustStep);
            else
                targetNew = targetNew + (-adjustDir) * (obsSegment.dist2Point(targetNew) - obsRadius - avoidLength - adjustStep);
        }
        return targetNew;
    }

    return targetNew;
}

