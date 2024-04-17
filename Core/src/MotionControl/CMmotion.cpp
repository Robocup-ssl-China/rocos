#include <iostream>
#include <math.h>
#include <stdio.h>
#include <CMmotion.h>
#include <utils.h>
#include <fstream>
#include "GDebugEngine.h"
#include "parammanager.h"
#include <fmt/core.h>


using namespace std;

namespace {
const double FRAME_PERIOD = 1.0 / PARAM::Vision::FRAME_RATE;

bool DEBUG_NO_ZERO_VEL = ZSS::ZParamManager::instance()->value("Debug/NoZeroVel", QVariant(false)).toBool();
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
double PREDICT_TIME_ACC_RATIO = ZSS::ZParamManager::instance()->value("CGotoPositionV2/PredictAccRatio", QVariant(1.5)).toDouble();
double PREDICT_TIME_THEIR_ACC_RATIO = ZSS::ZParamManager::instance()->value("CGotoPositionV2/PredictAccTheirRatio", QVariant(1.5)).toDouble();
bool addComp = true;
int timeDebugColor = COLOR_GREEN;
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
void __new_compute_motion_1d(double x0, double v0, double v1,
                              double a_max, double v_max, double a_factor,
                              double &traj_accel, double &traj_time){
  // First check to see if nothing needs to be done...
  if (x0 == 0 && v0 == v1) { traj_accel = 0; traj_time = 0;  return; }

  if(!finite(x0) || !finite(v0) || !finite(v1)){
    printf("Robot::compute_motion_1d: NANs!\n");
    traj_accel = 0; traj_time = 0;  return;
  }

  // Need to do some motion.
  a_max /= a_factor;

  double time_to_v1 = fabs(v0 - v1) / a_max;
  double x_to_v1 = fabs((v0 + v1) / 2.0) * time_to_v1;

  double period = 2.0 * FRAME_PERIOD;

  v1 = copysign(v1, -x0);

  if (v0 * x0 > 0 || (fabs(v0) > fabs(v1) && x_to_v1 > fabs(x0))) {
    // Time to reach goal after stopping + Time to stop.
    double time_to_stop = fabs(v0) / a_max;
    double x_to_stop = v0 * v0 / (2 * a_max);

    __new_compute_motion_1d(x0 + copysign(x_to_stop, v0), 0, v1, a_max * a_factor,
                      v_max, a_factor, traj_accel, traj_time);
    traj_time += time_to_stop;
 
    // Decelerate
    if (traj_time < period) 
      traj_accel = compute_stop(v0, a_max * a_factor);
    else if (time_to_stop < period) 
      traj_accel = time_to_stop / period * - copysign(a_max * a_factor, v0) +
	(1.0 - time_to_stop / period) * traj_accel;
    else traj_accel = -copysign(a_max * a_factor, v0);

    return;
  }


  // At this point we have two options.  We can maximally accelerate
  // and then maximally decelerate to hit the target.  Or we could
  // find a single acceleration that would reach the target with zero
  // velocity.  The later is useful when we are close to the target
  // where the former is less stable.

  // OPTION 1
  // This computes the amount of time to accelerate before decelerating.
  double t_a, t_accel, t_decel;

  if (fabs(v0) > fabs(v1)) {
    //    t_a = (sqrt((3*v1*v1 + v0*v0) / 2.0 - fabs(v0 * v1) + fabs(x0) * a_max) 
    //	   - fabs(v0)) / a_max;
    t_a = (sqrt((v0 * v0 + v1 * v1) / 2.0 + fabs(x0) * a_max) 
	   - fabs(v0)) / a_max;

    if (t_a < 0.0) t_a = 0;
    t_accel = t_a;
    t_decel = t_a + time_to_v1;
  } else if (x_to_v1 > fabs(x0)) {
    t_a = (sqrt(v0 * v0 + 2 * a_max * fabs(x0)) - fabs(v0)) / a_max;
    t_accel = t_a;
    t_decel = 0.0;
  } else {
    //    t_a = (sqrt((3*v0*v0 + v1*v1) / 2.0 - fabs(v0 * v1) + fabs(x0) * a_max) 
    //  - fabs(v1)) / a_max;

    t_a = (sqrt((v0 * v0 + v1 * v1) / 2.0 + fabs(x0) * a_max) 
	   - fabs(v1)) / a_max;

    if (t_a < 0.0) t_a = 0;
    t_accel = t_a + time_to_v1;
    t_decel = t_a;
  }

  // OPTION 2
  double a_to_v1_at_x0 = (v0 * v0 - v1 * v1) / (2 * fabs(x0));
  double t_to_v1_at_x0 = 
    (-fabs(v0) + sqrt(v0 * v0 + 2 * fabs(a_to_v1_at_x0) * fabs(x0))) / 
    fabs(a_to_v1_at_x0);

  // We follow OPTION 2 if t_a is less than a FRAME_PERIOD making it
  // difficult to transition to decelerating and stopping exactly.
  if (0 && a_to_v1_at_x0 < a_max && a_to_v1_at_x0 > 0.0 &&
      t_to_v1_at_x0 < 2.0 * FRAME_PERIOD && 0) {

    // OPTION 2
    // Use option 1 time, even though we're not following it.
    traj_time = t_accel + t_decel;;

    // Target acceleration to stop at x0.
    traj_accel = -copysign(a_to_v1_at_x0, v0);

    return;
  } else {

    // OPTION 1
    // Time to accelerate and decelerate.
    traj_time = t_accel + t_decel;

    // If the acceleration time would get the speed above v_max, then
    //  we need to add time to account for cruising at max speed.
    if (t_accel * a_max + fabs(v0) > v_max) {
      traj_time += 
	pow(v_max - (a_max * t_accel + fabs(v0)), 2.0) / a_max / v_max;
    }

    // Accelerate (unless t_accel is less than FRAME_PERIOD, then set
    // acceleration to average acceleration over the period.)
    if (t_accel < period && t_decel == 0.0) {
      traj_accel = copysign(a_max * a_factor, -x0);
    } else if (t_accel < period && t_decel > 0.0) {
      traj_accel = compute_stop(v0, a_max * a_factor);
    } else if (t_accel < period) {
      traj_accel = copysign((2.0 * t_accel / (period) - 1) * a_max * a_factor, v0);
    } else {
      traj_accel = copysign(a_max * a_factor, -x0);
    }
  }
}

void __new_compute_motion_2d(CVector x0, CVector v0, CVector v1,
                              double a_max, double v_max, double a_factor,
                              CVector &traj_accel, double &time){
  double time_x, time_y;
  double rotangle = x0.dir();

  x0 = x0.rotate(-rotangle);
  v0 = v0.rotate(-rotangle);
  v1 = v1.rotate(-rotangle);
  
  double traj_accel_x, traj_accel_y;

  __new_compute_motion_1d(x0.x(), v0.x(), v1.x(), a_max, v_max, a_factor,
		    traj_accel_x, time_x);
  __new_compute_motion_1d(x0.y(), v0.y(), v1.y(), a_max, v_max, a_factor,
		    traj_accel_y, time_y);

  traj_accel = CVector(traj_accel_x, traj_accel_y).rotate(rotangle);
  traj_accel = Utils::Polar2Vector(std::min(traj_accel.mod(),a_max), traj_accel.dir());

  time = std::max(time_x, time_y);
}

void __new_goto_point_omni(const PlayerPoseT& start,
						const PlayerPoseT& final,
						const PlayerCapabilityT& CAPABILITY,
						const double& ACC_FACTOR,
						const double& ANGLE_ACC_FACTOR,
						PlayerPoseT& nextStep){
    CGeoPoint target_pos = final.Pos();
    CVector x = start.Pos() - target_pos;
    CVector v = start.Vel();
    CVector target_vel = final.Vel();

    CVector a;
    double time;
    __new_compute_motion_2d(x, v, target_vel, CAPABILITY.maxAccel, CAPABILITY.maxSpeed, ACC_FACTOR, a, time);

    double w0 = Utils::Normalize(start.Dir() - final.Dir());
    double wv = start.RotVel();
    double wv1 = final.RotVel();
    double aw;
    double time_w;
    __new_compute_motion_1d(w0, wv, wv1, CAPABILITY.maxAngularAccel, CAPABILITY.maxAngularSpeed, ANGLE_ACC_FACTOR, aw, time_w);

    if (DISPLAY_ROTATION_LIMIT){
        auto P = start.Pos() + CVector(200,200);
        auto C = COLOR_PURPLE;
        GDebugEngine::Instance()->gui_debug_msg(P, "TT", C);
        GDebugEngine::Instance()->gui_debug_line(P,P+v*0.1,C);
        GDebugEngine::Instance()->gui_debug_msg(P+v*0.2, fmt::format("V,wv:{:.1f},l-({:.1f},{:.1f})",wv, (wv + aw * FRAME_PERIOD), std::min(1000.0,CAPABILITY.maxAccel/(v.mod()))), C);
    }

    v = v + a * FRAME_PERIOD;

    // option 1 : old method - bang-bang control to vw
    // wv = wv + aw * FRAME_PERIOD;

    // 思路 ： 根据移动和旋转的预计时间进行限制的分配
    // option2 : consider conbine constraint of vw and v
    double target_wv = wv + aw * FRAME_PERIOD;
    wv = copysign(std::min(fabs(target_wv), CAPABILITY.maxAccel/(v.mod())), target_wv);

    nextStep.SetValid(true);
    nextStep.SetVel(v);
    nextStep.SetPos(start.Pos() + v*FRAME_PERIOD);
    nextStep.SetRotVel(wv);
    nextStep.SetDir(Utils::Normalize(start.Dir() + wv * FRAME_PERIOD));
}

void compute_motion_1d(double x0, double v0, double v1,
                       double a_max, double d_max, double v_max, double a_factor, double vel_factor,
                       double &traj_accel, double &traj_time){
                        __new_compute_motion_1d(x0, v0, v1, a_max, v_max, a_factor, traj_accel, traj_time);
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
                       double a_factor, CVector &traj_accel, double &time) {
    double time_x = 0, time_x_acc = 0, time_x_dec = 0, time_x_flat = 0;
    double time_y = 0, time_y_acc = 0, time_y_dec = 0, time_y_flat = 0;
    double rotangle = 0;
    double traj_accel_x = 0;
    double traj_accel_y = 0;
    
    rotangle = x0.dir();
    x0 = x0.rotate(-rotangle);
    v0 = v0.rotate(-rotangle);
    v1 = v1.rotate(-rotangle); //坐标系转换，转换到末速度方向为x轴的坐标系中

    double velFactorX = 1.0, velFactorY = 1.0;

    compute_motion_1d(x0.x(), v0.x(), v1.x(), a_max, d_max, v_max, a_factor, velFactorX,
                      traj_accel_x, time_x);
    compute_motion_1d(x0.y(), v0.y(), v1.y(), a_max, d_max, v_max, a_factor, velFactorY,
                      traj_accel_y, time_y);//两轴同样的最大速度、加速度独立考虑求两轴运动时间

    // if(v1.mod() > 1e-8 && mode == ACCURATE) {
    //     if (time_x - time_y > FRAME_PERIOD) {
    //         compute_motion_1d(x0.y(), v0.y(), 0, a_max, d_max, v_max, a_factor, velFactorX,
    //                           traj_accel_y, time_y, time_y_acc, time_y_dec, time_y_flat, MOVE_X, mode);
    //     } else if (time_y - time_x > FRAME_PERIOD) {
    //         compute_motion_1d(x0.x(), v0.x(), 0, a_max, d_max, v_max, a_factor, velFactorY,
    //                           traj_accel_x, time_x, time_x_acc, time_x_dec, time_x_flat, MOVE_X, mode);
    //     }
    // }
    if(v1.mod() > 0 && DEBUG_NO_ZERO_VEL) {
        x0 = x0.rotate(rotangle);
        v0 = v0.rotate(rotangle);
        v1 = v1.rotate(rotangle); //坐标系转换，转换到末速度方向为x轴的坐标系中
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0.0, 0.0), QString("Vel: %1 %2").arg(v0.x()).arg(v0.y()).toLatin1());
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0.0, 20.0*10), QString("xVelFinal: %1").arg(v0.x() + traj_accel_x * FRAME_PERIOD).toLatin1());
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0.0, 40.0*10), QString("targetVel:  %1 %2 %3").arg(v1.mod()).arg(v1.x()).arg(v1.y()).toLatin1());
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0.0, 60.0*10), QString("yVel: %1").arg(v0.y()).toLatin1());
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0.0, 80.0*10), QString("yVelFinal: %1").arg(v0.y() + traj_accel_y * FRAME_PERIOD).toLatin1());
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0.0, 100.0*10), fmt::format("v0:{:.1f},{:.1f}",v0.x(),v0.y()));
        // GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0.0, 100.0*10), QString("v_max: %1").arg(v_max).toLatin1());
        auto S = CGeoPoint(-1000,-1000);
        GDebugEngine::Instance()->gui_debug_line(S,S+v0,COLOR_BLUE);
        GDebugEngine::Instance()->gui_debug_msg(S+v0, "v0", COLOR_BLUE);
        GDebugEngine::Instance()->gui_debug_line(S, S+v1, COLOR_BLUE);
        GDebugEngine::Instance()->gui_debug_msg(S+v1, "v1", COLOR_BLUE);
        GDebugEngine::Instance()->gui_debug_line(S, S+x0,COLOR_PURPLE);
        GDebugEngine::Instance()->gui_debug_msg(S+x0, "x0", COLOR_PURPLE);
        auto traj_acc = CVector(traj_accel_x, traj_accel_y).rotate(rotangle);
        GDebugEngine::Instance()->gui_debug_line(S, S+traj_acc,COLOR_RED);
        GDebugEngine::Instance()->gui_debug_msg(S+traj_acc, "Acc", COLOR_RED);
        GDebugEngine::Instance()->gui_debug_line(S,S+Utils::Polar2Vector(500,rotangle), COLOR_BLUE);
        GDebugEngine::Instance()->gui_debug_msg(S+Utils::Polar2Vector(500,rotangle), fmt::format("PlanDir: {:.1f},{:.1f}",traj_acc.x(),traj_acc.y()), COLOR_BLUE);
    }

    traj_accel = CVector(traj_accel_x, traj_accel_y);
    traj_accel = traj_accel.rotate(rotangle);
    traj_accel = Utils::Polar2Vector(std::min(traj_accel.mod(),a_max), traj_accel.dir());
    if(time_x < 1e-5 || time_x > 50) time_x = 0;
    if(time_y < 1e-5 || time_y > 50) time_y = 0;
    time = std::max(time_x, time_y);
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
                      PlayerPoseT& nextStep) {
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
    // compute_motion_2d(x, v, target_vel, max_accel, max_decel, max_speed, accel_factor, a, time);
    __new_compute_motion_2d(x, v, target_vel, max_accel, max_speed, accel_factor, a, time);
    // a = Utils::Polar2Vector(std::min(1.0,a.mod()),a.dir());
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

    compute_motion_1d(ang, ang_v, 0.0, max_angle_accel, max_angle_decel, max_angle_speed, angle_accel_factor, 1.0, ang_a, time_a);

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
    compute_motion_2d(x, v, target_vel,
                      maxAccel,
                      maxAccel,
                      maxVelocity,
                      accel_factor,
                      a, time);
    return time;
}

