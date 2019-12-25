#include <iostream>
#include <math.h>
#include <stdio.h>
#include <CMmotion.h>
#include <utils.h>
#include <fstream>
#include "GDebugEngine.h"
#include "parammanager.h"


using namespace std;

namespace {
const double FRAME_PERIOD = 1.0 / PARAM::Vision::FRAME_RATE;

bool DEBUG_NO_ZERO_VEL = false;
const double DEC_FACTOR = 2.0;
const double lowerBoundSpeedLimitRotate = 0;
const double upperBoundSpeedLimitRotate = 300*10;
const double min_max_angle_acc = 10.0;
const double min_max_angle_speed = 10.0;
const double rotateScaleFactor = 1.0;
const double minAngleLimitRotateSpeed = 15.0 / 180.0 * PARAM::Math::PI;

bool IS_SIMULATION = ZSS::ZParamManager::instance()->value("Alert/IsSimulation", QVariant(false)).toBool();
double OUR_MAX_SPEED = ZSS::ZParamManager::instance()->value("CGotoPositionV2/MNormalSpeed",QVariant(300*10)).toDouble();
double OUR_MAX_ACC = ZSS::ZParamManager::instance()->value("CGotoPositionV2/MNormalAcc",QVariant(450*10)).toDouble();
double OUR_MAX_DEC = ZSS::ZParamManager::instance()->value("CGotoPositionV2/MNormalDec",QVariant(450*10)).toDouble();
bool DISPLAY_ROTATION_LIMIT = ZSS::ZParamManager::instance()->value("Debug/RotationLimit",QVariant(false)).toBool();//true;
bool DEBUG_TIME = ZSS::ZParamManager::instance()->value("Debug/TimePredict", QVariant(false)).toBool();
bool addComp = true;
int timeDebugColor = COLOR_GREEN;
int timeItor = 0;
int isX = 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	void compute_motion_1d(double x0, double v0, double v1, double a_max, double v_max,
/// 		double a_factor, double &traj_accel, double &traj_time)
///
/// @brief	一维状态下计算运动过程，给定初始速度和末速度，最大加速度和最大速度，返回加速时间和过程时间
///
/// @author	Song Zhibai( Song Zhibai Zju.edu.cn)
/// @date		2010-4-2
///
/// @param	x0				初始位置
/// @param	v0				初速度
/// @param	v1				末速度
/// @param	a_max			最大加速度
/// @param	v_max			最大速度
/// @param	a_factor		常数，影响加速度的因子
/// @param	traj_accel		计算得加速度
/// @param	traj_time		计算得加速时间
////////////////////////////////////////////////////////////////////////////////////////////////////



void compute_motion_1d(double x0, double v0, double v1,
                       double a_max, double d_max, double v_max, double a_factor, double vel_factor,
                       double &traj_accel, double &traj_time, double &traj_time_acc, double &traj_time_dec, double &traj_time_flat, planType pT, nonZeroMode mode) {
    if (x0 == 0. && v0 == v1) {
        traj_accel = 0;
        if(DEBUG_TIME) GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-380*10 * isX, -270*10 + (timeItor++) * 20*10), QString("R1").toLatin1(), timeDebugColor);
        return;
    }

