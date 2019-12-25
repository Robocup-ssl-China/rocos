#ifndef _BALL_SPEED_MODEL_H_
#define _BALL_SPEED_MODEL_H_

#include "VisionModule.h"

class CBallSpeedModel{
public:
	CBallSpeedModel();
	~CBallSpeedModel();
	CVector speedForTime(double frame, const CVisionModule* pVision);   //计算若干帧以后的速度
	CVector speedForDist(double dist, const CVisionModule* pVision); //计算若干距离后的速度
	double timeForDist(double dist, const CVisionModule* pVision);      //计算球运动若干距离的时间
	CGeoPoint posForTime(double frame, const CVisionModule* pVision);   //计算若干帧以后的绝对位置
	void update( const CVisionModule* pVision );
//	double CalKickInitSpeed(const double dist);
private:
    CVector speedForTime_FM(double frame);   //计算若干帧以后的速度
    CVector speedForDist_FM(double dist); //计算若干距离后的速度
    double timeForDist_FM(double dist);      //计算球运动若干距离的时间
    CGeoPoint posForTime_FM(double frame);   //计算若干帧以后的绝对位置
private:
	CVector _ballVel;
//	CVector _lastBallVel;
	CGeoPoint _ballPos;
//	double cal_acc;
//	double cal_acc_high_speed;
//	double cal_acc_low_speed;
//	double _last_dist;
private:
	//add some private function to calculate ACC --hzy 2016.6.12 test
	//double getAcc(double speed) const;
//	const bool readTableFile_CheckTable(const string& filename);
//	const double timeForDist_CheckTable(const double dist) const;
//	const double distForTime_CheckTable(const int frame) const;
//	const double checkTime_CheckTable(const double const* _array,const double data) const;
//	const double checkData_CheckTable(const double const* _array,const double time) const;
//	double *_speed_data;
//	double *_dist_data;
//	int _num;
};
typedef NormalSingleton< CBallSpeedModel > BallSpeedModel;
#endif // _BALL_SPEED_MODEL_H_

