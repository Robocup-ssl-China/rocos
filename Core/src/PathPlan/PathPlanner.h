#ifndef _PATH_PLANNER_H_
#define _PATH_PLANNER_H_
#include <geometry.h>
#include <list>
#include "OtherLibs/cmu/obstacle.h"
#include "misc_types.h"
class CVisionModule;
// 障碍物类型
struct Obstacle{
	CGeoPoint pos; // 障碍物的位置
	CVector   vel; // 障碍物的速度
	double    radius; // 障碍物的半径
};
struct PathList : public std::list< CGeoPoint >{ // 路径
	PathList() : length(0){ }
    CGeoPoint  startNode;
	double length; // 路径长度
};
enum PlanType
{
	RRT = 0,
	GEO = 1,
	ASTAR = 2,
	STUPID = 3,
	BANGBANG = 4,
	PFAD = 5,
	DELAUNAY = 6,
	NEWGEO = 7,
};

// 障碍物屏蔽码
enum ObstacleMask{
	MASK_BALL = 0x0001,
	MASK_TEAMMATE0 = 0x0002,
	MASK_TEAMMATE1 = 0x0004,
	MASK_TEAMMATE2 = 0x0008,
	MASK_TEAMMATE3 = 0x0010,
	MASK_TEAMMATE4 = 0x0020,
	MASK_OPP0      = 0x0040,
	MASK_OPP1      = 0x0080,
	MASK_OPP2      = 0x0100,
	MASK_OPP3      = 0x0200,
	MASK_OPP4      = 0x0400
};

// 路径规划类接口

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	CPathPlanner{
///
/// @brief	所有路径规划器实现的接口类,构造函数是实现规划的统一接口.
///
/// @author	cliffyin(cliffyin@zju.edu.cn)
/// @date	2010-3-25
////////////////////////////////////////////////////////////////////////////////////////////////////

class CPathPlanner{
public:
	CPathPlanner(){ }
    CPathPlanner(const CVisionModule* pVision,const TaskT& task, obstacles& obs, const PlanType planType = RRT, const CGeoPoint& myPos = CGeoPoint(1e8, 1e8), bool searchInCircle = false, CGeoPoint circleCenter = CGeoPoint(0.0, 0.0), double circleRadius = 0.0);
	const PathList& getPath() const { return _path; }
	bool isPathValid() const{ return !_path.empty(); }
	static void resetObstacleMask();
	static void setObstacleMask(int robotid, int mask_code);
	static int _obstacle_masks[5];
    CVector get_vel(){return _vel;}
protected:
	PathList _path;
	CVector _vel;
};
#endif // _PATH_PLANNER_H_