double predictedTime(const PlayerVisionT& start, const CGeoPoint & Target, const CVector& targetVel) {
    CVector x = start.Pos() - Target;
    CVector v = start.Vel();
    double time;
    CVector a;
    double time_acc, time_dec, time_flat;
    double accel_factor = PREDICT_TIME_ACC_RATIO;
    if(IS_SIMULATION) {
        accel_factor = 1.0;
    }
    compute_motion_2d(x, v, targetVel,
                      OUR_MAX_ACC,
                      OUR_MAX_DEC,
                      OUR_MAX_SPEED,
                      accel_factor,
                      a, time);

    return time;
}

double predictedTimeWithRawVel(const PlayerVisionT& start, const CGeoPoint & Target, const CVector& targetVel) {
    CVector x = start.Pos() - Target;
    CVector v = start.RawVel();
//    GDebugEngine::Instance()->gui_debug_msg(start.Pos(), QString("vel: (%1, %2)").arg(v.x()).arg(v.y()).toLatin1());
    double time;
    CVector a;
    double time_acc, time_dec, time_flat;
    double accel_factor = PREDICT_TIME_ACC_RATIO;
    if(IS_SIMULATION) {
        accel_factor = 1.0;
    }
    compute_motion_2d(x, v, targetVel,
                      OUR_MAX_ACC,
                      OUR_MAX_DEC,
                      OUR_MAX_SPEED,
                      accel_factor,
                      a, time);

    return time;
}

