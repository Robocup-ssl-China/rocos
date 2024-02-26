#ifndef _ROBOT_CAPABILITY_H_
#define _ROBOT_CAPABILITY_H_
#include <geometry.h>
#include "staticparams.h"
#include <singleton.hpp>
#include <vector>
/**
* 描述机器人的能力类.
*/

#define SHOOT_HIGHEST				200
#define SHOOT_BELOW_HIGHEST			150
#define SHOOT_HIGHER				130
#define SHOOT_ABOVE_NORAML			110
#define SHOOT_NORAML				90
#define SHOOT_BELOW_NORAML			70
#define SHOOT_ABOVE_LOWEST			50
#define SHOOT_LOWEST				0

#define DRIBBLE_HIGHEST				3
#define DRIBBLE_BELOW_HIGHEST		3
#define DRIBBLE_ABOVE_NORAML		3
#define DRIBBLE_NORAML				2
#define DRIBBLE_BELOW_NORAML		2
#define DRIBBLE_ABOVE_LOWEST		1
#define DRIBBLE_LOWEST				1
#define DRIBBLE_DISABLED			0

// 对应控球反转
#define FLIP_DRIBBLE_LOW            5
#define FLIP_DRIBBLE_NORMAL         6
#define FLIP_DRIBBEL_HIGH           7


class CRobotCapability{
public:
	// 击球能力
	virtual double minShootAngleWidth() const = 0; // 最小射门角度范围

	// 挑球能力
	virtual double minChipKickObstacleDist() const = 0; // 挑球障碍物最近距离
	virtual double maxChipKickDist() const = 0; // 最远挑球距离
	virtual double maxChipShootDist() const = 0; // 最大挑射距离
	virtual double minChipShootExtra(const double dist) const = 0; // 挑球射门过顶的额外距离
	
	virtual double maxSpeedGoalie(const double angle) const = 0;
	virtual double maxAccelerationGoalie(const CVector& velocity, const double angle) const = 0;
	virtual double maxDecGoalie(const CVector& velocity, const double angle) const = 0;

	virtual double maxSpeedBack(const double angle) const = 0;
	virtual double maxAccelerationBack(const CVector& velocity, const double angle) const = 0;
	virtual double maxDecBack(const CVector& velocity, const double angle) const = 0;

	// 运动能力
	virtual double maxAcceleration(const CVector& velocity, const double angle) const = 0; // 某个方向的最大加速度
	virtual double maxAngularAcceleration() const = 0;
  virtual double maxDec(const CVector& velocity, const double angle) const = 0; // 某个方向的最大减速度
  virtual double maxAngularDec() const = 0;  //   z
	virtual double maxSpeed(const double angle) const = 0; // 某个方向的最大速度
	virtual double maxAngularSpeed() const = 0; // 最大旋转速度

    virtual double maxRushTranslationSpeed() const = 0;
	
	virtual ~CRobotCapability(){ } // 虚析构函数
};
class CRobotCapFactory{
public:
	CRobotCapFactory();
	const CRobotCapability* getRobotCap(const int side, const int num)
	{
		if( side == PARAM::Field::POS_SIDE_LEFT ){
			return _robotCaps[LeftRobotType[num]];
		}
		return _robotCaps[RightRobotType[num]];
	}
	~CRobotCapFactory()
	{
		for( int i=0; i<_robotCaps.size(); ++i ){
			delete _robotCaps[i];
		}
	}
private:
	std::vector< CRobotCapability* > _robotCaps;
	int LeftRobotType[PARAM::Field::MAX_PLAYER];
	int RightRobotType[PARAM::Field::MAX_PLAYER];
};
typedef Singleton< CRobotCapFactory > RobotCapFactory; // 声明为Singleton
#endif // _ROBOT_CAPABILITY_H_
