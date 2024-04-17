/************************************************************************/
/* Copyright (c) CSC-RL, Zhejiang University							*/
/* Team:		ZJUNlict												*/
/* HomePage:	http://www.nlict.zju.edu.cn/ssl/WelcomePage.html		*/
/************************************************************************/
/* File:	ControlModel.cpp											*/
/* Brief:	C++ Implementation:	Motion Control Warpper					*/
/* Func:	Support calling motion control algorithms					*/
/* Author:	cliffyin, 2012, 04											*/
/* Refer:	some papers													*/
/* E-mail:	cliffyin@zju.edu.cn		cliffyin007@gmail.com				*/
/* Version:	0.0.1														*/
/* Changelog:	2012-04-10	create										*/
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
// include files
#include "ControlModel.h"					// for self include
#include <utils.h>							// for some helper functions

#include <cornell/Trajectory.h>				// for Cornell BangBang
#include <cornell/TrajectorySupport.h>

#include <CMmotion.h>
#include <parammanager.h>

//#include "2012/ptg/PTG_Inc/CSCRL_PTG.h"

using namespace std;
namespace {
      bool IS_SIMULATION = ZSS::ZParamManager::instance()->value("Alert/IsSimulation", QVariant(false)).toBool();
}
/// BangBang control from Cornell : zero final velocity trajectory
void CControlModel::makeZeroFinalVelocityPath(const PlayerPoseT& start, const PlayerPoseT& final, const PlayerCapabilityT& capability)
{
    _pathList.clear();
    zeroFinalVelocityPath(start, final, PARAM::Vision::FRAME_RATE, capability, _nextStep);
}

/// BangBang control from Cornell : fast trajectory
void CControlModel::makeFastPath(const PlayerPoseT& start, const PlayerPoseT& final, const PlayerCapabilityT& capability,const double finalVelDir)
{
    fastPath(start, final, PARAM::Vision::FRAME_RATE, capability, _nextStep,finalVelDir);
}

/// BangBang control from Cornell : proportional gain trajectory
//void CControlModel::makeProportionalGainPath(const PlayerPoseT& start, const PlayerPoseT& final, const PlayerCapabilityT& capability)
//{
//	proportionalGainPath(start, final, PARAM::Vision::FRAME_RATE, capability, _nextStep);
//}

/// BangBang control from Cornell : zero final theta velocity
void CControlModel::makeZeroFinalVelocityTheta(const PlayerPoseT& start, const PlayerPoseT& final, const PlayerCapabilityT& capability)
{
    _nextStep.SetRotVel(zeroFinalVelocityTheta(start, final, PARAM::Vision::FRAME_RATE, capability));
}

/// BangBang control from Cornell : proportional gain theta trajectory
//void CControlModel::makeProportionalGainTheta(const PlayerPoseT& start, const PlayerPoseT& final, const PlayerCapabilityT& capability)
//{
//	_nextStep.SetRotVel(proportionalGainTheta(start, final, PARAM::Vision::FRAME_RATE, capability));
//}

/// Trapezoidal control from CMU : none-zero final velocity trajectory
void CControlModel::makeCmTrajectory(const PlayerPoseT& start, const PlayerPoseT& final, const PlayerCapabilityT& capability)
{
    _pathList.clear();
    double accel_factor = 1.5;
    double angle_accel_factor = 5.5;
    if(IS_SIMULATION) {
        accel_factor = 1.0;
        angle_accel_factor = 1.0;
    }

    //cout << "ControlModel"<< endl;
    //cout << start.X()<<" "<<start.Y()<<" "<<start.Vel().mod()<<" "<<start.RotVel()<< endl;
    //cout << final.X() << " " << final.Y() << " " << final.Vel().mod() << endl;
    goto_point_omni(start,final,capability,accel_factor,angle_accel_factor,_nextStep);
}

