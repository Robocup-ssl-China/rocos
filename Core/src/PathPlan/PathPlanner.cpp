#include "PathPlanner.h"
//#include "AStarPathPlanner.h"
//#include "StupidPathPlanner.h"
#include "RRTPathPlanner.h"
//#include "BangPathPlanner.h"
//#include "PfadPathPlanner.h"
//#include "DelaunayPathPlanner.h"
//#include "NewGeoPathPlanner.h"

	int CPathPlanner::_obstacle_masks[5] = {0,0,0,0,0}; 

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	CPathPlanner::CPathPlanner(const CVisionModule* pVision, const int player,
/// 	const CGeoPoint& target, const int flags, const double avoidLength,
/// 	const PlanType planType)
///
/// @brief	Constructor. 
///
/// @author	cliffyin(cliffyin@zju.edu.cn)
/// @date	2010-3-25
///
/// @param	pVision		指向视觉输入模块的指针.
/// @param	player		路径规划针对的小车,以小车所在位置为规划起点.
/// @param	target		路径规划的终点.
/// @param	flags		传入的一些标签.
/// @param	avoidLength	避碰的长度，指考虑小车自身半径之外的距离.
/// @param	planType	选择的规划方式,RRT GEO等,默认方式是GEO.
////////////////////////////////////////////////////////////////////////////////////////////////////

CPathPlanner::CPathPlanner(const CVisionModule* pVision,const TaskT& task, obstacles& obs, const PlanType planType, const CGeoPoint& myPos, bool searchInCircle, CGeoPoint circleCenter, double circleRadius)
{

	switch( planType )
	{
	case RRT:
        *this = CRRTPathPlanner(pVision, task, obs, myPos, searchInCircle, circleCenter, circleRadius);
		break;
	/*case GEO:
		*this = CGeoPathPlanner(pVision,task.executor,task.player.pos,task.player.flag,avoidLength);
		break;
	case ASTAR:
		*this = CAStarPathPlanner(pVision,task.executor,task.player.pos,task.player.flag,avoidLength);
	    break;
	case STUPID:
		*this = CStupidPathPlanner(pVision,task.executor,task.player.pos,task.player.flag,avoidLength);
	    break;
	case BANGBANG:
		*this = CBangPathPlanner(pVision,task.executor,task.player.pos,task.player.flag,avoidLength);
		break;
	case PFAD:
		*this = CPfadPathPlanner(pVision,task.executor,task.player.pos,task.player.flag,avoidLength);
		break;
	case DELAUNAY:
		*this = CDelaunayPathPlanner(pVision,task.executor,task.player.pos,task.player.flag,avoidLength);
		break;	
	case NEWGEO:
		*this = CNewGeoPathPlanner(pVision,task.executor,task.player.pos,task.player.flag,avoidLength);
		break;	*/

	default:
		printf("Default PathPlanner!\n");
        *this = CRRTPathPlanner(pVision, task, obs, myPos, searchInCircle, circleCenter, circleRadius);
	    break;
	}
	//*this = realPlanner;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	void CPathPlanner::resetObstacleMask()
///
/// @brief	重置障碍物屏蔽掩码
///
/// @author	cliffyin(cliffyin@zju.edu.cn)
/// @date	2010-3-25
////////////////////////////////////////////////////////////////////////////////////////////////////

void CPathPlanner::resetObstacleMask()
{
	for (int i=0; i<5; i++)
		_obstacle_masks[i] = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	void CPathPlanner::setObstacleMask(int robotid, int mask_code)
///
/// @brief	给第一个参数所指定的小车设置屏蔽掩码. 
///
/// @author	Cliffyin(cliffyin Zju.edu.cn)
/// @date	2010-3-25
///
/// @param	robotid		小车的车号(0-4). 
/// @param	mask_code	屏蔽掩码. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void CPathPlanner::setObstacleMask(int robotid, int mask_code)
{
	_obstacle_masks[robotid-1] |= mask_code;
}
