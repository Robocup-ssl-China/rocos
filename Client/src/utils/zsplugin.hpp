/************************************************
 * Author : mark
 * Email : hzypp1995@gmail.com
 * Lab : Zhejiang University ZJUNlict
 * Introduction : 
 *     (Semaphore_base)────┐             ┌──────(ZSData_base) 
 *        /        \       |             |     /      \    
 *    (SemaO)    (SemaM)  (ZSSemaDavta_Base)  (ZSData) (ZSDataQueue)
 *       |          |          /     \         |            |
 *       |          ├(ZSemaData NoFS)  (ZSemaData FS)       |
 *       └──────────│──────────────────────────┘            |
 *                  └───────────────────────────────────────┘
 ***************************************************/

#ifndef __ZSS_PLUGIN_H__
#define __ZSS_PLUGIN_H__
#include <iostream>
#include <string>
#include <thread>
#include <list>
#include <map>
#include <shared_mutex>
#include <mutex>
#include <cstring>
#include <condition_variable>

class Semaphore_base {
public:
    Semaphore_base(){
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << " Semaphore_base constructor" << std::endl;
        #endif
    }
    virtual ~Semaphore_base(){
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << " Semaphore_base destructor" << std::endl;
        #endif
    }
    virtual void signal(unsigned int c) = 0;
    virtual void wait() = 0;
    virtual bool try_wait() = 0;
};
class SemaphoreM:public Semaphore_base {
public:
    SemaphoreM(long count = 0)
        : count_(count) {
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << " SemaphoreM constructor" << std::endl;
        #endif
    }
    SemaphoreM(const SemaphoreM& s):count_(0){
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << " SemaphoreM copy constructor" << std::endl;
        #endif
    }
    virtual ~SemaphoreM(){
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << " SemaphoreM destructor" << std::endl;
        #endif
    }
    virtual void signal(unsigned int c = 1) override {
        std::unique_lock<std::mutex> lock(mutex_);
        count_+=c;
        cv_.notify_one();
    }
    virtual void wait() override {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [=] { return count_ > 0; });
        --count_;
    }
    virtual bool try_wait() override {
        std::unique_lock<std::mutex> lock(mutex_);
        if(count_ > 0){
            --count_;
            return true;
        }
        return false;
    }
private:
    std::mutex mutex_;
    std::condition_variable cv_;
    long count_;
};

class SemaphoreO:public Semaphore_base {
public:
    SemaphoreO(long count = 0)
        : count_(count) {
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << " SemaphoreO constructor" << std::endl;
        #endif
    }
    SemaphoreO(const SemaphoreO& s):count_(0){
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << " SemaphoreO copy constructor" << std::endl;
        #endif
    }
    virtual ~SemaphoreO(){
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << " SemaphoreO destructor" << std::endl;
        #endif
    }
    virtual void signal(unsigned int c = 1) override {
        std::unique_lock<std::mutex> lock(mutex_);
        count_=c;
        cv_.notify_one();
    }

    virtual void wait() override {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [=] { return count_ > 0; });
        --count_;
    }

    virtual bool try_wait() override {
        std::unique_lock<std::mutex> lock(mutex_);
        if(count_ > 0){
            --count_;
            return true;
        }
        return false;
    }
private:
    std::mutex mutex_;
    std::condition_variable cv_;
    long count_;
};
class ZSData;
class ZSData_base{
public:
    ZSData_base(){
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << " ZSData_base constructor" << std::endl;
        #endif
    }
    virtual ~ZSData_base(){
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << " ZSData_base destructor" << std::endl;
        #endif
    }
    virtual void popTo(ZSData&) = 0;
    virtual void store(const void* const data,unsigned long size) = 0;
};

