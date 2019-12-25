#include "ballmodel.h"
#include "CMmotion.h"
#include "staticparams.h"
#include <iostream>
using namespace std;
namespace {
///chip param
const double CHIP_FIRST_ANGLE = 54.29 / 180.0 * PARAM::Math::PI;
const double CHIP_SECOND_ANGLE = 45.59 / 180.0 * PARAM::Math::PI;
const double CHIP_LENGTH_RATIO = 1.266;
const double CHIP_VEL_RATIO = 0.6372;
const double G = 9.8;

///flat param
double ROLL_FRICTION;
double SLIDE_FRICTION;
double SLIDE_BALL_ACC;
double ROLL_BALL_ACC;
const double SLIDE_FACTOR = 15.0;
const double FLAT_SLIDE_RATIO = 2.0 / 7.0;
const double FLAT_ROLL_RATIO = 1 - FLAT_SLIDE_RATIO;
}

CBallModel::CBallModel() {
    bool isSimulation;
    ZSS::ZParamManager::instance()->loadParam(isSimulation, "Alert/IsSimulation", false);
    if(isSimulation)
        ZSS::ZParamManager::instance()->loadParam(ROLL_FRICTION, "AlertParam/Friction4Sim", 1520.0);
    else
        ZSS::ZParamManager::instance()->loadParam(ROLL_FRICTION, "AlertParam/Friction4Real", 800.0);
    SLIDE_FRICTION = SLIDE_FACTOR * ROLL_FRICTION;
    SLIDE_BALL_ACC = SLIDE_FRICTION / 2.0;
    ROLL_BALL_ACC = ROLL_FRICTION / 2.0;
}

double CBallModel::flatRollTime(double startVel) {
    if(startVel < 0.0) {
        cout << "Oh shit!!! Error startVel to calculate roll time!!! ---CBallModel.cpp" << endl;
        return 0.0;
    }
    return (startVel * FLAT_ROLL_RATIO / ROLL_BALL_ACC);
}

double CBallModel::flatSlideTime(double startVel) {
    if(startVel < 0.0) {
        cout << "Oh shit!!! Error startVel to calculate slide time!!! ---CBallModel.cpp" << endl;
        return 0.0;
    }
    return (startVel * FLAT_SLIDE_RATIO / SLIDE_BALL_ACC);
}

double CBallModel::flatSlideDist(double startVel) {
    if(startVel < 0.0) {
        cout << "Oh shit!!! Error startVel to calculate slide distance!!! ---CBallModel.cpp" << endl;
        return 0.0;
    }
    return (pow(startVel, 2.0) - pow(startVel * FLAT_ROLL_RATIO, 2.0)) / (2.0 * SLIDE_BALL_ACC);
}

double CBallModel::flatRollDist(double startVel) {
    if(startVel < 0.0) {
        cout << "Oh shit!!! Error startVel to calculate roll distance!!! ---CBallModel.cpp" << endl;
        return 0.0;
    }
    return pow(startVel * FLAT_ROLL_RATIO, 2.0) / (2.0 * ROLL_BALL_ACC);
}

double CBallModel::flatStopTime(double startVel) {
    return flatRollTime(startVel) + flatSlideTime(startVel);
}

double CBallModel::flatMoveVel(const double initVel, const double time){
    double ballVel = 0;
    double slidingTime = flatSlideTime(initVel);
    double totalTime = flatStopTime(initVel);
    if(time > totalTime) ballVel = 0;
    else if(time < slidingTime) ballVel = initVel - SLIDE_BALL_ACC * time;
    else ballVel = initVel * FLAT_ROLL_RATIO - ROLL_BALL_ACC * (time - slidingTime);
    return ballVel;
}

double CBallModel::flatMoveDist(const CVector initVel, const double time){
    double moveDist = 0;
    double velMod = initVel.mod();
    double slidingTime = flatSlideTime(velMod);
    double totalTime = flatStopTime(velMod);
    if(time > totalTime) {
        moveDist = flatRollDist(velMod) + flatSlideDist(velMod);
    }
    else if(time < slidingTime) {
        moveDist = velMod * time - 0.5 * SLIDE_BALL_ACC * pow(time, 2.0);
    }
    else {
        double slideDist = (pow(velMod, 2.0) - pow(velMod * FLAT_ROLL_RATIO, 2.0)) / (2.0 * SLIDE_BALL_ACC);
        double rollStartVel = velMod * FLAT_ROLL_RATIO;
        double rollTime = time - slidingTime;
        double rollDist = rollStartVel * rollTime - 0.5 * ROLL_BALL_ACC * pow(rollTime, 2.0);
        moveDist = slideDist + rollDist;
    }
    return moveDist;
}

