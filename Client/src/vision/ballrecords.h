#ifndef BALLRECORDS_H
#define BALLRECORDS_H
#include "messageformat.h"
#include "geometry.h"
#include "staticparams.h"
#include "parammanager.h"
#include <iostream>
#include <chrono>
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
class CVector3 {
  public:
    CVector3(): _x(0), _y(0), _z(0) {}
    CVector3(double x, double y, double z): _x(x), _y(y), _z(z) {}
    double x() const {
        return _x;
    }
    double y() const {
        return _y;
    }
    double z() const {
        return _z;
    }
    double mod() const {
        return sqrt(_x * _x + _y * _y + _z * _z);
    }
    void multiply(double a) {
        _x *= a;
        _y *= a;
        _z *= a;
    }
    void add(CVector3 a) {
        _x += a.x();
        _y += a.y();
        _z += a.z();
    }
    CVector3 addz(double a) {
        return CVector3(_x, _y, _z + a);
    }
    CVector3 multiplyNew(double a) {
        return CVector3(_x * a, _y * a, _z * a);
    }
    CVector3 addNew(CVector3 a) {
        return CVector3(_x + a.x(), _y + a.y(), _z + a.z());
    }
    void formXYZ(double x = 0, double y = 0, double z = 0) {
        _x = x;
        _y = y;
        _z = z;
    }
    void formXYZ(CGeoPoint pos2, double z = 0) {
        _x = pos2.x();
        _y = pos2.y();
        _z = z;
        //std::cout << "get fromXYZ" << _x << _y << _z << std::endl;
    }

  private:
    double _x, _y, _z;
};

class  BallRecord {
  public:
    BallRecord() {}
    high_resolution_clock::time_point getcapture() const {
        return capturetime;
    }
    CVector3 getpos()const {
        return pos;
    }
    CVector3 getcampos()const {
        return campos;
    }
    int getcamID()const {
        return camID;
    }
    double getidealTime() const{
        return idealTime;
    }
    long long timeinterval(high_resolution_clock::time_point capturetime0)const {
        milliseconds timeInterval = std::chrono::duration_cast<milliseconds>(capturetime - capturetime0);
        return timeInterval.count();
    }
    void setpos(double x, double y, double z = 0) {
        pos.formXYZ(x, y, z);
    }
    void setpos(CGeoPoint pos2, double z = 0) {
        //std::cout << "get setpos" << pos2 << z << std::endl;
        pos.formXYZ(pos2, z);
    }
    void setcampos(double x, double y, double z = 300) {
        campos.formXYZ(x, y, z);
    }
    void setcampos(CGeoPoint pos2, double z = 300) {
        //std::cout << "get setcampos" << pos2 << z << std::endl;
        campos.formXYZ(pos2, z);
    }
    void settime() {
        capturetime = high_resolution_clock::now();
    }
    void setcamID(int ID) {
        camID = ID;
    }
    void setidealTime(double t){
        idealTime=t;
    }
  private:
    CVector3 pos;
    CVector3 campos;
    high_resolution_clock::time_point capturetime;
    double idealTime;
    int camID;
    //for test


};
#endif // BALLRECORDS_H
