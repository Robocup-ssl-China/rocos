#ifndef CHIPSOLVER_H
#define CHIPSOLVER_H
#include "globaldata.h"
#include "ballrecords.h"
class Chipsolver {
  public:
    Chipsolver();
    ~Chipsolver();
    double chipsolveOffset(double timeOff);
    void getbestresult();
    CGeoPoint dealresult(CVector3 & realpos);
    void reset();
    void setrecord();
    void setrecord_test();

  private:
    CVector3 kickVel;
    CVector3 kickPos;
    CVector3 posNow;
    CVector3 posEnd;
    double l1Error;
    double timeOffset;
    double t;
    double tFly;
    BallRecord ballrecord;

};
#endif // CHIPSOLVER_H
