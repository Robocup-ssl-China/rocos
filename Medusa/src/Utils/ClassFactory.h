#ifndef __CLASS_FACTORY_H__
#define __CLASS_FACTORY_H__
#include   <string>
#include   <map>
#include   <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
using namespace std;
#define	CLASS_CAPACITY	100
#define	STRING_LENGTH	50
typedef void* (*CreateFuntion)(void);

struct RegisterPair{
	char s[STRING_LENGTH];
	CreateFuntion f;
};

class ClassFactory
{
public:
	static void* GetClassByName(std::string name)
	{
		for(int i = 0; i < CLASS_CAPACITY; i++){
			if(strcmp(m_vec[i].s, name.c_str()) == 0){
				return m_vec[i].f();
			}
		}
		return NULL;
	}
	static void RegistClass(std::string name,CreateFuntion method)
	{
	//	cout<<"RegisterPair "<<i++<<endl;
		static int i = 0;
		memset(m_vec[i].s, 0, STRING_LENGTH);
		memcpy(m_vec[i].s, name.c_str(), strlen(name.c_str()));
		m_vec[i].f = method;
		i++;
		if (i >= CLASS_CAPACITY)
		{
			cout << "ERROR: ClassFactory is full, please add the macro 'CLASS_CAPACITY' in file : ClassFactory.h !!"<<endl;
		}
	}
private:
	static RegisterPair m_vec[CLASS_CAPACITY];
};

class RegistyClass
{
public:
	RegistyClass(std::string name,CreateFuntion method)
	{
		ClassFactory::RegistClass(name,method);
	}
};

template<class T,const char name[]>
class Register
{
public:
	Register()
	{
		//这个一定要加，因为编译器不保证程序开始时就初始化变量rc
		const RegistyClass tmp = rc;
	}
	static void* CreateInstance()
	{
		return new T;
	}
public:
	static const RegistyClass rc;
};
template<class T,const char name[]>
const RegistyClass Register<T,name>::rc(name,Register<T,name>::CreateInstance);

#define DEFINE_CLASS(class_name)						\
char NameArray##class_name[]=#class_name;		\
class class_name:public Register<class_name,NameArray##class_name>

#define DEFINE_CLASS_EX(class_name,father_class)		\
char NameArray##class_name[]=#class_name;				\
class class_name:public Register<class_name,NameArray##class_name>,public father_class

#define DEFINE_CLASS_EX_H(class_name,father_class)		\
extern char NameArray##class_name[];				\
class class_name:public Register<class_name,NameArray##class_name>,public father_class

#define DEFINE_CLASS_EX_CPP(class_name)		\
char NameArray##class_name[]=#class_name;

#endif