double predictedTheirTime(const PlayerVisionT& start, const CGeoPoint & Target, const CVector& targetVel) {
    double max_acc = 500*10;
    double max_speed = 350*10;

    CVector x = start.Pos() - Target;
    CVector v = start.Vel();
    double time;
    CVector a;
    compute_motion_2d(x, v, targetVel,
                      max_acc,
                      max_acc,
                      max_speed,
                      PREDICT_TIME_THEIR_ACC_RATIO,
                      a, time);

    return time;
}

double predictedTime2d(const PlayerVisionT& start, const CGeoPoint& final, double maxAccel, double maxVelocity, double accel_factor) {
    CVector x0 = start.Pos() - final;
    CVector trajAcc;
    double trajTime;
    compute_motion_2d(x0, start.Vel(), CVector(0, 0),
                      maxAccel,
                      maxAccel,
                      maxVelocity,
                      accel_factor,
                      trajAcc, trajTime);
    return trajTime;
}

double predictedTime1d(const double &start, const double &end, const double &startVel, const double &maxAccel, const double &maxVelocity, const double endVel) {
    double trajTime, trajAcc, accTime, flatTime, decTime;
    compute_motion_1d(start - end, startVel, endVel,
                      maxAccel,
                      maxAccel,
                      maxVelocity,
                      1.5,
                      1.0, trajAcc, trajTime);
    return trajTime;
}

bool predictRushSpeed(const PlayerVisionT& start, const CGeoPoint& final, const double& time, const PlayerCapabilityT& capability, CVector& targetVel) {
    CVector x = start.Pos() - final;
    CVector v = start.Vel();
    double zeroTime;
    CVector a;
    double time_acc, time_dec, time_flat;
    compute_motion_2d(x, v, CVector(0, 0), capability.maxAccel, capability.maxDec, capability.maxSpeed, 1.5, a, zeroTime);
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
        compute_motion_1d(posDirDiff, start.RotVel(), 0, 15, 5, 5, 1.5, 1.0, rotAcc, rotTime);
    } else {
        compute_motion_1d(-posDirDiff, start.RotVel(), 0, 15, 5, 5, 1.5, 1.0, rotAcc, rotTime);
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


