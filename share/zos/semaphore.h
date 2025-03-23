#ifndef __ZOS_SEMAPHORE_H__
#define __ZOS_SEMAPHORE_H__

#include "zos/interface.h"
#include "zos/config.h"

#include <mutex>
#include <condition_variable>
namespace zos{
template<unsigned int _max_capacity=std::numeric_limits<unsigned int>::max()>
class SemaphoreT:public ISema {
public:
    SemaphoreT(unsigned int count = 0)
        : count_(count){
        #ifdef ZOS_DEBUG
        zos::log("{} SemaphoreT constructor,capa : {}\n",fmt::ptr(this),_max_capacity);
        #endif
    }
    SemaphoreT(const SemaphoreT& s):count_(0){
        #ifdef ZOS_DEBUG
        zos::log("{} SemaphoreT copy ctor\n",fmt::ptr(this));
        #endif
    }
    virtual ~SemaphoreT(){
//        notify_current();
        #ifdef ZOS_DEBUG
        zos::log("{} SemaphoreT destructor\n",fmt::ptr(this));
        #endif
    }
//    virtual void notify_current() override{
//        std::scoped_lock lock(mutex_);
//        count_ = 99999;
//        cv_.notify_all();
//    }
    virtual void release() override {
        std::scoped_lock lock(mutex_);
        if (count_ < _max_capacity){
            count_++;
            cv_.notify_one();
        }
    }
    virtual void acquire() override {
        std::unique_lock lock(mutex_);
        cv_.wait(lock, [=,this] { return count_ > 0; });
        --count_;
    }
    virtual bool try_acquire() override{
        return try_acquire_for(0);
    }
    template<typename _Rep,typename _Period>
    bool try_acquire_for(const std::chrono::duration<_Rep, _Period>& rel_time) {
        std::unique_lock lock(mutex_);
        auto res = cv_.wait_for(lock, rel_time, [=,this] { return count_ > 0; });
        if(count_ > 0){
            --count_;
            return true;
        }
        return false;
    }
    virtual bool try_acquire_for(unsigned int c) override{
        return try_acquire_for(c*__timestep);
    }
private:
    std::mutex mutex_;
    std::condition_variable cv_;
    unsigned int count_;
};
}// namespace zos
#endif // __ZOS_SEMAPHORE_H__