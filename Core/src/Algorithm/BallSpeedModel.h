#ifndef _BALL_SPEED_MODEL_H_
#define _BALL_SPEED_MODEL_H_
#include <tuple>
#include "singleton.hpp"
#include "WorldDefine.h"

class CVisionModule;
class CBallSpeedModel
{
public:
	CBallSpeedModel();
	~CBallSpeedModel();
	inline void registerVision(CVisionModule *vision) { this->vision = vision; }
	std::tuple<double,CVector> predictForDist(const double dist);	// 计算若干距离后的速度
	double timeForDist(const double dist);      //计算球运动若干距离的时间
	CVector speedForDist(const double dist);	// 计算球运动若干距离的速度
	ObjectPoseT poseForTime(const double time); // 计算若干帧以后的绝对位置
private:
	void update();
	CGeoPoint _ballPos;
	CVector _ballVel;
	int lastCycle = 0;
	CVisionModule *vision = nullptr;
};
typedef Singleton< CBallSpeedModel > BallSpeedModel;
#endif // _BALL_SPEED_MODEL_H_
