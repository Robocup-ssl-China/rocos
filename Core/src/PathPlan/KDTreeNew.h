/**************************************
* 新版KDTree， 优化新版RRT
* Author: Jia Shenhan
* Created Date: 2019/5/17
**************************************/

#ifndef KDTreeNew_H
#define KDTreeNew_H
#include <ObstacleNew.h>
#include <geometry.h>
#include <GDebugEngine.h>
#include <FastAllocator.h>
#include <utility>


typedef struct {
    CGeoPoint _lowerLeftBound, _upperRightBound;
    vector < pair < stateNew, size_t >* > _leaves; ///self pos and self index
    pair < size_t, size_t > _children;  ///index of the first child and the second child
} KDNode;

class KDTreeNew {
    vector < KDNode* > _tree;
    int _maxLeafSizePerNode, _maxTreeDepth;
    fastAllocator < KDNode > _nodeAllocator;
    fastAllocator < pair < stateNew, size_t > > _leafAllocator;
  private: /// private functions
    bool insideRegion(KDNode* node, const stateNew& s);
    double distPointToRectRegin(KDNode* node, const CGeoPoint& pos);
    void splitNode(KDNode* node, int splitDim);
    void NearestNode(int& idx, KDNode* node, double &bestDist, const CGeoPoint &pos);
  public:
    KDTreeNew() {}
    bool initTree(const CGeoPoint& lowerLeftBound, const CGeoPoint& upperRightBound, int maxLeafSizePerNode, int maxTreeDepth);
    bool nodeInTree(stateNew nodeState);
    int indexOfNearestNode(double &dist, const CGeoPoint& pos);
    bool addNode(const stateNew& s, const size_t selfIdx);
    void clearTree();
};




#endif // KDTreeNew_H