CGeoPoint CBallModel::flatPos(CGeoPoint currentPos, CVector startVel, double t) {
    if(t < 0.0) {
        cout << "Oh shit!!! Error time to calculate ball positon!!! ---CBallModel.cpp" << endl;
        return CGeoPoint(1e8, 1e8);
    }
    double velMod = startVel.mod();
    double moveDist = flatMoveDist(startVel, t);
    startVel = startVel / velMod;
    return (currentPos + startVel * moveDist);
}

double CBallModel::chipToTargetTime(double chipPower, double distToTarget) {
    double chipLength1 = chipPower / 1000.0; //mm->m
    double chipTime1 = sqrt(2.0 * chipLength1 * tan(CHIP_FIRST_ANGLE) / G);
    double chipLength2 = chipSecondJumpDist(chipLength1);
    double chipTime2 = sqrt(2.0 * chipLength2 * tan(CHIP_SECOND_ANGLE) / G);
    chipLength1 *= 100.0;
    chipLength2 *= 100.0;
    double rollDist = distToTarget - chipLength1 - chipLength2;
    double rollVel = pow(chipTime1 * 1000 * G / (2 * sin(CHIP_FIRST_ANGLE)), 2) * CHIP_VEL_RATIO / 980;;
    double time = chipTime1 + chipTime2;
    if(rollDist > 0) {
        double velLeft = pow(rollVel, 2) - 2.0 * ROLL_BALL_ACC * rollDist;
        if(velLeft < 0)
            time += rollVel / ROLL_BALL_ACC;
        else
            time += (rollVel - sqrt(velLeft)) / ROLL_BALL_ACC;
    }
    return time;
}

double CBallModel::chipSecondJumpDist(double chipPower) {
    return (CHIP_LENGTH_RATIO - 1.0) * chipPower;
}

double CBallModel::chipJumpTime(double chipPower) {
    double chipLength1 = chipPower / 1000.0; //mm->m
    double chipTime1 = sqrt(2.0 * chipLength1 * tan(CHIP_FIRST_ANGLE) / G);
    double chipLength2 = chipSecondJumpDist(chipLength1);
    double chipTime2 = sqrt(2.0 * chipLength2 * tan(CHIP_SECOND_ANGLE) / G);
    return chipTime1;
}

double CBallModel::flatPassVel(const CVisionModule *pVision, CGeoPoint passPos, int receiver, double bufferTime, double angleError){
    double passVel = 0;
    CGeoPoint ballPos = pVision->ball().Pos();
    CVector passLine = passPos - ballPos;
    double distance = ballPos.dist(passPos);
    CGeoPoint abnormalPos1 = ballPos + Utils::Polar2Vector(distance, passLine.dir() + angleError*PARAM::Math::PI/180);
    CGeoPoint abnormalPos2 = ballPos + Utils::Polar2Vector(distance, passLine.dir() - angleError*PARAM::Math::PI/180);
    double abnormalTime = std::max(predictedTime(pVision->ourPlayer(receiver), abnormalPos1), predictedTime(pVision->ourPlayer(receiver), abnormalPos2));
    double predictTime = std::max(predictedTime(pVision->ourPlayer(receiver), passPos), abnormalTime);
    double arriveTime = predictTime + bufferTime;
    arriveTime = std::max(arriveTime, 0.4);
    double minPassVel = sqrt(ROLL_FRICTION*distance);
    passVel = std::max((distance + 1/2* ROLL_BALL_ACC * pow(arriveTime, 2))/arriveTime, minPassVel);
    passVel /= FLAT_ROLL_RATIO;
    passVel = std::min(passVel, 5000.0);
    passVel = std::max(passVel, 2000.0);
    return passVel;
}

double CBallModel::chipPassVel(const CVisionModule *pVision, CGeoPoint passPos){
    static const double CHIP_DIST_RATIO = 0.8;
    double passVel = 0;
    CGeoPoint ballPos = pVision->ball().Pos();
    double distance = ballPos.dist(passPos);
    double chipDist = distance * CHIP_DIST_RATIO;
    passVel = chipDist / CHIP_LENGTH_RATIO;
    passVel = std::min(passVel, 4000.0);
    passVel = std::max(passVel, 500.0);
    return passVel;
}
