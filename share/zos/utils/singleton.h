#ifndef __SINGLETON_H__
#define __SINGLETON_H__
#include <type_traits>
#include <iostream>
template <typename T, typename D = T>
class Singleton{
    friend D;
    static_assert(std::is_base_of_v<T, D>, "T should be a base type for D");
public:
    template<typename... Ts>
    static T* instance(Ts... args);
    template<typename... Ts>
    static T* GetInstance(Ts... args){ return instance(args...); }
    template<typename... Ts>
    static T* _(Ts... args){ return instance(args...); }
private:
    Singleton() = default;
    ~Singleton(){
        delete inst;
    };
    Singleton( const Singleton& ) = delete;
    Singleton& operator=( const Singleton& ) = delete;
    static inline T* inst = nullptr;
};

template <typename T, typename D>
template <typename... Ts>
T* Singleton<T, D>::instance(Ts... args){
    if (inst==nullptr)
        inst = new T(args...);
    return inst;
}
#endif // __SINGLETON_H__