    if(!finite(x0) || !finite(v0) || !finite(v1)) {
        traj_accel = 0;
        if(DEBUG_TIME) GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-380*10 * isX, -270*10 + (timeItor++) * 20*10), QString("R2").toLatin1(), timeDebugColor);
        return;
    }
    if(pT == MOVE_X && fabs(v1) > 1e-8
            && fabs(v0) < fabs(v1) && v0 * v1 > 0) {
        traj_accel = copysign(a_max, v1);
        return;
    }


    double decFactor = (pT == ROTATE ? 1.0 : DEC_FACTOR);
    a_max /= a_factor;
    d_max /= (1.0 * a_factor);

    double accel_time_to_v1 = fabs(v1 - v0) / a_max;                                                  // 最大加速度加速到末速度的时间
    double accel_dist_to_v1 = fabs((v1 + v0) / 2.0) * accel_time_to_v1;                               // 单一加速到末速度时的位移
    double decel_time_to_v1 = fabs(v0 - v1) / d_max;                                                  // 最大减速度减速到末速度的时间
    double decel_dist_to_v1 = fabs((v0 + v1) / 2.0) * decel_time_to_v1;                               // 单一减速到末速度时的位移

    // 这个时间很关键，设得较大则定位精度将大大降低 by qxz
    double period = 1 / 40.0; // 一段很小的时间，处理运动到目标点附近时加速度，稳定到点，防止超调
    if(pT == MOVE_X)
        period = 1 / 40.0;
    else if(pT == MOVE_Y)
        period = 1.0 / 75.0;
    else
        period = 1 / 40.0;

    if(a_max > 600*10 && pT != MOVE_Y) {
        if(fabs(v0) > 150*10)
            period = 1 / 15.0;
        else if(fabs(v0) > 50*10)
            period = 1 / 20.0;
    }
    else if(a_max > 480*10 && pT != MOVE_Y) {
        if(fabs(v0) > 150*10)
            period = 1 / 25.0;
        else if(fabs(v0) > 50*10)
            period = 1 / 30.0;
    }

    // 计算时间部分
    // 需要先后退再加速非零速到点
    if (fabs(v1) > 1e-8 && v1 * x0 > 0 && mode == ACCURATE) {
        double time_to_accel = fabs(v1) / a_max;
        double x_to_accel = (v1 * v1) / (2.0 * a_max);
        compute_motion_1d(x0 + copysign(x_to_accel, v1), v0, 0, a_max * a_factor, d_max *  a_factor,
                          v_max, a_factor, vel_factor, traj_accel, traj_time, traj_time_acc, traj_time_dec, traj_time_flat, pT, mode);
        traj_time += time_to_accel;
        if(DEBUG_TIME) GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-380*10 * isX, -270*10 + (timeItor++) * 20*10), QString("R3").toLatin1(), timeDebugColor);

        return;
    }
    // 从x0运动到零点
    // 初速和目标点反向 或 初速大于目标速来不及减速到目标速
    // 全程减速
    else if (v0 * x0 > 0 || (fabs(v0) > fabs(v1) && decel_dist_to_v1 > fabs(x0))) {
        // 停下后到达的时间 + 停下所用时间
        double time_to_stop = fabs(v0) / (d_max);                                                       // 停下时间
        double x_to_stop = v0 * v0 / (2.0 * d_max);                                                   // 停止时运动距离
        double time_to_accel = fabs(v1) / a_max;
        double x_to_accel = (v1 * v1) / (2.0 * a_max);

        if (fabs(v1) > 1e-8 && mode == ACCURATE) {
            compute_motion_1d(x0 + copysign(x_to_accel, v1), v0, 0, a_max * a_factor, d_max * a_factor,
                              v_max, a_factor, vel_factor, traj_accel, traj_time, traj_time_acc, traj_time_dec, traj_time_flat, pT, mode);
            traj_time += time_to_accel;
            if(DEBUG_TIME) GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-380*10 * isX, -270*10 + (timeItor++) * 20*10), QString("R4").toLatin1(), timeDebugColor);
            return;
        }
        else {
            compute_motion_1d(x0 + copysign(x_to_stop, v0), 0, v1, a_max * a_factor, d_max * a_factor,
                              v_max, a_factor, vel_factor, traj_accel, traj_time, traj_time_acc, traj_time_dec, traj_time_flat, pT, mode);    // 递归运算直到跳出这一条件
            traj_time += time_to_stop;                                                                    // 加上路径规划时间
            traj_time_dec += time_to_stop;
            // 减速
            if (time_to_stop < period) {
                if(pT == MOVE_Y)
                    traj_accel = compute_stop(v0, a_max);
                else
                    traj_accel = time_to_stop / period * (- copysign(d_max * a_factor, v0)) + (1.0 - time_to_stop / period) * traj_accel;
            }
            else {
                traj_accel = - copysign(decFactor * d_max * a_factor, v0);
            }
            if(DEBUG_TIME) GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-380*10 * isX, -270*10 + (timeItor++) * 20*10), QString("R5").toLatin1(), timeDebugColor);
            return;
        }
    }

    if (accel_dist_to_v1 > fabs(x0) && fabs(v0) < fabs(v1)) {
        traj_time_acc = (sqrt(2 * a_max * fabs(x0) + v0 * v0) - fabs(v0)) / a_max;
        traj_time_flat = 0;
        traj_time_dec = 0;
    } else if (decel_dist_to_v1 > fabs(x0) && fabs(v0) > fabs(v1)) {
        traj_time_acc = 0;
        traj_time_flat = 0;
        traj_time_dec = (fabs(v0) - sqrt(v0 * v0 - 2 * d_max * fabs(x0))) / d_max;
    } else {
        double v_max_dist = (v_max * v_max - v0 * v0) / (2 * a_max) + (v_max * v_max - v1 * v1) / (2 * d_max);
        if (v_max_dist > fabs(x0)) {
            double v_m = sqrt((2 * a_max * d_max * fabs(x0) + d_max * v0 * v0 + a_max * v1 * v1) / (a_max + d_max));
            traj_time_acc = (v_m - fabs(v0)) / a_max;
            traj_time_flat = 0;
            traj_time_dec = (v_m - fabs(v1)) / d_max;
        } else {
            traj_time_acc = (v_max - fabs(v0)) / a_max;
            traj_time_flat = (fabs(x0) - v_max_dist) / v_max;
            traj_time_dec = (v_max - fabs(v1)) / d_max;
        }
    }
    // 分配加速度部分

    double a_to_v1_at_x0 = fabs(v0 * v0 - v1 * v1) / (2 * fabs(x0));
    double t_to_v1_at_x0 = (-fabs(v0) + sqrt(v0 * v0 + 2 * fabs(a_to_v1_at_x0) * fabs(x0))) / fabs(a_to_v1_at_x0);
    if (t_to_v1_at_x0 < period && a_to_v1_at_x0 < a_max) {
        traj_accel = - copysign(a_to_v1_at_x0, v0);
        traj_time += t_to_v1_at_x0;
        if(DEBUG_TIME) GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-380*10 * isX, -270*10 + (timeItor++) * 20*10), QString("R6").toLatin1(), timeDebugColor);
        return;
    }

    if (FRAME_PERIOD * a_max + fabs(v0) > v_max && traj_time_flat > period ) {                           // 匀速运动阶段
        traj_time += traj_time_acc + traj_time_flat + traj_time_dec;
        traj_accel = 0;
    }
    else if (traj_time_acc < vel_factor * period && traj_time_flat <  period && traj_time_dec > 0.0) {                                         // 加速接近结束且需减速
        traj_time += traj_accel + traj_time_flat +  traj_time_dec;
        traj_accel =  copysign(d_max * a_factor, -v0);
    }
    else {
        traj_time += traj_time_acc + traj_time_flat + traj_time_dec;
        traj_accel =  copysign(a_max * a_factor, -x0);
    }

    if(DEBUG_TIME) GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-380*10 * isX, -270*10 + (timeItor++) * 20*10), QString("R7").toLatin1(), timeDebugColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	void compute_motion_2d(CVector x0, CVector v0, CVector v1, double a_max, double v_max,
/// 		double a_factor, CVector &traj_accel, double &time)
///
/// @brief 在二维下计算运动过程，给定初始速度向量和最终速度向量，以及最大加速度，最大速度
///
/// @author	Song Zhibai( Song Zhibai Zju.edu.cn)
/// @date		2010-4-2
///
/// @param	x0						initial coordinate
/// @param	v0						initial velocities
/// @param	v1						final velocities
/// @param	a_max					maxium of accelerate
/// @param	v_max					maxium of velocities
/// @param	a_factor				factor of accelerate
/// @param [in,out]	traj_accel	the traj accel.
/// @param [in,out]	time		the time.
////////////////////////////////////////////////////////////////////////////////////////////////////

void compute_motion_2d(CVector x0, CVector v0, CVector v1,
                       double a_max, double d_max, double v_max,
                       double a_factor, CVector &traj_accel, double &time, double &time_acc, double &time_dec, double &time_flat, nonZeroMode mode) {
    double time_x = 0, time_x_acc = 0, time_x_dec = 0, time_x_flat = 0;
    double time_y = 0, time_y_acc = 0, time_y_dec = 0, time_y_flat = 0;
    double rotangle = 0;
    double traj_accel_x = 0;
    double traj_accel_y = 0;
    if(v0 * x0 > 0) {           //如果发现正在反方向走，则不再零速到点，防止车冲出去
//        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(1.0, 0.0), QString("Limit v1").toLatin1());
        v1 = CVector(0.0, 0.0);
    }
    if (v1.mod() == 0 || mode == FAST) {
        rotangle = x0.dir();
    }
    else {
        rotangle = v1.dir();
    }
    x0 = x0.rotate(-rotangle);
    v0 = v0.rotate(-rotangle);
    v1 = v1.rotate(-rotangle); //坐标系转换，转换到末速度方向为x轴的坐标系中

    double velFactorX = 1.0, velFactorY = 1.0;
    velFactorX = (fabs(v1.x()) > 1e-8 ? 2.8 : 1.0);
    velFactorY = (fabs(v1.y()) > 1e-8 ? 2.8 : 1.0);
    if(v1.mod() > 0 && mode == FAST) {
        v1.setVector(copysign(v1.mod(), v1.x()), 0);
//        v_max = v1.mod();
    }

    timeItor = 0;
    isX = 1;
    compute_motion_1d(x0.x(), v0.x(), v1.x(), a_max, d_max, v_max, a_factor, velFactorX,
                      traj_accel_x, time_x, time_x_acc, time_x_dec, time_x_flat, MOVE_X, mode);
    timeItor = 0;
    isX = -1;
    compute_motion_1d(x0.y(), v0.y(), v1.y(), a_max, d_max, v_max, a_factor, velFactorY,
                      traj_accel_y, time_y, time_y_acc, time_y_dec, time_y_flat, MOVE_Y, mode);//两轴同样的最大速度、加速度独立考虑求两轴运动时间

    if(v1.mod() > 1e-8 && mode == ACCURATE) {
        if (time_x - time_y > FRAME_PERIOD) {
            compute_motion_1d(x0.y(), v0.y(), 0, a_max, d_max, v_max, a_factor, velFactorX,
                              traj_accel_y, time_y, time_y_acc, time_y_dec, time_y_flat, MOVE_X, mode);
        } else if (time_y - time_x > FRAME_PERIOD) {
            compute_motion_1d(x0.x(), v0.x(), 0, a_max, d_max, v_max, a_factor, velFactorY,
                              traj_accel_x, time_x, time_x_acc, time_x_dec, time_x_flat, MOVE_X, mode);
        }
    }
    if(v1.mod() > 0 && DEBUG_NO_ZERO_VEL) {
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0.0, 0.0), QString("xVel: %1").arg(v0.x()).toLatin1());
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0.0, 20.0*10), QString("xVelFinal: %1").arg(v0.x() + traj_accel_x * FRAME_PERIOD).toLatin1());
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0.0, 40.0*10), QString("targetVel:  %1").arg(v1.mod()).toLatin1());
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0.0, 60.0*10), QString("yVel: %1").arg(v0.y()).toLatin1());
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0.0, 80.0*10), QString("yVelFinal: %1").arg(v0.y() + traj_accel_y * FRAME_PERIOD).toLatin1());
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0.0, 100.0*10), QString("v_max: %1").arg(v_max).toLatin1());
    }

    traj_accel = CVector(traj_accel_x, traj_accel_y);
    if (traj_accel.mod())
        traj_accel = traj_accel.rotate(rotangle);
    if(time_x < 1e-5 || time_x > 50) time_x = 0;
    if(time_y < 1e-5 || time_y > 50) time_y = 0;
    if(time_x < time_y) {
        if(DEBUG_TIME) GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(320*10 , -270*10), QString("this").toLatin1());
        time = time_y;
        time_acc = time_y_acc;
        time_dec = time_y_dec;
        time_flat = time_y_flat;
    } else {
        if(DEBUG_TIME) GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-320*10 , -270*10), QString("this").toLatin1());
        time = time_x;
        time_acc = time_x_acc;
        time_dec = time_x_dec;
        time_flat = time_x_flat;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	double compute_stop(double v, double max_a)