void CControlModel::makeNewCmTrajectory(const PlayerPoseT& start, const PlayerPoseT& final, const PlayerCapabilityT& capability)
{
    _pathList.clear();
    double accel_factor = 1.5;
    double angle_accel_factor = 1.5;
    if(IS_SIMULATION) {
        accel_factor = 1.0;
        angle_accel_factor = 1.0;
    }
    __new_goto_point_omni(start,final,capability,accel_factor,angle_accel_factor,_nextStep);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// some light-weight functions
/// x-axis BangBang control
double zeroFinalXSpeed(const double length, const double xSpeed, const double maxSpeed, const double maxAccel)
{
    int  i,j,numIter;
    double du,fmid,xmid,u;
    double t1x,t2x,tfx,ux;
    double dt;
    double x,vx;

    double timeScale             = maxSpeed / maxAccel;
    double lengthScale           = maxSpeed * timeScale;
    double velocityScale         = maxSpeed;

    //  nondimensionalize positions and velocities
    double oneoverlengthscale = 1/lengthScale;
    double x0      = 0;
    double xf      = length * oneoverlengthscale;
    double vx0     = xSpeed / velocityScale;

    //finds the zero of tf(x0,xf,vx0,u)-tf(y0,yf,vy0,sqrt(1-sqr(u))) in (0,1) by bisection
    numIter = 10;//number of iterations performed. error in u is 2^(-numIter)
    u   =  0.999f;
    du  = -0.998f;

    for (j=0; j<numIter; j++)
    {
    fmid = dtFinal(x0,xf,vx0,0,0,0,xmid=u+(du *= 0.5));
    if (fmid <= 0.0) u=xmid;
    }

    //now we calculate the path properties
    bangBangPathProperties(x0,xf,vx0,u,t1x,t2x,tfx,ux);

    //theta is independent of x and y so we use bang-bang with u=1
    double nondimFrameRate = PARAM::Vision::FRAME_RATE * timeScale;
    int tfint = (int)ceil(tfx * nondimFrameRate);

    dt = 1/nondimFrameRate;

    int	maxStepNumber = min(tfint, 1);
    // 只要下一个周期的状态就行了，没有必要多迭代，调试的时候可以把1改成其他值
    // 向前迭代
    for( i=0; i<=maxStepNumber; ++i ){
        // Calculate step along x-axis
        step(x0,xf,vx0,ux,t1x,tfx,dt,i,x,vx);
        x  = lengthScale * x;
        vx = velocityScale * vx;
    }
    return vx;
}
double fastXSpeed(const double length, const double xSpeed, const double maxSpeed, const double maxAccel)
{
    // 直接以最大加速度加速
    double vx = xSpeed + maxAccel / PARAM::Vision::FRAME_RATE * Utils::Sign(length);
    if( vx > maxSpeed ){
        return maxSpeed;
    }
    if( vx < -maxSpeed ){
        return -maxSpeed;
    }
    return vx;

}

/// x-axis fast speed control
double zeroFinalAngularSpeed(const double length, const double angSpeed, const double maxSpeed, const double maxAccel)
{
    double t1theta,t2theta,tftheta,utheta;

    double angularTimeScale      = maxSpeed / maxAccel;
    double angularScale          = maxSpeed * maxSpeed / maxAccel;
    double angularVelocityScale  = maxSpeed;

    double theta0  = 0;
    double thetaf  = length / angularScale;

    double vtheta0 = angSpeed / angularVelocityScale;

    //use bang-bang with u=1 on the theta-axis
    bangBangPathProperties(theta0, thetaf, vtheta0, 1, t1theta, t2theta, tftheta, utheta);

    double nondimAngularFrameRate = PARAM::Vision::FRAME_RATE * angularTimeScale;
    double angulardt = 1 / nondimAngularFrameRate;
    double temp= std::exp(-angulardt);
    double result = angularVelocityScale * (temp * vtheta0 + utheta * (1 - temp));
    if( result * length < 0 ){
        return 0; // 转动没有那么大的惯性，所以可以直接反转
    }
    return result;
}

/// fast angular speed control
double fastAngularSpeed(const double length, const double angSpeed, const double maxSpeed, const double maxAccel)
{
    double va = angSpeed + maxAccel / PARAM::Vision::FRAME_RATE * Utils::Sign(length);
    if( va > maxSpeed ){
        return maxSpeed;
    }
    if( va < -maxSpeed ){
        return -maxSpeed;
    }
    if( va * length < 0 ){
        return 0; // 转动没有那么大的惯性，所以可以直接反转
    }
    return va;
}

/// none-zero final speed, 1d plan
double nonZeroFinalSpeed(const double length, const double Speed, const double maxSpeed, const double maxAccel)
{
    int  i;
    double z,vz;
    if (maxAccel==0 || maxSpeed==0 ){
        std::cout << "Trajectory code error: invalid scale(s)" << std::endl;
    }
    //fastPath能实现非零速度到点，在到达指定速度后保持匀速
    if (length<4)
    {
        return Speed;
        /*std::cout<<"keep velocity!!!!!!!!!!"<<endl;*/
    }
    else{
        double timeScale             =  maxSpeed /maxAccel;
        double lengthScale           =  maxSpeed*timeScale;
        double velocityScale         =  maxSpeed;
        //  nondimensionalize positions and velocities
        double z0      = 0;
        double vz0     = Speed/velocityScale;
        double zf      = length/lengthScale;
        double dz=zf-z0;
        double t=0;
        double dt=0.01f;
        double func=0;
        double temp=0;
        //下面几行代码是求bangbang控制过程的加速（减速）时间
        while(func>=0)
        {
            t+=dt;
            temp=1-(double)exp(-t);
            func=sqr( dz-(vz0*temp) ) - sqr(temp-t);
        }
//        double phi = 0;
        double nondimFrameRate = PARAM::Vision::FRAME_RATE *timeScale;
        int tfint = (int)ceil(t*nondimFrameRate);
        dt = 1/nondimFrameRate;
        int maxStepNumber = min(tfint, 1);
        // 只要下一个周期的状态就行了，没有必要多迭代，调试的时候可以把1改成其他值
        // 向前迭代一步
        for( i=0; i<=maxStepNumber; ++i ){
            // Calculate step
            step(z0,zf,vz0,1,t,t,dt,i, z, vz);
            z  = lengthScale*z;
            vz = velocityScale*vz;
        }
    }
    return vz;
}

/// trapezoidal velocity trajectory, 1d plan
double TrapezoidalVelocity(const double length,const double initialSpeed,const double maxSpeed,const double finalSpeed,const double maxAccel)
{
     const double GAIN_RATE = 1.5;
     double acceleration = maxAccel;//最大加速度加速
     double deceleration = GAIN_RATE*acceleration;//最大减速度，比例先大致取一下
     double triangleDistance = abs(sqr(maxSpeed)-sqr(initialSpeed))/(2*acceleration)+abs(sqr(maxSpeed)-sqr(finalSpeed))/(2*deceleration);
     double t1,t2,t3;//加速、匀速、减速过程时间
     double vz;
     if (triangleDistance < length)
     {
         //此时有速度规划过程为三段，中间有一段匀速过程
         t1 = (maxSpeed-initialSpeed)/acceleration;
         t3 = (maxSpeed-finalSpeed)/deceleration;
         t2 = (length-triangleDistance)/maxSpeed;
     }
     else{
         //此时速度规划没有匀速过程
         int sign =Utils::Sign(finalSpeed-initialSpeed);
         if (sign>0)
         {

             double accDist = abs(sqr(finalSpeed)-sqr(initialSpeed))/(2*acceleration) ;
             if (accDist>=length)
             {
                 //只有加速过程
                 t1 = (finalSpeed-initialSpeed)/acceleration;
                 t2 = t3 = 0;
             }
             else{
                //有加速过程和减速过程
                 double vmSqr = (2*acceleration*deceleration*length+deceleration*sqr(initialSpeed)+acceleration*sqr(finalSpeed))/(deceleration+acceleration);
                 double vm = sqrt(vmSqr);
                 t1 = (vm-initialSpeed)/acceleration;
                 t2 = 0;
                 t3 = (vm-finalSpeed)/deceleration;
             }
         }
         else
         {
             double decDist = abs(sqr(finalSpeed)-sqr(initialSpeed))/(2*deceleration) ;
             if (decDist>=length)
             {
                 //只有减速过程
                 t1 = t2 = 0;
                 t3 = (initialSpeed - finalSpeed)/deceleration;
             }
             else{
                 //有加速和减速过程
                 double vmSqr = (2*acceleration*deceleration*length+deceleration*sqr(initialSpeed)+acceleration*sqr(finalSpeed))/(deceleration+acceleration);
                 double vm = sqrt(vmSqr);
                 t1 = (vm-initialSpeed)/acceleration;
                 t2 = 0;
                 t3 = (vm-finalSpeed)/deceleration;
             }
         }
     }
     double totalTime = t1+t2+t3;
     int tfint = (int)ceil(PARAM::Vision::FRAME_RATE*totalTime);
     int maxStepNumber = min(tfint, 1);
     for(int i=0;i<=maxStepNumber;++i){
         if (i<=t1*PARAM::Vision::FRAME_RATE)
         {
             vz = initialSpeed+acceleration*i/PARAM::Vision::FRAME_RATE	;
         }else if (i<=(t1+t2)*PARAM::Vision::FRAME_RATE){
             vz = maxSpeed;
         }else{
            vz = initialSpeed-deceleration*i/PARAM::Vision::FRAME_RATE;
         }
     }
     if (vz>maxSpeed)
     {
         return maxSpeed;
     }
     return vz;
}
