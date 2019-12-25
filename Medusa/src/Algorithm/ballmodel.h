#ifndef BallModel_H
#define BallModel_H

#include "geometry.h"
#include "parammanager.h"
#include "VisionModule.h"
#include "singleton.hpp"
class CBallModel
{

public:
    CBallModel();
    double flatSlideDist(double startVel);
    double flatRollDist(double startVel);
    double flatSlideTime(double startVel);
    double flatRollTime(double startVel);
    double flatStopTime(double startVel);
    // 计算球的速度
    double flatMoveVel(const double initVel, const double time);
    // 计算球移动的距离
    double flatMoveDist(const CVector initVel, const double time);
    CGeoPoint flatPos(CGeoPoint currentPos, CVector startVel, double t);
    double chipToTargetTime(double chipPower, double distToTarget); //chipPower == first jump dist
    double chipSecondJumpDist(double chipPower);
    double chipJumpTime(double chipPower);
    // 计算平射传球力度
    double flatPassVel(const CVisionModule* pVision, CGeoPoint passPos, int receiver, double bufferTime=0, double angleError=0);
    // 计算挑射传球力度
    double chipPassVel(const CVisionModule* pVision, CGeoPoint passPos);
};
typedef Singleton <CBallModel> BallModel;

#endif // BallModel_H
