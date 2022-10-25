/**************************************
* 新版RRT， 用于全局路径搜索，兼容原RRT和SB-RRT FOR BEZIER
* Author: Jia Shenhan
* Created Date: 2019/5/15
**************************************/

#ifndef RRTPATHPLANNERNEW_H
#define RRTPATHPLANNERNEW_H
#include <ObstacleNew.h>
#include <geometry.h>
#include <PathPlanner.h>
#include <VisionModule.h>
#include <utility>
#include <KDTreeNew.h>

typedef enum
{
    finalTarget,
    wayPoint,
    randomState
} targetType;

struct backTrackIdx {
    int i;
    double length;
    bool operator < (backTrackIdx lAndIdx) { return length < lAndIdx.length; }
};

//后面要在跑点skill里面，比如smartGoto，对每一辆车都生成一个planner
class CRRTPathPlannerNew {
    bool _inited;
    size_t _maxNodesNum;
    size_t _maxWayPointsNum;
    size_t _maxPathPointsNum;

    double _targetChooseProb;
    double _wayPointsChooseProb;
    double _pathPointsAddToWayPointsProb;
    double _stepSize;
    double _findDist;

    vector < pair < stateNew, size_t > > _nodes; // node pos and parent's index
    vector < stateNew > _wayPoints;
    vector < stateNew > _pathPoints;

    vector < pair < stateNew, size_t > > _nodesRev; // node pos and parent's index
    vector < stateNew > _wayPointsRev;
    vector < stateNew > _pathPointsRev;

    stateNew _start;
    stateNew _target;

    ObstaclesNew *_obs;
    static KDTreeNew _searchTree;
    static KDTreeNew _searchTreeRev;

    double _searchAreaUpperBound;
    double _searchAreaLowerBound;
    double _searchAreaLeftBound;
    double _searchAreaRightBound;

    bool _searchInCircle;
    double _circleRadius;
    CGeoPoint _circleCenter;
private: //private functions
    stateNew generateRandomState(void);
    stateNew generateNodeStateAccordingToProb(targetType& tType);
    stateNew generateNodeStateAccordingToProbRev(targetType& tType);
    void addNode(stateNew node, size_t parent);
    void addNodeRev(stateNew node, size_t parent);
    int findParent(stateNew nodeToAdd);
    int findParentRev(stateNew nodeToAdd);
    void extend(size_t parent, stateNew nodeToAdd);
    void extendRev(size_t parent, stateNew nodeToAdd);
    void searchTreeGrow(size_t& nearestIdx);
    void searchTreeRevGrow(size_t& nearestIdx);
    void findBackTrackingIdx(vector < size_t >& idx, vector < size_t > &idxRev);
    void backTracking(size_t nearestNode, size_t nearestIdxToTree, size_t nearestNodeRev, size_t nearestIdxToTreeRev, int idx, vector < backTrackIdx >& lengthAndIdx);
    void backTracking(size_t nearestNode, size_t nearestIdxToTree, size_t nearestNodeRev, size_t nearestIdxToTreeRev);
    void addToWayPoints(void);
    void addToWayPointsRev(void);
public:
    CRRTPathPlannerNew();
    void initPlanner(int maxNodesNum, int maxWayPointsNum, int maxPathPointsNum,double targetChooseProb, double wayPointsChooseProb, double stepSize, bool planInOurField = false, bool searchInCircle = false, CGeoPoint circleCenter = CGeoPoint(0.0, 0.0), double circleRadius = 0.0); //only need to init once
    bool plannerInitted(void) const { return _inited; }
    void planPath(ObstaclesNew *obs, stateNew start, stateNew target);
    vector < stateNew > getPathPoints(void) const { return _pathPoints; }
    pair < vector < pair < stateNew, size_t > >, vector < pair < stateNew, size_t > > > getNodes(void) const { return pair < vector < pair < stateNew, size_t > >, vector < pair < stateNew, size_t > > >(_nodes, _nodesRev); }
};

#endif // RRTPATHPLANNERNEW_H
