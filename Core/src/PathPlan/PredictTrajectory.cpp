#include "PredictTrajectory.h"
#include <GDebugEngine.h>

#define ourPlayer 0
#define theirPlayer 1
#define THISPLAYER 2

#define VELMIN 1
#define ACCMIN 1
#define SAFE 1
#define UNSAFE 0

void PredictTrajectory::MakeTrajectory(const vector2f pos, const vector2f vel, const vector2f acc,
                                       const float t0, const float tc,
                                       const float Radius, const float DMAX, const int type) {
    // 储存轨迹
    _qlength = 0;
    // 如果当前是我方的车
    if (type == THISPLAYER || type == ourPlayer) {
        _type = type;
        double x0 = pos.x;
        double y0 = pos.y;
        double vx0 = vel.x;
        double vy0 = vel.y;
        double ax0 = acc.x;
        double ay0 = acc.y;
        double t1 = t0 + tc;
        // 如果小车一开始就是静止的,第一段轨迹就是一个点
        if (abs(vx0) <= VELMIN && abs(vy0) <= VELMIN && abs(ax0) <= ACCMIN && abs(ay0) <= ACCMIN) {
            q[_qlength++].init(CCar(CPoint(x0, y0), 0.0f, 0.0f, 0.0f, 0.0f, Radius), t0, t1);
            _tend = t0;
            return;
        }
        else {
            // 否则第一段轨迹是一段抛物线
            q[_qlength++].init(CCar(CPoint(x0, y0), vx0, vy0, ax0, ay0, Radius), t0, t1);
            double vx1 = vx0 + ax0 * tc;
            double vy1 = vy0 + ay0 * tc;
            double v1 = sqrt(vx1 * vx1 + vy1 * vy1);
            // 如果一帧之后小车静止
            if (abs(vx1) <= VELMIN && abs(vy1) <= VELMIN) {
                _tend = t1;
                return;
            }
            // 如果一帧之后小车不静止，再接一段减速路径
            else {
                double x1 = x0 + vx0 * tc + 0.5f * ax0 * tc * tc;
                double y1 = y0 + vy0 * tc + 0.5f * ay0 * tc * tc;
                double ax1 =  - vx1/v1 * DMAX;
                double ay1 =  - vy1/v1 * DMAX;
                double t2 = t1 + v1/DMAX;
                double x2 = x1 - vx1 * vx1 / (2 * ax1);
                double y2 = y1 - vy1 * vy1 / (2 * ay1);
                q[_qlength++].init(CCar(CPoint(x1, y1), vx1, vy1, ax1, ay1, Radius), t1, t2);
                _tend = t2;
//                GDebugEngine::Instance()->gui_debug_line(CGeoPoint(x0, y0), CGeoPoint(x1, y1), COLOR_GREEN);
//                GDebugEngine::Instance()->gui_debug_line(CGeoPoint(x1, y1), CGeoPoint(x2, y2), COLOR_GREEN);
                return;
            }
        }
    }
    // 如果是对方车,只考虑一段抛物线
    else if (type == theirPlayer) {
        _type = type;
        double t1 = tc;
        double x0 = pos.x;
        double y0 = pos.y;
        double vx0 = vel.x;
        double vy0 = vel.y;
        double ax0 = acc.x;
        double ay0 = acc.y;
        double x1 = x0 + vx0 * t1 + 0.5 * ax0 * t1 * t1;
        double y1 = y0 + vy0 * t1 + 0.5 * ay0 * t1 * t1;

        q[_qlength++].init(CCar(CPoint(x0, y0), vx0, vy0, ax0, ay0, Radius), t0, t1);
//        GDebugEngine::Instance()->gui_debug_line(CGeoPoint(x0, y0), CGeoPoint(x1, y1), COLOR_GREEN);
        return;
    }
    //  如果是球，直接考虑匀速运动
    else {
        _type = type;
        double t1 = tc;
        double x0 = pos.x;
        double y0 = pos.y;
        double vx0 = vel.x;
        double vy0 = vel.y;
        double x1 = x0 + vx0 * t1;
        double y1 = y0 + vy0 * t1;
        q[_qlength++].init(CCar(CPoint(x0, y0), vx0, vy0, 0.0f, 0.0f, Radius), t0, t1);
//        GDebugEngine::Instance()->gui_debug_line(CGeoPoint(x0, y0), CGeoPoint(x1, y1));
        return;
    }

}
/************************************************************************/
/* 判断两个轨迹是否相撞
/************************************************************************/
bool PredictTrajectory::CheckTrajectory(const PredictTrajectory& T1) { // 检测两个轨迹之间是否相互碰撞
    if (T1._type == ourPlayer) {
        if (((T1.q[0].pX - q[0].pX) * q[0].pVelX + (T1.q[0].pY - q[0].pY) * q[0].pVelY) <= 0 ) { // 表示己方小车在速度方向的后面，则表示安全
            return SAFE;
        }
        else { // 否则就安正常的判断流程判断
            for (int i = 0; i < _qlength; i++) {
                for(int j = 0; j < T1._qlength; j++) {
                    CQuadraticEquation q1 = q[i];
                    CQuadraticEquation q2 = T1.q[j];
                    CQuarticEquation q3 = q1 - q2;
                    if (q3.getIsCrash()) {
                        vector2f tdir = vector2f((q2.pX - q1.pX), (q2.pY - q1.pY));
                        vector2f tdirgo = vector2f(q1.ax, q1.ay);
                        float dir_angle = 1.0f;
                        if (tdir.length() < EPSILON || tdirgo.length() < EPSILON) {
                            dir_angle = 0.0f;
                        }
                        else {
                            dir_angle = cosine(tdir, tdirgo);
                        }
                        /*cout<<"time:"<<q3.getRoot()<<endl;*/
                        //判断小车是否相撞，需要考虑碰撞的时间，以及碰撞小车的速度情况
                        if ((q3.getRoot() >= _limitTime) || (q3.getRoot() <= _limitTime && i == 0 && dir_angle > 0.3f)) {
                            return UNSAFE;
                        }
                    }
                }
            }
        }
    }
    else {
        for(int i = 0; i < _qlength; i++) {
            for( int j = 0; j < T1._qlength; j++) {
                CQuadraticEquation q1 = q[i];
                CQuadraticEquation q2 = T1.q[j];
                CQuarticEquation q3 = q1 - q2;
                if (q3.getIsCrash()) {
                    vector2f tdir = vector2f((q2.pX - q1.pX), (q2.pY - q1.pY));
                    vector2f tdirgo = vector2f(q1.ax, q1.ay);
                    float dir_angle = 1.0f;
                    if (tdir.length() < EPSILON || tdirgo.length() < EPSILON) {
                        dir_angle = 0.0f;
                    }
                    else {
                        dir_angle = cosine(tdir, tdirgo);
                    }
                    //判断小车是否相撞，需要考虑碰撞的时间，以及碰撞小车的速度情况
                    if ((q3.getRoot() >= _limitTime) || (q3.getRoot() <= _limitTime && i == 0 && dir_angle > 0.3f)) {
                        return UNSAFE;
                    }
                }
            }
        }
    }
    return SAFE;
}