///
/// @brief	Calculates the stop.
///
/// @author	Song Zhibai( Song Zhibai Zju.edu.cn)
/// @date		2010-4-2
///
/// @param	v		The v.
/// @param	max_a	The maximum a.
///
/// @return	The calculated stop.
////////////////////////////////////////////////////////////////////////////////////////////////////

double compute_stop(double v, double max_a) {
    if (fabs(v) > max_a * FRAME_PERIOD) return  copysign(max_a, -v); // 如果一帧内不能减速到0，就用最大加速度
    else return -v / FRAME_PERIOD; // 如果能减到0，加速度就用速度除以一帧的时间即可
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	void goto_point_omni( const PlayerPoseT& start, const PlayerPoseT& final,
/// 		const PlayerCapabilityT& capability, const double& accel_factor,
/// 		const double& angle_accel_factor, PlayerPoseT& nextStep)
///
/// @brief	给定初始坐标，结束坐标及小车性能，计算小车可能的轨迹，更新下一个位置的机器人状态
///
/// @author	Song Zhibai( Song Zhibai Zju.edu.cn)
/// @date		2010-4-2
///
/// @param	start				The start.
/// @param	final				The final.
/// @param	capability			The capability.
/// @param	accel_factor		The accel factor.
/// @param	angle_accel_factor	The angle accel factor.
/// @param [in,out]	nextStep	the next step.
////////////////////////////////////////////////////////////////////////////////////////////////////

void goto_point_omni( const PlayerPoseT& start,
                      const PlayerPoseT& final,
                      const PlayerCapabilityT& capability,
                      const double& accel_factor,
                      const double& angle_accel_factor,
                      PlayerPoseT& nextStep,
                      nonZeroMode mode) {
    CGeoPoint target_pos = final.Pos();
    CVector x = start.Pos() - target_pos;
    CVector v = start.Vel();
    double ang = Utils::Normalize(start.Dir() - final.Dir());
    double ang_v = start.RotVel();
    CVector target_vel = final.Vel();
    double max_accel = capability.maxAccel;
    double max_decel = capability.maxDec;
    double max_speed = capability.maxSpeed;

    double max_angle_speed = capability.maxAngularSpeed;
    double max_angle_accel = capability.maxAngularAccel;
    double max_angle_decel = capability.maxAngularDec;
    CVector a;
    double ang_a, factor_a;
    double time_a, time_a_acc, time_a_dec, time_a_flat, time;
    double time_acc, time_dec, time_flat;
    compute_motion_2d(x, v, target_vel, max_accel, max_decel, max_speed, accel_factor, a, time, time_acc, time_dec, time_flat, mode);
    factor_a = 1;

    double rotateFactor;
    double selfSpeed = start.Vel().mod();

    if(upperBoundSpeedLimitRotate == lowerBoundSpeedLimitRotate)
        cout << "Oh shit!!! Error speed limit params!!! ---CMmotion.cpp" << endl;
    else if(selfSpeed < lowerBoundSpeedLimitRotate)
        rotateFactor = 1.0;
    else if(selfSpeed < upperBoundSpeedLimitRotate)
        rotateFactor = 1 - (selfSpeed - lowerBoundSpeedLimitRotate) / (upperBoundSpeedLimitRotate - lowerBoundSpeedLimitRotate);
    else
        rotateFactor = 0.0;
//    rotateFactor = 1.0;
    if(capability.maxAngularAccel < min_max_angle_acc)
        max_angle_accel= max_angle_decel = capability.maxAngularAccel;
    else
        max_angle_accel = max_angle_decel = min_max_angle_acc + (capability.maxAngularAccel * rotateScaleFactor - min_max_angle_acc) * rotateFactor;

    if(fabs(ang) > minAngleLimitRotateSpeed) {
        if(capability.maxAngularSpeed < min_max_angle_speed)
            max_angle_speed = capability.maxAngularSpeed;
        else
            max_angle_speed = min_max_angle_speed + (capability.maxAngularSpeed * rotateScaleFactor - min_max_angle_speed) * rotateFactor;
    }

    compute_motion_1d(ang, ang_v, 0.0, max_angle_accel, max_angle_decel, max_angle_speed, angle_accel_factor, 1.0, ang_a, time_a, time_a_acc, time_a_dec, time_a_flat, ROTATE, mode);

    if(DISPLAY_ROTATION_LIMIT){
        GDebugEngine::Instance()->gui_debug_msg(target_pos+CVector(0,-40*10), QString("maxRotateAcc:   %1").arg(max_angle_accel).toLatin1());
        GDebugEngine::Instance()->gui_debug_msg(target_pos+CVector(0,-20*10), QString("maxRotateSpeed: %1").arg(max_angle_speed).toLatin1());
        GDebugEngine::Instance()->gui_debug_msg(target_pos+CVector(0,-60*10), QString("rotateVel:      %1").arg(ang_v).toLatin1());
        GDebugEngine::Instance()->gui_debug_msg(target_pos+CVector(0,-80*10), QString("nextRotateVel:  %1").arg(ang_v + ang_a * FRAME_PERIOD).toLatin1());
    }

    v = v + a * FRAME_PERIOD;
    ang_v += ang_a * FRAME_PERIOD;
    double delta_ang = ang_v * FRAME_PERIOD;
    v.rotate(-delta_ang);
    if (v.mod() > max_speed) {
        v = v * max_speed / v.mod();
    }
    if (ang_v > max_angle_speed) {
        ang_v = max_angle_speed;
    } else if (ang_v < -max_angle_speed) {
        ang_v = -max_angle_speed;
    }
    CGeoPoint next_pos = start.Pos() + Utils::Polar2Vector(v.mod() * FRAME_PERIOD, v.dir());
    double next_angle = start.Dir() + ang_v * FRAME_PERIOD;

    nextStep.SetValid(true);
    nextStep.SetPos(next_pos);
    nextStep.SetDir(next_angle);
    nextStep.SetVel(v);

    nextStep.SetRotVel(ang_v);

    if(DEBUG_TIME) {
        CVector acc;
        compute_motion_2d(x, v, target_vel, OUR_MAX_ACC, OUR_MAX_DEC, OUR_MAX_SPEED, accel_factor, acc, time, time_acc, time_dec, time_flat);

        static double lastTime = time;
        static double lastTimeAcc = time_acc;
        static double lastTimeDec = time_dec;
        static double lastTimeFlat = time_flat;
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-100*10, -400*10), QString("initialVel: %1").arg(start.Vel().mod()).toLatin1(), timeDebugColor);
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(100*10, -400*10), QString("finalVel: %1").arg((start.Vel() + a / PARAM::Vision::FRAME_RATE).mod()).toLatin1(), timeDebugColor);
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0.0, 0.0), QString("time: %1").arg((lastTime - time) * 1000.0).toLatin1(), timeDebugColor);
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0.0, 20*10), QString("timeAcc: %1").arg(time_acc * 1000.0).toLatin1(), timeDebugColor);
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0.0, 40*10), QString("timeDec: %1").arg(time_dec * 1000.0).toLatin1(), timeDebugColor);
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0.0, 60*10), QString("timeFlat: %1").arg((lastTimeFlat - time_flat) * 1000.0).toLatin1(), timeDebugColor);

        lastTime = time;
        lastTimeAcc = time_acc;
        lastTimeDec = time_dec;
        lastTimeFlat = time_flat;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	double expectedCMPathTime(const PlayerPoseT& start, const CGeoPoint& final,
/// 	double maxAccel, double maxVelocity, double accel_factor)
///
/// @brief	调用 compute_motion_2d
///
/// @author	Song Zhibai( Song Zhibai Zju.edu.cn)
/// @date	2010-4-2
///
/// @param	start			The start.
/// @param	final			The final.
/// @param	maxAccel		The maximum accel.
/// @param	maxVelocity		The maximum velocity.
/// @param	accel_factor	The accel factor.
///
/// @return	.
////////////////////////////////////////////////////////////////////////////////////////////////////

double expectedCMPathTime(const PlayerPoseT& start, const CGeoPoint& final, double maxAccel, double maxVelocity, double accel_factor) {
    CGeoPoint target_pos = final;
    CVector x = start.Pos() - target_pos;
    CVector v = start.Vel();
    CVector target_vel = CVector(0, 0);
    CVector a;
    double time;
    double time_acc, time_dec, time_flat;
    compute_motion_2d(x, v, target_vel,
                      maxAccel,
                      maxAccel,
                      maxVelocity,
                      accel_factor,
                      a, time, time_acc, time_dec, time_flat);
    return time;
}

double predictedTime(const PlayerVisionT& start, const CGeoPoint & Target, const CVector& targetVel) {
    CVector x = start.Pos() - Target;
    CVector v = (start.Vel().mod() < 2.5*10) ? CVector(0, 0) : start.Vel();
    double time;
    CVector a;
    double time_acc, time_dec, time_flat;
    double accel_factor = 1.5;
    if(IS_SIMULATION) {
        accel_factor = 1.0;
    }
    compute_motion_2d(x, v, targetVel,
                      OUR_MAX_ACC,
                      OUR_MAX_DEC,
                      OUR_MAX_SPEED,
                      accel_factor,
                      a, time, time_acc, time_dec, time_flat);

    return time;
}

double predictedTimeWithRawVel(const PlayerVisionT& start, const CGeoPoint & Target, const CVector& targetVel) {
    CVector x = start.Pos() - Target;
    CVector v = (start.RawVel().mod() < 2.5*10) ? CVector(0, 0) : start.RawVel();
//    GDebugEngine::Instance()->gui_debug_msg(start.Pos(), QString("vel: (%1, %2)").arg(v.x()).arg(v.y()).toLatin1());
    double time;
    CVector a;
    double time_acc, time_dec, time_flat;
    double accel_factor = 1.5;
    if(IS_SIMULATION) {
        accel_factor = 1.0;
    }
    compute_motion_2d(x, v, targetVel,
                      OUR_MAX_ACC,
                      OUR_MAX_DEC,
                      OUR_MAX_SPEED,
                      accel_factor,
                      a, time, time_acc, time_dec, time_flat);

    return time;
}

double predictedTheirTime(const PlayerVisionT& start, const CGeoPoint & Target, const CVector& targetVel) {
    double max_acc = 500*10;
    double max_speed = 350*10;

    CVector x = start.Pos() - Target;
    CVector v = start.Vel();
    double time;
    CVector a;
    double time_acc, time_dec, time_flat;
    compute_motion_2d(x, v, targetVel,
                      max_acc,
                      max_acc,
                      max_speed,
                      1.5,
                      a, time, time_acc, time_dec, time_flat);

    return time;
}

double predictedTime2d(const PlayerVisionT& start, const CGeoPoint& final, double maxAccel, double maxVelocity, double accel_factor) {
    CVector x0 = start.Pos() - final;
    CVector trajAcc;
    double trajTime;
    double time_acc, time_dec, time_flat;
    compute_motion_2d(x0, start.Vel(), CVector(0, 0),
                      maxAccel,
                      maxAccel,
                      maxVelocity,
                      accel_factor,
                      trajAcc, trajTime, time_acc, time_dec, time_flat);
    return trajTime;
}

double predictedTime1d(const double &start, const double &end, const double &startVel, const double &maxAccel, const double &maxVelocity, const double endVel) {
    double trajTime, trajAcc, accTime, flatTime, decTime;
    compute_motion_1d(start - end, startVel, endVel,
                      maxAccel,
                      maxAccel,
                      maxVelocity,
                      1.5,
                      1.0, trajAcc, trajTime, accTime, flatTime, decTime, MOVE_X);
    return trajTime;
}

bool predictRushSpeed(const PlayerVisionT& start, const CGeoPoint& final, const double& time, const PlayerCapabilityT& capability, CVector& targetVel) {
    CVector x = start.Pos() - final;
    CVector v = start.Vel();
    double zeroTime;
    CVector a;
    double time_acc, time_dec, time_flat;
    compute_motion_2d(x, v, CVector(0, 0), capability.maxAccel, capability.maxDec, capability.maxSpeed, 1.5, a, zeroTime, time_acc, time_dec, time_flat);
    if (zeroTime < time) {
        targetVel = CVector(0, 0);
        return true;
    } else {
        CVector me2target = final - start.Pos();
        double startVel = start.Vel() * me2target.unit();
        double triangleDist = (pow(capability.maxSpeed, 2) - pow(startVel, 2))/ (2 * capability.maxAccel);
        double triangleTime = (capability.maxSpeed - startVel) / capability.maxAccel;
        double restDist = x.mod() - triangleDist;
        double restTime = time - triangleTime;
        if (restTime - restDist / capability.maxSpeed > 0) {
            double decTime = sqrt(2 * (capability.maxSpeed * restTime - restDist) / capability.maxAccel);
            targetVel = Utils::Polar2Vector(capability.maxSpeed - decTime * capability.maxAccel, (final - start.Pos()).dir());
            if(isnan(targetVel.x()) || isnan(targetVel.y())) {
                cout << "Oh shit!!! Error targetVel!!! ---CMmotion.cpp" << endl;
            }
            return true;
        } else {
            targetVel = Utils::Polar2Vector(capability.maxSpeed, (final - start.Pos()).dir());
            return false;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	double openSpeedCircle(const PlayerPoseT& start, const double dist2Center, const double dirDiff,
///  const double finalAngle, PlayerPoseT& nextStep)
///
/// @param	start			The start.
/// @param  dist2Center     The rotate radius.
/// @param  dirDiff         The direcition diff between robot angle and circle tangent.
/// @param  finalAngle      The final angle u want the robot finish at.
/// @param  nextStep        The return trajectory.
///
/// @return	.
////////////////////////////////////////////////////////////////////////////////////////////////////

void openSpeedCircle(const PlayerPoseT& start, const double dist2Center, const int rotateMethod, const double finalAngle, PlayerPoseT& nextStep, const double dirDiff) {
    CGeoPoint startPos = start.Pos();
    double startRotVel = start.RotVel();

    double startDir, finalDir;
    if (rotateMethod == 1 || rotateMethod == 4) {
        startDir = Utils::Normalize(start.Dir() + dirDiff);
        finalDir = Utils::Normalize(finalAngle + dirDiff);
    } else {
        startDir = Utils::Normalize(start.Dir() - dirDiff);
        finalDir = Utils::Normalize(finalAngle - dirDiff);
    }

    double posDirDiff = abs(Utils::Normalize(startDir - finalDir));

    double rotAcc, rotTime, rotAccTime, rotDecTime, rotFlatTime;

    if (rotateMethod == 1 || rotateMethod == 4) {
        compute_motion_1d(posDirDiff, start.RotVel(), 0, 15, 5, 5, 1.5, 1.0, rotAcc, rotTime, rotAccTime, rotDecTime, rotFlatTime, ROTATE);
    } else {
        compute_motion_1d(-posDirDiff, start.RotVel(), 0, 15, 5, 5, 1.5, 1.0, rotAcc, rotTime, rotAccTime, rotDecTime, rotFlatTime, ROTATE);
    }

    double rotVel = startRotVel + rotAcc / PARAM::Vision::FRAME_RATE;
    double velMod = abs(rotVel * dist2Center);
    CVector vel;
    if (rotateMethod == 1 || rotateMethod == 3) {
        vel = Utils::Polar2Vector(velMod, startDir);
    } else {
        vel = Utils::Polar2Vector(velMod, Utils::Normalize(startDir + PARAM::Math::PI));
    }

    CVector localVel = vel.rotate(- start.Dir());
    CGeoPoint nextPos = startPos + vel / PARAM::Vision::FRAME_RATE;
    double nextDir = startDir + rotVel / PARAM::Vision::FRAME_RATE;

    nextStep.SetValid(true);
    nextStep.SetPos(nextPos);
    nextStep.SetDir(nextDir);
    nextStep.SetVel(localVel);
    nextStep.SetRotVel(rotVel);
}


