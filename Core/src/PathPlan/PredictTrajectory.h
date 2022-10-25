#ifndef PREDICTPredictTrajectory_H
#define PREDICTPredictTrajectory_H

#include "cmu/vector.h"
#include "QuadraticEquation.h"
#include "CubicEquation.h"
#include "QuarticEquation.h"

/************************************************************************/
/* 轨迹类：储存小车的运行轨迹，并且判断与其它轨迹之间是否碰撞
/************************************************************************/
class PredictTrajectory {
public:
    PredictTrajectory(){_qlength = 0; _limitTime = EPSILON;}
    ~PredictTrajectory(){}
    void MakeTrajectory(const vector2f pos,const vector2f vel, const vector2f acc,
                        const float t0,const float tc,
                        const float Radius,const float DMAX, const int type);
    bool CheckTrajectory(const PredictTrajectory& T1);
    CQuadraticEquation q[3]; // 二次方程式的实例
    int _qlength;
    int _type; //小车的类型
    int _carnum;
    float Radius;
    float _limitTime;
    float _tend;
};


#endif // PREDICTPredictTrajectory_H
