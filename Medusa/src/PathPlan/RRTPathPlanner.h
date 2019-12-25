#ifndef _RRT_PATH_PLANNER_H_
#define _RRT_PATH_PLANNER_H_
#include "PathPlanner.h"
#include <VisionModule.h>
/**
* CRRTPathPlanner.
* CMU's Random Tree path planner.
*/
class CRRTPathPlanner : public CPathPlanner{
public:
    CRRTPathPlanner(const CVisionModule* pVision, const TaskT& task, obstacles& obs, const CGeoPoint& myPos = CGeoPoint(1e8, 1e8), bool searchInCircle = false, CGeoPoint circleCenter = CGeoPoint(0.0, 0.0), double circleRadius = 0.0);
};
#endif
