#ifndef _SINGLETON_H_
#define _SINGLETON_H_
/************************************************************************/
/*            Meyers Singleton, 退出时释放资源                          */
/************************************************************************/
template <class SingletonClass >
class MeyersSingleton{
public:
	static SingletonClass * Instance(){
		static SingletonClass instance;
		return &instance;
	}
	SingletonClass* operator ->() { return Instance(); }
	const SingletonClass* operator ->() const { return Instance(); }
private:
	MeyersSingleton(){ }
	~MeyersSingleton(){ }
};

/************************************************************************/
/*              普通的Singleton, 退出时不释放资源                         */
/************************************************************************/
// 利用构造函数私有化，使本类只能有一个对象实体 --解读注释
template <class SingletonClass >
class NormalSingleton{
public:
	static SingletonClass * Instance(){
		static SingletonClass* instance = 0;
		if( !instance ){
			instance = new SingletonClass;
		}
		return instance;
	}
	SingletonClass* operator ->() { return Instance(); }
	const SingletonClass* operator ->() const { return Instance(); }
private:
	NormalSingleton(){ }
	~NormalSingleton(){ }
};
#endif
