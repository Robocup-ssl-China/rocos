#ifndef __MESSAGEFORMAT_H__
#define __MESSAGEFORMAT_H__
#include "staticparams.h"
#include <iostream>
#include "geometry.h"
#include "ballstate.h"
/************************************************************************/
/*                 Basic Vision Classes                                 */
/************************************************************************/
namespace {
/*
    struct Pos2d{
        double x;
        double y;
        Pos2d():x(-32767),y(-32767){}
        Pos2d(double _x,double _y){x=_x;y=_y;}
        bool fill(double x,double y){
            this->x=x;
            this->y=y;
            return true;
        }
        double dist(Pos2d p){
            return sqrt((x-p.x)*(x-p.x)+(y-p.y)*(y-p.y));
        }
    };
    */

//enum ballState {received, touched, kicked, struggle, chip_pass, flat_pass};

struct Ball {
    CGeoPoint pos;
    CGeoPoint predict_pos;
    Ballstate ball_state_machine;
    double height;
    int cameraID;
    CVector velocity;
    bool valid = false;
    Ball(): pos(), height(0) {}
    bool  fill(double x, double y, double height = 0, int id = -1) {
        this->pos.setX(x);
        this->pos.setY(y);
        this->height = height;
        this->cameraID = id;
        return true;
    }
    bool fill(double x, double y, double height, CVector vel = CVector(0, 0)) {
        this->pos.setX(x);
        this->pos.setY(y);
        this->height = height;
        this->velocity = vel;
        return true;
    }
    bool fill(const Ball& ball) {
        return fill(ball.pos.x(), ball.pos.y(), ball.height, ball.velocity);
    }
    bool fillPredictPos(CGeoPoint prePos) {
        this->predict_pos = prePos;
        return true;
    }
};
struct Robot {
    unsigned short id;
    CGeoPoint pos;
    CGeoPoint predict_pos;
    double angle;
    double raw_angle;
    CVector velocity;
    CVector accelerate;
    CVector raw_vel;
    double rotateVel;
    double rawRotateVel;
    bool valid = false;
    Robot(): id(-1), accelerate(0, 0) {}
    Robot(double _x, double _y, double _angle, double _id = -1) {
        pos.setX(_x);
        pos.setY(_y);
        angle = _angle;
        raw_angle = _angle;
        id = _id;
    }
    bool fill(unsigned short id, double _x, double _y, double angle, CVector vel = CVector(0, 0)) {
        this->id = id;
        pos.setX(_x);
        pos.setY(_y);
        this->angle = angle;
        this->raw_angle = angle;
        this->velocity = vel;
        return true;
    }
    bool fill(const Robot& robot) {
        return fill(robot.id, robot.pos.x(), robot.pos.y(), robot.angle, robot.velocity);
    }
    bool setPos(CGeoPoint pos) {
        this->pos = pos;
        return true;
    }
    bool setPredictPos(CGeoPoint prePos) {
        this->predict_pos = prePos;
        return true;
    }
};
struct CameraEdge {
    double min, max;
};
struct SingleCamera {
    unsigned short id;
    CGeoPoint campos;
    double height;
    CameraEdge leftedge, rightedge, upedge, downedge;
    SingleCamera(): height(3500) {} //set 3500mm as an example
    void fillCenter(double x, double y) {
        campos.fill(x, y);
        leftedge.min = rightedge.min = x;
        upedge.min = downedge.min = y;
    }
    void fillCenter(CGeoPoint p) {
        campos = p;
        leftedge.min = rightedge.min = p.x();
        upedge.min = downedge.min = p.y();
    }
};
struct CameraFix {
    double theta, t1, t2;
    CameraFix(): theta(0), t1(0), t2(0) {}
};
}


//* Robot store the data, Index and Size exist for the index search for robots;
//* Index : -1 means not exist, or it means the index in Robot Array;
//* VisionMessage use for the final processed vision data.
class OriginMessage {
  public:
    unsigned short robotSize[2];
    unsigned short ballSize;
    Robot robot[2][PARAM::ROBOTNUM];
    Ball ball[PARAM::BALLNUM];
    int robotIndex[2][PARAM::ROBOTMAXID];
    OriginMessage(): ballSize(0) {
        robotSize[PARAM::BLUE] = robotSize[PARAM::YELLOW] = 0;
        std::fill_n(&robotIndex[0][0], 2 * PARAM::ROBOTMAXID, -1);
    }
    void init() {
        robotSize[PARAM::BLUE] = robotSize[PARAM::YELLOW] = ballSize = 0;
        std::fill_n(&robotIndex[0][0], 2 * PARAM::ROBOTMAXID, -1);
        ball[0].pos.fill(-32767, -32767);
    }
    bool addBall(double x, double y, double height = 0, int id = -1) {
        return ballSize >= PARAM::BALLNUM ? false : ball[ballSize++].fill(x, y, height, id);
    }
    bool addBall(CGeoPoint point, double height = 0, int id = -1) {
        return ballSize >= PARAM::BALLNUM ? false : ball[ballSize++].fill(point.x(), point.y(), height, id);
    }
    bool addRobot(int color, unsigned short id, double x, double y, double angel) {
        if(robotSize[color] >= PARAM::ROBOTNUM)
            return false;
        robotIndex[color][id] = robotSize[color];
        return robot[color][robotSize[color]++].fill(id, x, y, angel);
    }
    bool addRobot(int color, unsigned short id, CGeoPoint point, double angel) {
        if(robotSize[color] >= PARAM::ROBOTNUM)
            return false;
        robotIndex[color][id] = robotSize[color];
        return robot[color][robotSize[color]++].fill(id, point.x(), point.y(), angel);
    }
    bool addRobot(int color, const Robot& r) {
        if(robotSize[color] >= PARAM::ROBOTNUM)
            return false;
        robotIndex[color][r.id] = robotSize[color];
        return robot[color][robotSize[color]++].fill(r);
    }
    bool addBall(const Ball& b) {
        return ballSize >= PARAM::BALLNUM ? false : ball[ballSize++].fill(b);
    }
};

class ReceiveVisionMessage: public OriginMessage {
  public:
    unsigned short camID;
    unsigned long long frame;
    double captureTime; // ![1] TODO -> gjy

    ReceiveVisionMessage(): camID(-1), frame(-1), captureTime(-1) {}
    void clear() {
        captureTime = frame = camID = -1;
        ballSize = robotSize[PARAM::BLUE] = robotSize[PARAM::YELLOW] = 0;
    }
};

#endif // __MESSAGEFORMAT_H__
