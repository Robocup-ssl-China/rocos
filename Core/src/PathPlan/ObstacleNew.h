#ifndef __OBSTACLENEW_H__
#define __OBSTACLENEW_H__

#include "utils.h"
#include <iostream>
#include <vector>
#include <VisionModule.h>
#define EPSILON (1.0E-10)
using namespace std;

enum ObstacleNewType {
    OBS_CIRCLE_NEW,
    OBS_LONG_CIRCLE_NEW,
    OBS_RECTANGLE_NEW
};

class stateNew {
  public:
    stateNew() {}
    stateNew(CGeoPoint p, double d) : pos(p), orient(d) {}
    stateNew(CGeoPoint p, double d, CVector v, double r) : pos(p), orient(d), vel(v), rotVel(r) {}
    stateNew(const stateNew& s) : pos(s.pos), vel(s.vel), orient(s.orient), rotVel(s.rotVel) {}
    CGeoPoint pos;
    CVector vel;
    double orient;
    double rotVel;
};

class ObstacleNew {
  public:
    ObstacleNew() {}
    float closestDist(CGeoPoint p) const;
    float closestDist(CGeoSegment s) const;

    bool check(const CGeoPoint& p, float minDist = 0) const;
    bool check(const CGeoSegment& s, float minDist = 0) const;
    bool check(const float segP1x, const float segP1y, const float segP2x, const float segP2y, const float minDist = 0) const;
    bool check(const float px, const float py, const float minDist = 0) const;

    int getType() {
        return _type;
    }
    CGeoPoint getStart() {
        return _segStart;
    }
    CGeoPoint getEnd() {
        return _segEnd;
    }
    CGeoPoint getCentroid() const {
        return CGeoPoint((_segStart.x() + _segEnd.x()) / 2, (_segStart.y() + _segEnd.y()) / 2);
    }
    double getRobotRadius() {
        return _robotRadius;
    }
    double getRadius() {
        return _obsRadius;
    }
    CVector getVel() {
        return _vel;
    }

    bool isDynamic() const {
        return _dynamic;
    }
    bool isValid() {
        return _valid;
    }

    void setType(int type) {
        _type = type;
    }
    void setRobotRadius(double r) {
        _robotRadius = r;
    }
    void setObsRadius(double r) {
        _obsRadius = r;
    }
    void setVel(CVector v) {
        _vel = v;
    }
    void setPos(CGeoPoint pos) {
        _segStart = pos;
        _segEnd = pos;
    }
    void setPos(CGeoPoint start, CGeoPoint end) {
        _segStart = start;
        _segEnd = end;
    }
    void setDynamic(bool d) {
        _dynamic = d;
    }
    void setValid(bool v) {
        _valid = v;
    }
    void setParameters(ObstacleNew obst);

  private:
    int _type;
    bool _dynamic;
    bool _valid;
    double _robotRadius; // robot radius
    double _obsRadius;  // obstacle radius
    CVector _vel;  // object velocity
    CGeoPoint _segStart;
    CGeoPoint _segEnd;
};


const int MAX_OBSTACLESNEW = 50;
class ObstaclesNew {
    double robotRadius;
    int num;
  public:
    vector<ObstacleNew> obs;
  public:
    ObstaclesNew() {}
    ObstaclesNew(double robotRadius) : robotRadius(robotRadius), num(0) {
        obs.reserve(MAX_OBSTACLESNEW);
    }
    void circleRect(CGeoPoint p1, CGeoPoint p2, CVector normv, double radius, vector < CGeoPoint >& c);
    void drawLongCircle(CGeoPoint p1, CGeoPoint p2, double radius);
    void drawCircle(CGeoPoint p, double radius);
    void drawRect(CGeoPoint leftUp, CGeoPoint rightDown);

    void addObs(const CVisionModule* pVision, const TaskT& task, bool drawObs, double oppAvoidDist, double teammateAvoidDist, double ballAvoidDist, bool shrinkTheirPenalty);
    void clear() {
        vector<ObstacleNew>().swap(obs);
        num = 0;
    }
    int getNum() const {
        return num;
    }
    double getRobotRadius() const {
        return robotRadius;
    }
    void addLongCircle(const CGeoPoint& x0, const CGeoPoint& x1, const CVector& v, double r, int type = OBS_LONG_CIRCLE_NEW, bool isDynamic = false, bool drawObst = false);
    void addCircle(const CGeoPoint& x0, const CVector& v, float r, int type = OBS_CIRCLE_NEW, bool isDynamic = false, bool drawObst = false);
    void addRectangle(const CGeoPoint& x0, const CGeoPoint& x1, double r, int type = OBS_RECTANGLE_NEW, bool drawObst = false);
    bool check(const CGeoPoint& p);
    bool check(const CGeoPoint& p, int &id);
    bool check(const CGeoPoint& p1, const CGeoPoint& p2);
    bool check(const CGeoPoint& p1, const CGeoPoint& p2, int &id);
    void operator= (const ObstaclesNew& obstacles) {
        obs = obstacles.obs;
        num = obstacles.getNum();
        robotRadius = obstacles.robotRadius;
    }
    void changeWorld(const stateNew& curState, double dMax);
    void drawObstacles();
};

class RobotFootprintModel {
  public:
    RobotFootprintModel() {}
    RobotFootprintModel(double radius) : _radius(radius) {}
    double calculateDistance(const CGeoPoint& currentPose, const ObstacleNew* obstacle) const {
        return obstacle->closestDist(currentPose);
    }
    void setRadius(double radius) {
        _radius = radius;
    }
  private:
    double _radius;
};


#endif /*__OBSTACLENEW_H__*/
