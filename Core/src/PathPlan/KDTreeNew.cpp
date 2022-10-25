#include "KDTreeNew.h"

namespace {
const int NO_CHILD = -1;

}

inline bool KDTreeNew::insideRegion(KDNode* node, const stateNew& s) {
    if(node == nullptr) {
        cout << "Oh shit!!! Node is nullptr!!! ---KDTreeNew.cpp" << endl;
        return false;
    }
    return (s.pos.x() > node->_lowerLeftBound.x() && s.pos.y() > node->_lowerLeftBound.y()
            && s.pos.x() < node->_upperRightBound.x() && s.pos.y() < node->_upperRightBound.y());
}

inline double KDTreeNew::distPointToRectRegin(KDNode* node, const CGeoPoint& pos) {
    double boundedX = pos.x();
    double boundedY = pos.y();
    if(boundedX > node->_upperRightBound.x())
        boundedX = node->_upperRightBound.x();
    else if(boundedX < node->_lowerLeftBound.x())
        boundedX = node->_lowerLeftBound.x();
    if(boundedY > node->_upperRightBound.y())
        boundedY = node->_upperRightBound.y();
    else if(boundedY < node->_lowerLeftBound.y())
        boundedY = node->_lowerLeftBound.y();
    double dx = pos.x() - boundedX;
    double dy = pos.y() - boundedY;
    return sqrt(dx * dx + dy * dy);
}

bool KDTreeNew::nodeInTree(stateNew nodeState) {
    if(_tree.empty() || _tree.front() == nullptr) {
        cout << "Oh shit!!! KDTree not initialized!!! ---KDTreeNew.cpp!!!" << endl;
        return false;
    }
    return insideRegion(_tree.front(), nodeState);
}

void KDTreeNew::splitNode(KDNode* node, int splitDim) {
    if(splitDim != 0 && splitDim != 1) {
        cout << "Oh shit!!! Bad splitNum ---KDTreeNew.cpp!!!" << endl;
        return;
    } else if(node == nullptr) {
        cout << "Oh shit!!! Nullptr!!! ---KDTreeNew.cpp!!!" << endl;
        return;
    }
    double midBound = 0;
    KDNode* first = _nodeAllocator.fastMalloc();
    KDNode* second = _nodeAllocator.fastMalloc();
    first->_children.first = NO_CHILD;
    first->_children.second = NO_CHILD;
    second->_children.first = NO_CHILD;
    second->_children.second = NO_CHILD;
    first->_leaves.clear();
    first->_leaves.reserve(_maxLeafSizePerNode);
    second->_leaves.clear();
    second->_leaves.reserve(_maxLeafSizePerNode);
    first->_lowerLeftBound = second->_lowerLeftBound = node->_lowerLeftBound;
    first->_upperRightBound = second->_upperRightBound = node->_upperRightBound;

    if(splitDim == 0) {
        midBound = (node->_lowerLeftBound.x() + node->_upperRightBound.x()) / 2.0;
        first->_upperRightBound.setX(midBound);
        second->_lowerLeftBound.setX(midBound);
    } else {
        midBound = (node->_lowerLeftBound.y() + node->_upperRightBound.y()) / 2.0;
        first->_upperRightBound.setY(midBound);
        second->_lowerLeftBound.setY(midBound);
    }
    for(pair< stateNew, size_t >* iter : node->_leaves) {
        if(iter == nullptr) {
            cout << "Oh shit!!! KDTreeNew has nullptr leaf!!! ---KDTreeNew.cpp" << endl;
            return;
        }
        if((splitDim == 0 ? iter->first.pos.x() : iter->first.pos.y()) < midBound) {
            first->_leaves.push_back(iter);
        } else {
            second->_leaves.push_back(iter);
        }
    }
    node->_leaves.clear();
    node->_leaves.shrink_to_fit();
    node->_children.first = _tree.size();
    node->_children.second = _tree.size() + 1;
    _tree.push_back(first);
    _tree.push_back(second);
    return;
}

