/************************************************************************/
/* Copyright (c) CSC-RL, Zhejiang University							*/
/* Team:		ZJUNlict												*/
/* HomePage:	http://www.nlict.zju.edu.cn/ssl/WelcomePage.html		*/
/************************************************************************/
/* File:	ControlModel.h												*/
/* Brief:	C++ Interface:	Motion Control Warpper						*/
/* Func:	Support calling motion control algorithms					*/
/* Author:	cliffyin, 2012, 04											*/
/* Refer:	some papers													*/
/* E-mail:	cliffyin@zju.edu.cn		cliffyin007@gmail.com				*/
/* Version:	0.0.1														*/
/* Changelog:	2012-04-10	create										*/
/************************************************************************/

#ifndef _CONTROL_MODEL_H_
#define _CONTROL_MODEL_H_

//////////////////////////////////////////////////////////////////////////
// include files
#include <WorldDefine.h>			// for RobotPose define
#include <vector>
#include <CMmotion.h>
using namespace std;

//////////////////////////////////////////////////////////////////////////
// define some enumerator
enum PTG_CTRL_MODE {
	DIFF = 0,
	OMNI,
};

//////////////////////////////////////////////////////////////////////////
// define the CControlModel class used to support motion control 
// similar to an external wrapper
class CControlModel {
public:
	/// BangBang control from Cornell : zero final velocity trajectory
	void makeZeroFinalVelocityPath(const PlayerPoseT& start, const PlayerPoseT& final, const PlayerCapabilityT& capability);

	/// BangBang control from Cornell : fast trajectory
	void makeFastPath(const PlayerPoseT& start, const PlayerPoseT& final, const PlayerCapabilityT& capability,const double finalVelDir = 0 );
	
	/// BangBang control from Cornell : proportional gain trajectory
	//void makeProportionalGainPath(const PlayerPoseT& start, const PlayerPoseT& final, const PlayerCapabilityT& capability);
	
	/// BangBang control from Cornell : zero final theta velocity
	void makeZeroFinalVelocityTheta(const PlayerPoseT& start, const PlayerPoseT& final, const PlayerCapabilityT& capability);
	
	/// BangBang control from Cornell : proportional gain theta trajectory
	//void makeProportionalGainTheta(const PlayerPoseT& start, const PlayerPoseT& final, const PlayerCapabilityT& capability);
	
	/// Trapezoidal control from CMU : none-zero final velocity trajectory
    void makeCmTrajectory(const PlayerPoseT& start, const PlayerPoseT& final, const PlayerCapabilityT& capability, nonZeroMode mode = FAST);

	/// Trapezoidal control from ZJU : zero final velocity trajectory
	void makeTrapezoidalVelocityPath(const PlayerPoseT& start, const PlayerPoseT& final, const PlayerCapabilityT& capability);
	
	/// Trapezoidal control from ZJU : none-zero final velocity trajectory
	void makeNoneTrapezoidalVelocityPath(const PlayerPoseT& start, const PlayerPoseT& final, const PlayerCapabilityT& capability);
	
	/// Parameterized control from ZJU : diff-omni zero final velocity trajectory
	// void makePTGTrajectory(const PlayerPoseT& start, const PlayerPoseT& final, const PlayerCapabilityT& capability, PTG_CTRL_MODE mode = DIFF);
	
	/// Get the real-time next step
	const PlayerPoseT& getNextStep() const { return _nextStep; }

	/// Get the real-time full trajectory
	const vector< vector<double> >& getFullPath() const { return _pathList;}

private:
	/// Next State for the robot includes velocities
	PlayerPoseT _nextStep;

	/// Full Trajectory for the robot
    vector< vector<double> > _pathList;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// some light-weight functions
/// x-axis BangBang control
double zeroFinalXSpeed(const double length, const double xSpeed, const double maxSpeed, const double maxAccel);

/// x-axis fast speed control
double fastXSpeed(const double length, const double xSpeed, const double maxSpeed, const double maxAccel);

/// angular speed control
double zeroFinalAngularSpeed(const double length, const double angSpeed, const double maxSpeed, const double maxAccel);

/// fast angular speed control
double fastAngularSpeed(const double length, const double angSpeed, const double maxSpeed, const double maxAccel);

/// none-zero final speed, 1d plan
double nonZeroFinalSpeed(const double length, const double Speed, const double maxSpeed, const double maxAccel);

/// trapezoidal velocity trajectory, 1d plan
double TrapezoidalVelocity(const double length,const double initialSpeed,const double maxSpeed,const double finalSpeed,const double maxAccel);

#endif // ~_CONTROL_MODEL_H_
