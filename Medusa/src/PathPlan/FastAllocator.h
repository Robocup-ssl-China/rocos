/**************************************
* 快速内存申请释放，减小开销
* Author: Jia Shenhan
* Created Date: 2019/5/17
**************************************/
#ifndef FASTALLOCATOR_H
#define FASTALLOCATOR_H
#include <ObstacleNew.h>

const int MAX_ALLOCATOR_LENGTH = 400;

template < class item >
class fastAllocator {
    vector < item* > _freedList;
    int _maxListLength;
public:
    fastAllocator():_maxListLength(MAX_ALLOCATOR_LENGTH) {}
    ~fastAllocator();

    item* fastMalloc();
    void free(item* its);
    void freeVec(vector < item* > itsVec);
};


template < class item >
fastAllocator < item >::~fastAllocator() {
    for(item* iter : _freedList)
        delete iter;
    vector < item* >().swap(_freedList);
}

template < class item >
item* fastAllocator < item >::fastMalloc() {
    item* ptr;
    if(_freedList.empty()) {
        ptr = new item;
    } else {
        ptr = _freedList.back();
        _freedList.pop_back();
    }
    return ptr;
}

template < class item >
void fastAllocator < item >::free(item *its) {
    if(_freedList.size() < _maxListLength)
        _freedList.push_back(its);
    else
        delete its;
    return;
}

template < class item >
void fastAllocator < item >::freeVec(vector < item* > itsVec) {
    item* iter;
    while(!itsVec.empty()) {
        if(_freedList.size() >= _maxListLength) break;
        iter = itsVec.back();
        _freedList.push_back(iter);
        itsVec.pop_back();
    }
    while(!itsVec.empty()) {
        iter = itsVec.back();
        delete iter;
        itsVec.pop_back();
    }

    return;
}
#endif // FASTALLOCATOR_H
