#ifndef __ZOS_INTERFACE_H__
#define __ZOS_INTERFACE_H__
namespace zos{
class Data;
// data interface
class IData{
public:
    IData() = default;
    virtual ~IData() = default;
//    IData(const IData&) = delete;
    virtual void pop(Data&) = 0;
    virtual void pop(Data*) = 0;
    virtual void store(const void* const data,unsigned long size) = 0;
};
class ISema{
public:
    ISema() = default;
    virtual ~ISema() = default;
//    ISema(const ISema&) = delete;
    virtual void release() = 0;
    virtual void acquire() = 0;
    virtual bool try_acquire_for(unsigned int) = 0;
    virtual bool try_acquire() = 0;
};
class ISemaData:public ISema,public IData{
public:
    ISemaData() = default;
    virtual ~ISemaData() = default;
    virtual bool try_pop(Data&) = 0;
    virtual bool try_pop_for(Data&,unsigned int) = 0; 
//    ISemaData(const ISemaData&) = delete;
};
}// namespace zos;
#endif // __ZOS_INTERFACE_H__
