#ifndef _COLLISION_SIMULATOR_H_
#define _COLLISION_SIMULATOR_H_
#include "WorldDefine.h"
/**
* 碰撞模拟类,估计车和球的碰撞
*/
class CCollisionSimulator{
public:
	CCollisionSimulator() : _hasCollision(false){ }
	void reset(const CGeoPoint& ballRawPos, const CVector& ballVel);
	void simulate(PlayerVisionT robot, const double time); // 模拟一定时间
	const MobileVisionT& ball() const { return _ball; }
	bool hasCollision() const { return _hasCollision; }
private:
	MobileVisionT _ball; // 球的信息
	CVector _ballRelToRobot; // 球在机器人局部坐标系中的位置
	bool _hasCollision; // 是否碰撞
};
#endif // _COLLISION_SIMULATOR_H_