// thread unsafe base node
class ZSDataNode{
public:
    ZSDataNode():_last(nullptr),_next(nullptr),_data(nullptr),_capacity(0),_size(0){
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << " ZSDataNode constructor" << std::endl;
        #endif
    }
    virtual ~ZSDataNode(){
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << " ZSDataNode destructor" << std::endl;
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
    ZSDataNode* _last;
    ZSDataNode* _next;
    void* _data;
    unsigned long _capacity;
    unsigned long _size;
};
class ZSData:public ZSData_base,public ZSDataNode{
public:
    ZSData():ZSDataNode(){
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << " ZSData constructor" << std::endl;
        #endif
    }
    ZSData(const ZSData& data){
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << " ZSData copy constructor" << std::endl;
        #endif
        store(data);
    }
    virtual ~ZSData(){
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << " ZSData destructor" << std::endl;
        #endif
    }
    // self thread-safe
    virtual int size() const {
        std::shared_lock<std::shared_mutex> lock(_mutex);
        return _size;
    }
    virtual void popTo(ZSData& p){
        std::unique_lock<std::shared_mutex> lock(_mutex);
        p.store(this->data(),this->_size);
        resize(0);
    }
    virtual void copyTo(ZSData* p){
        std::shared_lock<std::shared_mutex> lock(_mutex);
        p->store(this->data(),this->_size);
    }
    virtual void store(const ZSData& data){
        store(data.data(),data._size);
    }
    virtual void store(const ZSData* data){
        store(data->data(),data->_size);
    }
    virtual void store(const void* const data,unsigned long size) override{
        std::unique_lock<std::shared_mutex> lock(_mutex);
        resize(size);
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

class ZSDataQueue:public ZSData_base{
public:
    ZSDataQueue():_start(new ZSDataNode()),_end(_start),_capacity(1),_size(0){
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << " ZSDataQueue constructor" << std::endl;
        #endif
        _start->_last = _start;
        _start->_next = _start;
    }
    virtual ~ZSDataQueue(){
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << " ZSDataQueue destructor" << std::endl;
        #endif
        while(_capacity>1){
            _start = _start->_next;
            delete _start->_last;
            _capacity--;
        }
        delete _start;
    }
    virtual void popTo(ZSData& p){
        std::unique_lock<std::shared_mutex> lock(_mutex);
        p.store(_start->_data,_start->_size);
        _start = _start->_next;
        _size--;
        #ifdef ZSPLUGIN_DEBUG
            std::cout << this << " ZSDataQueue popTo()" << std::endl;
            if(_size < 0){
                std::cout << this << "size < 0 after popTo(" << &p << "). _size=" << _size << ", _capacity=" << _capacity << std::endl; 
            }
        #endif
    }
    virtual void store(const ZSData& data){
        store(data.data(),data._size);
    }
    virtual void store(const ZSData* data){
        store(data->data(),data->_size);
    }
    virtual void store(const void* const data,unsigned long size){
        std::unique_lock<std::shared_mutex> lock(_mutex);
        ZSDataNode* storeNode = nullptr;
        if(_size < _capacity){
            storeNode = _end;
            _end = _end->_next;
        }else{
            storeNode = new ZSDataNode();
            storeNode->_next = _end;
            storeNode->_last = _end->_last;
            _end->_last->_next = storeNode;
            _end->_last = storeNode;
            _capacity++;
        }
        storeNode->resize(size);
        memcpy(storeNode->_data,data,size);
        _size++;
        #ifdef ZSPLUGIN_DEBUG
            std::cout << this << " ZSDataQueue store() " << storeNode << ' ' << data << ", size=" << _size << ", capacity=" << _capacity << std::endl;
        #endif
    }
    virtual unsigned long size(){
        std::shared_lock<std::shared_mutex> lock(_mutex);
        return _size;
    }
protected:
    unsigned long _size;
    unsigned long _capacity;
    ZSDataNode* _start;
    ZSDataNode* _end;
    mutable std::shared_mutex _mutex;
};

class ZSSemaData:public ZSData_base,public Semaphore_base{
public:
    ZSSemaData(bool singleData = true):_data(nullptr),_semaphore(nullptr){
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << " ZSSemaData constructor" << std::endl;
        #endif
        if(singleData){
            _data = new ZSData();
            _semaphore = new SemaphoreO();
        }else{
            _data = new ZSDataQueue();
            _semaphore = new SemaphoreM();
        }
    }
    ~ZSSemaData(){
        delete _data;
        delete _semaphore;
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << " ZSSemaData destructor" << std::endl;
        #endif
    }
    virtual void popTo(ZSData& p){
        _data->popTo(p);
    }
    virtual void store(const ZSData& data){
        store(data.data(),data._size);
    }
    virtual void store(const ZSData* data){
        store(data->data(),data->_size);
    }
    virtual void store(const void* const data,unsigned long size){
        _data->store(data,size);
    }
    virtual void signal(unsigned int c = 1){
        _semaphore->signal(c);
    }
    virtual void wait(){
        _semaphore->wait();
    }
    virtual bool try_wait(){
        return _semaphore->try_wait();
    }
private:
    ZSData_base* _data;
    Semaphore_base* _semaphore;
};

class ZSPlugin{
public:
    ZSPlugin(const std::string& name):_name(name){
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << ' ' << this->_name << " ZSPlugin constructor" << std::endl;
        #endif
    }
    ZSPlugin(const ZSPlugin&):_name("_copyed"){
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << ' ' << this->_name << " ZSPlugin copy constructor" << std::endl;
        #endif
    }
    virtual ~ZSPlugin(){
        for(auto it:_databox){
            delete (it.second);
        }

        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << ' ' << this->_name << " ZSPlugin destructor" << std::endl;
        #endif
    }
    virtual void run() = 0;
    virtual void start() final{
        _t = std::thread([=]{run();});
    }
    virtual void detech() final{
        _t.detach();
    }
    virtual void join() final{
        _t.join();
    }
    virtual void publish(const std::string& msg,const void* data = nullptr,const unsigned long size = 0) final{
        auto it = _subscribers.find(msg);
        if (it != _subscribers.end()){
            if (data != nullptr){
                for(auto p:_subscribers[msg]){
                    p->store(data,size);
                    p->signal();
                }
            }else{
                for(auto p:_subscribers[msg]){
                    p->signal();
                }
            }
        }
        if (it == _subscribers.end()){
            std::cerr << "ERROR : didn't DECLARE to PUBLISH this kind of message, check your message type : " << msg << std::endl;
            return;
        }
    }
    virtual void publish(const std::string& msg,const ZSData& data) final{
        auto it = _subscribers.find(msg);
        if (it != _subscribers.end()){
            for(auto p:_subscribers[msg]){
                p->store(data);
                p->signal();
            }
        }
        if (it == _subscribers.end()){
            std::cerr << "ERROR : didn't DECLARE to PUBLISH this kind of message, check your message type : " << msg << std::endl;
            return;
        }
    }
    virtual void receive(const std::string& msg){
        auto it = _databox.find(msg);
        if (it == _databox.end()){
            std::cerr << "ERROR : didn't DECLARE to RECEIVE this kind of message, check your message type : " << msg << std::endl;
            return;
        }
        it->second->wait();
    }
    virtual void receive(const std::string& msg,ZSData& data) final{
        auto it = _databox.find(msg);
        if (it == _databox.end()){
            std::cerr << "ERROR : didn't DECLARE to RECEIVE this kind of message, check your message type : " << msg << std::endl;
            return;
        }
        it->second->wait();
        it->second->popTo(data);
    }
    virtual bool try_receive(const std::string& msg,ZSData& data) final{
        auto it = _databox.find(msg);
        if (it == _databox.end()){
            std::cerr << "ERROR : didn't DECLARE to RECEIVE this kind of message, check your message type : " << msg << std::endl;
            return false;
        }
        bool res = it->second->try_wait();
        if(res){
            it->second->popTo(data);
        }
        return res;
    }
    virtual bool try_receive(const std::string& msg) final{
        auto it = _databox.find(msg);
        if (it == _databox.end()){
            std::cerr << "ERROR : didn't DECLARE to RECEIVE this kind of message, check your message type : " << msg << std::endl;
            return false;
        }
        bool res = it->second->try_wait();
        return res;
    }
    virtual void link(ZSPlugin* p,const std::string& msg) final{
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << ' ' << this->_name << " link : " << msg << " with " << p << ' ' << p->_name << std::endl;
        #endif
        auto it = _subscribers.find(msg);
        if (it == _subscribers.end()){
            std::cerr << "ERROR : didn't DECLARE to PUBLISH message : " << msg << std::endl;
            return;
        }
        auto iit = p->_databox.find(msg);
        if(iit == p->_databox.end()){
            std::cerr << "ERROR : didn't DECLARE to STORE this kind of message, check your message type : " << msg << std::endl;
            return;
        }
        std::cout << "link function called : " << msg << std::endl;
        it->second.push_back(iit->second);
    }
    virtual void declare_receive(const std::string& msg,bool frameSkipMode = true) final{
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << ' ' << this->_name << " declare_receive " << msg << std::endl;
        #endif
        auto it = _databox.find(msg);
        if (it != _databox.end()){
            std::cerr << "ERROR : REDECLARE, check your message type : " << msg << std::endl;
            return;
        }
        _databox.insert(std::pair<std::string,ZSSemaData*>(msg,new ZSSemaData(frameSkipMode)));
    }
    virtual void declare_publish(const std::string& msg) final{
        #ifdef ZSPLUGIN_DEBUG
        std::cout << this << ' ' << this->_name << " declare_publish " << msg << std::endl;
        #endif
        auto it = _subscribers.find(msg);
        if (it != _subscribers.end()){
            std::cerr << "ERROR : REDECLARE_PUBLISH, check your message type : " << msg << std::endl;
            return;
        }
        _subscribers[msg] = {};
    }
private:
    std::map<std::string,std::list<ZSSemaData*>> _subscribers = {};
    std::map<std::string,ZSSemaData*> _databox = {};
    std::thread _t;
    std::string _name;
};

#endif // __ZSS_PLUGIN_H__
