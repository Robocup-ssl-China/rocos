#ifndef __ZOS_DATANODE_H__
#define __ZOS_DATANODE_H__
#include <cstdlib>

#ifdef ZOS_DEBUG
#include "zos/log.h"
#endif

namespace zos{
class DataNode{
public:
    DataNode(DataNode* _last=nullptr,DataNode* _next=nullptr):_last(_last),_next(_next),_data(nullptr),_capacity(0),_size(0){
        #ifdef ZOS_DEBUG
        zos::log("{} ZOS DataNode constructor\n",fmt::ptr(this));
        #endif
    }
    DataNode(const DataNode&)=delete;
    DataNode(DataNode&&)=default;
    virtual ~DataNode(){
        #ifdef ZOS_DEBUG
        zos::log("{} ZOS DataNode destructor\n",fmt::ptr(this));
        #endif
        if(_capacity > 0){
            free(_data);
        }
    }
    virtual void resize(const unsigned long size){
        if(size > _capacity){
            if(_data != nullptr){
                free(_data);
            }
            _data = malloc(size);
            _capacity = size;
        }
        _size = size;
    }
    DataNode* _last;
    DataNode* _next;
    void* _data;
    unsigned long _capacity;
    unsigned long _size;
};
}
#endif // __ZOS_DATANODE_H__