bool KDTreeNew::initTree(const CGeoPoint& lowerLeftBound, const CGeoPoint& upperRightBound, int maxLeafSizePerNode, int maxTreeDepth) {
    this->_maxLeafSizePerNode = maxLeafSizePerNode;
    this->_maxTreeDepth = maxTreeDepth;
    KDNode* root = _nodeAllocator.fastMalloc();
    if(root == nullptr) return false;
    root->_children.first = NO_CHILD;
    root->_children.second = NO_CHILD;
    root->_lowerLeftBound = lowerLeftBound;
    root->_upperRightBound = upperRightBound;
    root->_leaves.clear();
    root->_leaves.reserve(_maxLeafSizePerNode);
    _tree.clear();
    _tree.reserve(pow(2, maxTreeDepth) - 1);
    _tree.push_back(root);
    return true;
}

bool KDTreeNew::addNode(const stateNew& s, const size_t selfIdx) {
    if(_tree.empty() || _tree.front() == nullptr) {
        cout << "Oh shit!!! KDTreeNew hasn't be initialized!!! ---KDTreeNew.cpp --addNode Func" << endl;
        return false;
    } else if(!insideRegion(_tree.front(), s)) {
//        GDebugEngine::Instance()->gui_debug_msg(s.pos, QString("Oh shit!!! State is out of KDTreeNew field!!! ---KDTreeNew.cpp").toLatin1(), COLOR_GREEN);
//        cout << "Oh shit!!! State is out of KDTreeNew field!!! ---KDTreeNew.cpp"  << s.pos << endl;
        return false;
    }
    int depth = 0;
    pair< stateNew, size_t >* leafNew = _leafAllocator.fastMalloc();
    leafNew->first = s;
    leafNew->second = selfIdx;
    KDNode* p = _tree.front(); ///p initialized to be the root
    while(p->_children.first != NO_CHILD && p->_children.second != NO_CHILD) {
        int nextChild = insideRegion(_tree[p->_children.first], s) ? p->_children.first : p->_children.second;
        p = _tree[nextChild];
        depth++;
    }
    p->_leaves.push_back(leafNew);
    if(p->_leaves.size() > _maxLeafSizePerNode && depth < _maxTreeDepth)
        splitNode(p, depth % 2);
    return true;
}

void KDTreeNew::NearestNode(int &idx, KDNode *node, double &bestDist, const CGeoPoint &pos) {
    double childDist[2];
    double dist;

    if(node->_children.first == NO_CHILD) {
        for(pair < stateNew, size_t >* iter : node->_leaves) {
            dist = iter->first.pos.dist(pos);
            if(dist < bestDist) {
                bestDist = dist;
                idx = iter->second;
            }
        }
    } else {
        childDist[0] = distPointToRectRegin(_tree[node->_children.first], pos);
        childDist[1] = distPointToRectRegin(_tree[node->_children.second], pos);

        if(childDist[0] < childDist[1]) {
            if(childDist[0] < bestDist) NearestNode(idx, _tree[node->_children.first], bestDist, pos);
            if(childDist[1] < bestDist) NearestNode(idx, _tree[node->_children.second], bestDist, pos);
        } else {
            if(childDist[1] < bestDist) NearestNode(idx, _tree[node->_children.second], bestDist, pos);
            if(childDist[0] < bestDist) NearestNode(idx, _tree[node->_children.first], bestDist, pos);
        }
    }
    return;
}

int KDTreeNew::indexOfNearestNode(double &dist, const CGeoPoint &pos) {
    int index = 0;
    dist = 1e8;
    if(_tree.empty()) {
        cout << "Oh shit!!! KDTree hasn't be initialized!!! ---KDTree.cpp --indexOfNearestNode Func" << endl;
        return 0;
    }
    NearestNode(index, _tree[0], dist, pos);
    return index;
}

void KDTreeNew::clearTree() {
    if(_tree.empty()) {
        cout << "Oh shit!!! KDTree hasn't be initialized!!! ---KDTree.cpp --clearTree Func" << endl;
        return;
    }
    while(_tree.size() > 1) {
        KDNode* p = _tree.back();
        _leafAllocator.freeVec(p->_leaves);
        p->_leaves.clear();
        _nodeAllocator.free(p);
        _tree.pop_back();
    }
    _leafAllocator.freeVec(_tree[0]->_leaves);
    _tree[0]->_leaves.clear();
    _tree[0]->_children.first = NO_CHILD;
    _tree[0]->_children.second = NO_CHILD;

}
