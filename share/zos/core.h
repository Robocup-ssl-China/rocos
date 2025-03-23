#ifndef ZOSCORE_H
#define ZOSCORE_H
#include <string>
#include <set>

#include <typeinfo>

#include <iostream>

#include "zos/utils/threadpool.h"
#include "zos/utils/singleton.h"
#include "zos/config.h"
#include "zos/token.h"
#include "zos/semadata.h"
#include "zos/log.h"
#include "zos/type.h"
#include "zos/socket.h"

namespace zos{
using __callback_type = zos::type::callback_type;
class SubscriberBase;
class Publisher;
class Pool:public Singleton<Pool>,public ThreadPool{
public:
    Pool():ThreadPool(zos::config::threadpool_nums){}
    friend SubscriberBase;
};
class SubscriberBase{
public:
    SubscriberBase(const std::string& name,const std::string& msg_name,const __callback_type& f = {}):_data(nullptr),_name(name),_msg_name(msg_name){
        if(f){
            _callback = std::bind(f,std::placeholders::_1);
        }
    }
    // should be used when there's no callback functions
    void get(Data& data){
        _data->pop(data);
    }
    void get(Data* data=nullptr){
        _data->pop(data);
    }
    bool try_get(Data& data){
        auto res = _data->try_pop(data);
        return res;
    }
protected:
    bool add_task(){
        if(_callback){
            zos::Data data;
            auto res = try_get(data);
            #ifdef ZOS_DEBUG
            zos::log("add_task data:{},s:{}\n",fmt::ptr(&data),data.size());
            #endif
            if(!res){
                zos::log("error on function add_task, no data received.\n");
                return false;
            }
            Pool::_()->enqueue(_callback,std::move(data));
        }
        return true;
    }
    std::unique_ptr<ISemaData> _data;
    __callback_type _callback = {};
    const std::string _name;
    const std::string _msg_name;
    friend class Publisher;
};
template<unsigned int buffer_size=2>
class Subscriber:public SubscriberBase{
public:
    Subscriber(const std::string& name,const __callback_type& f = {}):Subscriber(name,name+"_msg",f){}
    Subscriber(const std::string& name,const std::string& msg_name,const __callback_type& f = {}):SubscriberBase(name,msg_name,f){
        _data = std::make_unique<SemaData<buffer_size>>();
    }
};
class Publisher{
public:
    Publisher(const std::string& name,const std::string& msg_name=""):_name(name),_msg_name(msg_name){
    }
    ~Publisher(){
    }
    void publish(const void* data = nullptr, const unsigned long size = 0){
        #ifdef ZOS_DEBUG
        zos::log("trigger publish -> receiver nums:{}\n",_subscribers.size());
        #endif
        std::shared_lock s_lock(_mutex_subscriber);
        for(auto s:_subscribers){
            s->_data->store(data,size);
            s->add_task();
        }
    }
    void publish(const Data& data){
        this->publish(data.data(),data.size());
    }
    template<typename... Ts>
    requires zos::concepts::are_convertiable<SubscriberBase*,Ts...>
    void link(Ts... subs){
        std::unique_lock u_lock(this->_mutex_subscriber);
        _subscribers.insert({static_cast<SubscriberBase*>(subs)...});
        #ifdef ZOS_DEBUG
        zos::log("Publisher link() : ",fmt::ptr(static_cast<SubscriberBase*>(subs))...);
        #endif
    }
    template<typename... Ts>
    requires zos::concepts::are_convertiable<SubscriberBase*,Ts...>
    void unlink(Ts... subs){
        std::unique_lock u_lock(this->_mutex_subscriber);
        _subscribers.erase({static_cast<SubscriberBase*>(subs)...});
        #ifdef ZOS_DEBUG
        zos::log("Publisher link() : ",fmt::ptr(static_cast<SubscriberBase*>(subs))...);
        #endif
    }
protected:
    std::set<SubscriberBase*> _subscribers = {};
    mutable std::shared_mutex _mutex_subscriber;
    const std::string _name;
    const std::string _msg_name;
};

} // namespace zos

#endif // ZOSCORE_H
