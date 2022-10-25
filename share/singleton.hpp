#ifndef __SINGLETON_H__
#define __SINGLETON_H__
template <class SingletonClass>
class Singleton{
public:
    static SingletonClass * instance(){
        static SingletonClass instance;
        return &instance;
    }
    SingletonClass* operator ->() { return instance(); }
    const SingletonClass* operator ->() const { return instance(); }
private:
    Singleton(){}
    ~Singleton(){}
};
#endif // __SINGLETON_H__
