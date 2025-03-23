#ifndef __ZOS_DATA_H__
#define __ZOS_DATA_H__

#include <cstring>
#include <mutex>
#include <shared_mutex>

#include "zos/datanode.h"
#include "zos/interface.h"
namespace zos{
// thread unsafe base node
class Data:public IData,public DataNode{
public:
    Data():DataNode(){
        #ifdef ZOS_DEBUG
        zos::log("{} ZOS Data constructor\n",fmt::ptr(this));
        #endif
    }
    Data(const Data& data):DataNode(){
        #ifdef ZOS_DEBUG
        zos::log("{} COPY ZOS Data constructor from {}\n",fmt::ptr(this),fmt::ptr(&data));
        #endif
        resize(data._size);
        if(data._size > 0)
            memcpy(_data,data._data,data._size);
    }
    Data(Data&& data){
        #ifdef ZOS_DEBUG
        zos::log("{} MOVE ZOS Data constructor from {}\n",fmt::ptr(this),fmt::ptr(&data));
        #endif
        std::unique_lock<std::shared_mutex> lock(data._mutex);
        _last = data._last;
        _next = data._next;
        _data = data._data;
        _capacity = data._capacity;
        _size = data._size;
        data._data = nullptr;
        data._capacity = data._size = 0;
    }
    virtual ~Data(){
        #ifdef ZOS_DEBUG
        zos::log("{} ZOS Data destructor\n",fmt::ptr(this));
        #endif
    }
    // self thread-safe
    virtual int size() const {
        std::shared_lock<std::shared_mutex> lock(_mutex);
        return _size;
    }
    virtual void pop(Data& p) override{
        std::unique_lock<std::shared_mutex> lock(_mutex);
        p.store(this->data(),this->_size);
        resize(0);
    }
    virtual void pop(Data* p) override{
        std::unique_lock<std::shared_mutex> lock(_mutex);
        p->store(this->data(),this->size());
        resize(0);
    }
    virtual void copyTo(Data* p){
        std::shared_lock<std::shared_mutex> lock(_mutex);
        p->store(this->data(),this->_size);
    }
    virtual void store(const Data& data){
        store(data.data(),data._size);
    }
    virtual void store(const Data* data){
        store(data->data(),data->_size);
    }
    virtual void store(const void* const data,unsigned long size) override{
        std::unique_lock<std::shared_mutex> lock(_mutex);
        resize(size);
        if(size > 0)
            memcpy(_data,data,size);
    }
    // thread-unsafe
    virtual const void* data() const { return _data; }
    virtual void* ptr() {
        return _data;
    }
protected:
    mutable std::shared_mutex _mutex;
};

}
#endif // __ZOS_DATA_H__
