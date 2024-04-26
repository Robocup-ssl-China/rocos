#include "ObstacleNew.h"
#include "utils.h"
#include "GDebugEngine.h"
#include <iostream>
#include "PlayInterface.h"
#include "TaskMediator.h"
#include "WorldModel.h"

namespace {
const double MAX_PROB_VEL = 500*10;
const double MAX_DEC = 650*10;
const double ZERO_VEL = 20*10;
const double ROBOT_RADIUS = PARAM::Vehicle::V2::PLAYER_SIZE / 2;
const double FRAME_PERIOD = 1.0 / PARAM::Vision::FRAME_RATE;
const double MIN_AVOID_DIST = PARAM::Vehicle::V2::PLAYER_SIZE + 2.0*10;
const double LOWER_BOUND_AVOID_SPEED = 50*10;
const double UPPER_BOUND_AVOID_SPEED = 250*10;

double stopBallAvoidDist = 50*10;

inline float minObs(float a, float b) {
    if(a < b) return a;
    return b;
}

inline float maxObs(float a, float b) {
    if(a < b) return b;
    return a;
}

inline void getABC(const float x1, const float y1, const float x2, const float y2, float& a, float& b, float& c) {
    if(y1 == y2) {
        a = 0;
        b = 1;
        c = -1.0 * y1;
    } else {
        a = 1;
        b = -1.0 * (x1 - x2) / (y1 - y2);
        c = (x1 * y2 - y1 * x2) / (y1 - y2);
    }
}

inline void LineLineInter(const float a1, const float b1, const float c1, const float line2x1, const float line2y1, const float line2x2, const float line2y2, float& px, float& py, bool& intersectant) {
    float a2, b2, c2, d;
    getABC(line2x1, line2y1, line2x2, line2y2, a2, b2, c2);

    d = a1 * b2 - b1 * a2;
    if(abs(d) < 0.0001){
        intersectant = false;
    } else{
        px = (b1 * c2 - c1 * b2) / d;
        py = (c1 * a2 - a1 * c2) / d;
        intersectant = true;
    }
}

inline bool pointOnLineOnSeg(const float segFirstX, const float segFirstY, const float segSecondX, const float segSecondY, const float pointX, const float pointY) {
    bool pointOnSegment;
    if(fabs(segFirstX - segSecondX) > fabs(segFirstY - segSecondY))
        pointOnSegment = (pointX > minObs(segFirstX, segSecondX)) && (pointX < maxObs(segFirstX, segSecondX));
    else
        pointOnSegment = (pointY > minObs(segFirstY, segSecondY)) && (pointY < maxObs(segFirstY, segSecondY));
    return pointOnSegment;
}

inline bool SegSegInter(const float line1x1, const float line1y1, const float line1x2, const float line1y2, const float line2x1, const float line2y1, const float line2x2, const float line2y2) {
    bool intersectant;
    float a1, b1, c1, px, py;
    getABC(line1x1, line1y1, line1x2, line1y2, a1, b1, c1);
    LineLineInter(a1, b1, c1, line2x1, line2y1, line2x2, line2y2, px, py, intersectant);
    return intersectant && (pointOnLineOnSeg(line1x1, line1y1, line1x2, line1y2, px, py) && pointOnLineOnSeg(line2x1, line2y1, line2x2, line2y2, px, py));
}

inline float distSegToPoint(const float segFirstX, const float segFirstY, const float segSecondX, const float segSecondY, const float pointX, const float pointY) {

    float projectionX = 0.0, projectionY = 0.0;
    if(segFirstX == segSecondX) {
        projectionX = segFirstX;
        projectionY = pointY;
    } else {
        float k = (segSecondY - segFirstY) / (segSecondX - segFirstX);
        projectionX = (k * k * segFirstX + k * (pointY - segFirstY) + pointX) / (k * k + 1);
        projectionY = k * (projectionX - segFirstX) + segFirstY;
    }
    bool pointOnSegment = pointOnLineOnSeg(segFirstX, segFirstY, segSecondX, segSecondY, projectionX, projectionY);
    if(pointOnSegment) {
        return sqrt(pow(projectionX - pointX, 2) + pow(projectionY - pointY, 2));
    }
    else {
        float dist2P1 = sqrt(pow(pointX - segFirstX, 2) + pow(pointY - segFirstY, 2));
        float dist2P2 = sqrt(pow(pointX - segSecondX, 2) + pow(pointY - segSecondY, 2));
        return minObs(dist2P1, dist2P2);
    }
}

inline float distSegToSeg(float seg1x1, float seg1y1, float seg1x2, float seg1y2, float seg2x1, float seg2y1, float seg2x2, float seg2y2) {
    if(SegSegInter(seg1x1, seg1y1, seg1x2, seg1y2, seg2x1, seg2y1, seg2x2, seg2y2)) return 0;
    else return minObs(distSegToPoint(seg1x1, seg1y1, seg1x2, seg1y2, seg2x1, seg2y1), distSegToPoint(seg1x1, seg1y1, seg1x2, seg1y2, seg2x2, seg2y2));
}

inline float distRectToPoint(const float recLeftX, const float recRightX, const float recUpY, const float recDownY, const float pointX, const float pointY) {

    if(fabs(pointX - recLeftX) + fabs(pointX - recRightX) - fabs(recLeftX - recRightX) < 1.0e-5 &&
            fabs(pointY - recUpY) + fabs(pointY - recDownY) - fabs(recUpY - recDownY) < 1.0e-5) return 0;
    float dist1 = distSegToPoint(recLeftX, recUpY, recLeftX, recDownY, pointX, pointY);
    float dist2 = distSegToPoint(recRightX, recDownY, recRightX, recUpY, pointX, pointY);
    float dist3 = distSegToPoint(recLeftX, recUpY, recRightX, recUpY, pointX, pointY);
    float dist4 = distSegToPoint(recLeftX, recDownY, recRightX, recDownY, pointX, pointY);
    return minObs(dist1, minObs(dist2, minObs(dist3, dist4)));
}

inline void lineRectangleInter(float lineA, float lineB, float lineC,
                               float* x, float* y,
                               bool& intersectant,
                               float* IPX, float* IPY) {
    int num = 0, i = 0;
    float a = lineA, b = lineB, c = lineC;
    float px1, py1, px2, py2, interPointX, interPointY;;
    bool isLineLineInter;
    for(i = 0; i < 4; i++) {
        px1 = x[i];
        py1 = y[i];
        px2 = x[(i+1) % 4];
        py2 = y[(i+1) % 4];
        LineLineInter(a, b, c, px1, py1, px2, py2, interPointX, interPointY, isLineLineInter);
        if(isLineLineInter && pointOnLineOnSeg(px1, py1, px2, py2, interPointX, interPointY)){
            if(num >=2 ){
                printf("Error in CGeoLineRectangleIntersection, Num be: %d \n", num);
                break;
            }
            IPX[num] = interPointX;
            IPY[num] = interPointY;
            num++;
        }
    }
    if(num > 0) {
        intersectant = true;
    } else {
        intersectant = false;
    }
}

}



bool testFunction(void) {
    CGeoPoint p1(150*10, 0), p2(-150*10, 0), p3(20*10, 0), p4(10*10, 0);
    CGeoSegment seg1(p1, p2), seg2(p3, p4);
    float a, b, c;
    getABC(p1.x(), p1.y(), p2.x(), p2.y(), a, b, c);
    CGeoLineLineIntersection lineInter(seg1, seg2);
    bool isInter;
    float px, py;
    LineLineInter(a, b, c, p3.x(), p3.y(), p4.x(), p4.y(), px, py, isInter);
    bool correct = (isInter == lineInter.Intersectant());
    bool pOnSeg1 = pointOnLineOnSeg(seg1.point1().x(), seg1.point1().y(), seg1.point2().x(), seg1.point2().y(), px, py);
    bool pOnSeg2 = pointOnLineOnSeg(seg2.point1().x(), seg2.point1().y(), seg2.point2().x(), seg2.point2().y(), px, py);

    correct = float((seg1.dist2Segment(seg2)) == distSegToSeg(p1.x(), p1.y(), p2.x(), p2.y(), p3.x(), p3.y(), p4.x(), p4.y()));
//    cout << (float((seg1.dist2Segment(seg2)) - distSegToSeg(p1.x(), p1.y(), p2.x(), p2.y(), p3.x(), p3.y(), p4.x(), p4.y())));
//    cout << distSegToSeg(p1.x(), p1.y(), p2.x(), p2.y(), p3.x(), p3.y(), p4.x(), p4.y()) << endl;;
    return correct;
}


//====================================================================//
//    Obstacle class implementation
//====================================================================//

float ObstacleNew::closestDist(CGeoPoint p) const {
    if (_type == OBS_RECTANGLE_NEW) { // defence area
        float left = minObs(_segStart.x(), _segEnd.x());
        float right = maxObs(_segStart.x(), _segEnd.x());
        float up = maxObs(_segStart.y(), _segEnd.y());
        float down = minObs(_segStart.y(), _segEnd.y());
        return distRectToPoint(left, right, up, down, p.x(), p.y());
    } else if ((_segStart - _segEnd).mod() > EPSILON) { // long circle
        return distSegToPoint(_segStart.x(), _segStart.y(), _segEnd.x(), _segEnd.y(), p.x(), p.y());
    } else { // circle
        return (_segStart - p).mod();
    }
}

float ObstacleNew::closestDist(CGeoSegment s) const {
    if (_type == OBS_RECTANGLE_NEW) {
        float left = minObs(_segStart.x(), _segEnd.x());
        float right = maxObs(_segStart.x(), _segEnd.x());
        float up = maxObs(_segStart.y(), _segEnd.y());
        float down = minObs(_segStart.y(), _segEnd.y());
        float dist2First = distRectToPoint(left, right, up, down, s.point1().x(), s.point1().y());
        float dist2Second = distRectToPoint(left, right, up, down, s.point2().x(), s.point2().y());

        if (dist2First == 0 || dist2Second == 0) return 0;
        bool isInter;
        float IPX[2], IPY[2];
        float x[4], y[4];
        x[0] = left - _robotRadius - _obsRadius; y[0] = down - _robotRadius - _obsRadius;
        x[1] = left - _robotRadius - _obsRadius; y[1] = up + _robotRadius + _obsRadius;
        x[2] = right + _robotRadius + _obsRadius; y[2] = up + _robotRadius + _obsRadius;
        x[3] = right + _robotRadius + _obsRadius; y[3] = down - _robotRadius - _obsRadius;
        lineRectangleInter(s.a(), s.b(), s.c(), x, y, isInter, IPX, IPY);
        if (isInter && (pointOnLineOnSeg(s.point1().x(), s.point1().y(), s.point2().x(), s.point2().y(), IPX[0], IPY[0]) || pointOnLineOnSeg(s.point1().x(), s.point1().y(), s.point2().x(), s.point2().y(), IPX[1], IPY[1]))) return 0;
        else return minObs(dist2First, dist2Second);
    }
    else {
        return distSegToSeg(s.point1().x(), s.point1().y(), s.point2().x(), s.point2().y(), _segStart.x(), _segStart.y(), _segEnd.x(), _segEnd.y());
    }

}

// return false if there will be collision
bool ObstacleNew::check(const CGeoPoint& p, float minDist) const {
    return closestDist(p) - _obsRadius - _robotRadius - minDist > 0;
}

bool ObstacleNew::check(const CGeoSegment& s, float minDist) const {
    if(_type == OBS_RECTANGLE_NEW)
        return closestDist(s) > 0;
    else
        return closestDist(s) - _obsRadius - _robotRadius - minDist > 0;
}

bool ObstacleNew::check(const float px, const float py, const float minDist) const {
    float closeDist = 0.0;
    float stX = _segStart.x(), stY = _segStart.y(), seX = _segEnd.x(), seY = _segEnd.y();
    if (_type == OBS_RECTANGLE_NEW) {
        float left , right, up, down;
        if(stX > seX) {
            left = seX;
            right = stX;
        } else {
            left = stX;
            right = seX;
        }
        if(stY > seY) {
            up = stY;
            down = seY;
        } else {
            up = seY;
            down = stY;
        }
        closeDist = distRectToPoint(left, right, up, down, px, py);
    }
    else if (sqrt((stX - seX) * (stX - seX) + (stY - seY) * (stY - seY)) > EPSILON) { // long circle
        closeDist = distSegToPoint(stX, stY, seX, seY, px, py);
    } else { // circle
        closeDist = sqrt((stX - px) * (stX - px) + (stY - py) * (stY - py));
    }
    return closeDist - _obsRadius - _robotRadius - minDist > 0;
}

bool ObstacleNew::check(const float segP1x, const float segP1y, const float segP2x, const float segP2y, const float minDist) const {
    float closeDist = 0.0;
    float a, b, c;
    float stX = _segStart.x(), stY = _segStart.y(), seX = _segEnd.x(), seY = _segEnd.y();
    getABC(segP1x, segP1y, segP2x, segP2y, a, b, c);

    if(_type == OBS_RECTANGLE_NEW) {
        float left, right, up, down, dist2First, dist2Second;
        bool isInter;
        float IPX[2], IPY[2];
        float x[4], y[4];
        if(stX > seX) {
            left = seX;
            right = stX;
        } else {
            left = stX;
            right = seX;
        }
        if(stY > seY) {
            up = stY;
            down = seY;
        } else {
            up = seY;
            down = stY;
        }
        dist2First = distRectToPoint(left, right, up, down, segP1x, segP1y);
        dist2Second = distRectToPoint(left, right, up, down, segP2x, segP2y);
        if (dist2First == 0 || dist2Second == 0)
            closeDist = 0.0;

        x[0] = left - _robotRadius - _obsRadius; y[0] = down - _robotRadius - _obsRadius;
        x[1] = left - _robotRadius - _obsRadius; y[1] = up + _robotRadius + _obsRadius;
        x[2] = right + _robotRadius + _obsRadius; y[2] = up + _robotRadius + _obsRadius;
        x[3] = right + _robotRadius + _obsRadius; y[3] = down - _robotRadius - _obsRadius;
        lineRectangleInter(a, b, c, x, y, isInter, IPX, IPY);
        if (isInter && (pointOnLineOnSeg(segP1x, segP1y, segP2x, segP2y, IPX[0], IPY[0]) || pointOnLineOnSeg(segP1x, segP1y, segP2x, segP2y, IPX[1], IPY[1])))
            closeDist = 0.0;
        else
            closeDist = minObs(dist2First, dist2Second);
        return closeDist > 0.0;
    }
    if (sqrt((stX - seX) * (stX - seX) + (stY - seY) * (stY - seY)) < EPSILON) { // circle
        closeDist = distSegToPoint(segP1x, segP1y, segP2x, segP2y, stX, stY);
    }
    else {
        closeDist = distSegToSeg(segP1x, segP1y, segP2x, segP2y, stX, stY, seX, seY);
    }
    return closeDist - _obsRadius - _robotRadius - minDist > 0;
}

void ObstacleNew::setParameters(ObstacleNew obst) {
    this->_robotRadius = obst.getRobotRadius();
    this->_obsRadius = obst.getRadius();
    this->_vel = obst.getVel();
    this->_segStart = obst.getStart();
    this->_segEnd = obst.getEnd();
    this->_dynamic = obst.isDynamic();
    this->_valid = obst.isValid();
    this->_type = obst.getType();
}

//====================================================================//
//    Obstacles class implementation
//====================================================================//


void ObstaclesNew::circleRect(CGeoPoint p1, CGeoPoint p2, CVector normv, double radius, vector < CGeoPoint >& c) {
    c.clear();
    c.push_back(p1 + normv * radius);
    c.push_back(p1 + normv * (-radius));
    c.push_back(p2 + normv * (-radius));
    c.push_back(p2 + normv * radius);
}

void ObstaclesNew::drawLongCircle(CGeoPoint p1, CGeoPoint p2, double radius) {
    vector < CGeoPoint > c;
    CVector vec = p1 - p2;
    CVector norm(vec.y(), -vec.x());
    norm = norm / norm.mod();
    circleRect(p1, p2, norm, radius, c);
    for(int i = 0; i < 4; i++)
        GDebugEngine::Instance()->gui_debug_line(c[i], c[(i + 1) % 4], COLOR_GREEN);
    GDebugEngine::Instance()->gui_debug_arc(p2, radius, 0.0, 360.0, COLOR_GREEN);
    GDebugEngine::Instance()->gui_debug_arc(p1, radius, 0.0, 360.0, COLOR_GREEN);
}

void ObstaclesNew::drawCircle(CGeoPoint p, double radius) {
    GDebugEngine::Instance()->gui_debug_arc(p, radius, 0, 360, COLOR_GREEN);
}

void ObstaclesNew::drawRect(CGeoPoint leftUp, CGeoPoint rightDown) {
    double x1 = leftUp.x();
    double y1 = leftUp.y();
    double x2 = rightDown.x();
    double y2 = rightDown.y();
    CGeoPoint p1(x1, y1);
    CGeoPoint p2(x1, y2);
    CGeoPoint p3(x2, y1);
    CGeoPoint p4(x2, y2);

    GDebugEngine::Instance()->gui_debug_line(p1, p2, COLOR_GREEN);
    GDebugEngine::Instance()->gui_debug_line(p4, p2, COLOR_GREEN);
    GDebugEngine::Instance()->gui_debug_line(p1, p3, COLOR_GREEN);
    GDebugEngine::Instance()->gui_debug_line(p3, p4, COLOR_GREEN);
}

void ObstaclesNew::addObs(const CVisionModule * pVision, const TaskT & task, bool drawObs, double oppAvoidDist, double teammateAvoidDist, double ballAvoidDist, bool shrinkTheirPenalty) {
    int rolenum = task.executor;
    int flags = task.player.flag;
    int shootCar = task.ball.Sender;
    CGeoPoint myPos = pVision->ourPlayer(rolenum).Pos();
    const bool isBack = (rolenum == TaskMediator::Instance()->leftBack()) ||
                         (rolenum == TaskMediator::Instance()->rightBack()) ||
                        (rolenum == TaskMediator::Instance()->singleBack()) ||
                        (rolenum == TaskMediator::Instance()->sideBack()) ||
                        (rolenum == TaskMediator::Instance()->defendMiddle());
    bool notAvoidOurBack = (isBack && Utils::InOurPenaltyArea(myPos, 400));
    //后卫只有在禁区边界才允许撞车
    if(isBack) {
        if(!Utils::InOurPenaltyArea(myPos, 40*10)) {
            flags &= (~PlayerStatus::NOT_AVOID_THEIR_VEHICLE);
        } else {
            flags |= PlayerStatus::NOT_AVOID_THEIR_VEHICLE;
        }
        flags &= (~PlayerStatus::NOT_AVOID_OUR_VEHICLE);
    }

//    GDebugEngine::Instance()->gui_debug_msg(myPos, QString("%1").arg(flags & PlayerStatus::NOT_AVOID_PENALTY).toLatin1(), COLOR_BLUE);
    // opp defence area
    if(flags & PlayerStatus::FREE_KICK) {
        addRectangle(CGeoPoint(PARAM::Field::PITCH_LENGTH / 2, -PARAM::Field::PENALTY_AREA_WIDTH / 2 + robotRadius - PARAM::Vehicle::V2::PLAYER_SIZE - 20.0*10),
                    CGeoPoint(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH + robotRadius - PARAM::Vehicle::V2::PLAYER_SIZE - 20.0*10, PARAM::Field::PENALTY_AREA_WIDTH / 2 - robotRadius + PARAM::Vehicle::V2::PLAYER_SIZE + 20.0*10),
                    0);
    }
    else if(!(flags & PlayerStatus::NOT_AVOID_PENALTY)) {
        if(shrinkTheirPenalty) {
            addRectangle(CGeoPoint(PARAM::Field::PITCH_LENGTH / 2, -PARAM::Field::PENALTY_AREA_WIDTH / 2 + robotRadius - PARAM::Vehicle::V2::PLAYER_SIZE),
                        CGeoPoint(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH + robotRadius - PARAM::Vehicle::V2::PLAYER_SIZE , PARAM::Field::PENALTY_AREA_WIDTH / 2 - robotRadius + PARAM::Vehicle::V2::PLAYER_SIZE),
                        0);
        } else {

            addRectangle(CGeoPoint(PARAM::Field::PITCH_LENGTH / 2, -PARAM::Field::PENALTY_AREA_WIDTH / 2 - PARAM::Vehicle::V2::PLAYER_SIZE),
                        CGeoPoint(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH - PARAM::Vehicle::V2::PLAYER_SIZE, PARAM::Field::PENALTY_AREA_WIDTH / 2 + PARAM::Vehicle::V2::PLAYER_SIZE),
                        0);
        }
    }
    CGeoPoint p11((PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::GOAL_POST_THICKNESS + PARAM::Field::GOAL_DEPTH), PARAM::Field::GOAL_WIDTH / 2);
    CGeoPoint p12(PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::GOAL_WIDTH / 2 - PARAM::Field::GOAL_POST_THICKNESS);
    CGeoPoint p21((PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::GOAL_POST_THICKNESS + PARAM::Field::GOAL_DEPTH), -(PARAM::Field::GOAL_WIDTH / 2 - PARAM::Field::GOAL_POST_THICKNESS));
    CGeoPoint p22(PARAM::Field::PITCH_LENGTH / 2, -PARAM::Field::GOAL_WIDTH / 2);
    CGeoPoint p31((PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::GOAL_POST_THICKNESS + PARAM::Field::GOAL_DEPTH + 10), PARAM::Field::GOAL_WIDTH / 2 - PARAM::Field::GOAL_POST_THICKNESS + 10);
    CGeoPoint p32((PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::GOAL_DEPTH), -(PARAM::Field::GOAL_WIDTH / 2 - PARAM::Field::GOAL_POST_THICKNESS));
// TODO
    addRectangle(p11, p12, 0.0);
    addRectangle(p21, p22, 0.0);
    addRectangle(p31, p32, 0.0);

    // avoid shooting line
    if (flags & PlayerStatus::AVOID_SHOOTLINE) {
        const PlayerVisionT& shooter = pVision->ourPlayer(shootCar);
        // center of the gate
        addLongCircle(shooter.RawPos(), CGeoPoint(PARAM::Field::PITCH_LENGTH / 2, 0.0f), CVector(0.0f, 0.0f), 3.0f*10, OBS_LONG_CIRCLE_NEW);
        // left gate post
        addLongCircle(shooter.RawPos(), CGeoPoint(PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::GOAL_WIDTH / 2.0), CVector(0.0f, 0.0f), 3.0f*10, OBS_LONG_CIRCLE_NEW);
        // right gate post
        addLongCircle(shooter.RawPos(), CGeoPoint(PARAM::Field::PITCH_LENGTH / 2, -PARAM::Field::GOAL_WIDTH / 2.0), CVector(0.0f, 0.0f), 3.0f*10, OBS_LONG_CIRCLE_NEW);
    }

    // set up teammates as obstacles
    if (!(flags & PlayerStatus::NOT_AVOID_OUR_VEHICLE)) {
        for (int i = 0; i < PARAM::Field::MAX_PLAYER; ++i) {
            const PlayerVisionT& teammate = pVision->ourPlayer(i);
            if ((i != rolenum) && teammate.Valid()) {
                if(i == TaskMediator::Instance()->rightBack() ||
                        i == TaskMediator::Instance()->leftBack() ||
                        i == TaskMediator::Instance()->singleBack() ||
                        i == TaskMediator::Instance()->sideBack() ||
                        i == TaskMediator::Instance()->defendMiddle() ||
                        WorldModel::Instance()->CurrentRefereeMsg() == "OurTimeout") {
                    if(!notAvoidOurBack ||  WorldModel::Instance()->CurrentRefereeMsg() == "OurTimeout") {
                        addCircle(teammate.RawPos(), teammate.Vel(), PARAM::Vehicle::V2::PLAYER_SIZE, OBS_CIRCLE_NEW);
                    }
                }
                else {
                    double factor;
                    if(teammate.Vel().mod() < LOWER_BOUND_AVOID_SPEED) factor = 0.0;
                    else if(teammate.Vel().mod() < UPPER_BOUND_AVOID_SPEED) factor = (teammate.Vel().mod() - LOWER_BOUND_AVOID_SPEED) / (UPPER_BOUND_AVOID_SPEED - LOWER_BOUND_AVOID_SPEED);
                    else factor = 1.0;
                    double tempAvoidDist;
                    if(teammateAvoidDist < MIN_AVOID_DIST)
                        tempAvoidDist = MIN_AVOID_DIST;
                    else
                        tempAvoidDist = MIN_AVOID_DIST + factor * (teammateAvoidDist - MIN_AVOID_DIST);
                    addCircle(teammate.RawPos(), teammate.Vel(), tempAvoidDist, OBS_CIRCLE_NEW);
                }
            }
        }
    }

    // set up opponents as obstacles
    if (!(flags & PlayerStatus::NOT_AVOID_THEIR_VEHICLE)) {
        for (int i = 0; i < PARAM::Field::MAX_PLAYER; ++i) {
            const PlayerVisionT& opp = pVision->theirPlayer(i);
            if (opp.Valid()) {
                double factor;
                if(opp.Vel().mod() < LOWER_BOUND_AVOID_SPEED) factor = 0.0;
                else if(opp.Vel().mod() < UPPER_BOUND_AVOID_SPEED) factor = (opp.Vel().mod() - LOWER_BOUND_AVOID_SPEED) / (UPPER_BOUND_AVOID_SPEED - LOWER_BOUND_AVOID_SPEED);
                else factor = 1.0;
                double tempAvoidDist;
                if(oppAvoidDist < MIN_AVOID_DIST)
                    tempAvoidDist = MIN_AVOID_DIST;
                else
                    tempAvoidDist = MIN_AVOID_DIST + factor * (oppAvoidDist - MIN_AVOID_DIST);
                addCircle(opp.RawPos(), opp.Vel(), tempAvoidDist, OBS_CIRCLE_NEW);
            }
        }
    }

    if (task.executor != TaskMediator::Instance()->goalie()) {
        if(!(flags & PlayerStatus::NOT_AVOID_PENALTY))
            addRectangle(CGeoPoint(-PARAM::Field::PITCH_LENGTH / 2, -PARAM::Field::PENALTY_AREA_WIDTH / 2 + robotRadius - PARAM::Vehicle::V2::PLAYER_SIZE),
                        CGeoPoint(-PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH - robotRadius + PARAM::Vehicle::V2::PLAYER_SIZE, PARAM::Field::PENALTY_AREA_WIDTH / 2 - robotRadius + PARAM::Vehicle::V2::PLAYER_SIZE),
                        0);
        CGeoPoint p11(-(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::GOAL_POST_THICKNESS + PARAM::Field::GOAL_DEPTH), PARAM::Field::GOAL_WIDTH / 2);
        CGeoPoint p12(-PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::GOAL_WIDTH / 2 - PARAM::Field::GOAL_POST_THICKNESS);
        CGeoPoint p21(-(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::GOAL_POST_THICKNESS + PARAM::Field::GOAL_DEPTH), -(PARAM::Field::GOAL_WIDTH / 2 - PARAM::Field::GOAL_POST_THICKNESS));
        CGeoPoint p22(-PARAM::Field::PITCH_LENGTH / 2, -PARAM::Field::GOAL_WIDTH / 2);
        CGeoPoint p31(-(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::GOAL_POST_THICKNESS + PARAM::Field::GOAL_DEPTH + 10), PARAM::Field::GOAL_WIDTH / 2 - PARAM::Field::GOAL_POST_THICKNESS + 10);
        CGeoPoint p32(-(PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::GOAL_DEPTH), -(PARAM::Field::GOAL_WIDTH / 2 - PARAM::Field::GOAL_POST_THICKNESS));
        addRectangle(p11, p12, 0.0);
        addRectangle(p21, p22, 0.0);
        addRectangle(p31, p32, 0.0);
    }

    // ball
    if (flags & PlayerStatus::DODGE_BALL) {
        const ObjectPoseT& ball = pVision->ball();
        addCircle(ball.RawPos(), ball.Vel(), ballAvoidDist, OBS_CIRCLE_NEW);
    }

    if (WorldModel::Instance()->CurrentRefereeMsg() == "GameStop" ||
            (flags & PlayerStatus::AVOID_STOP_BALL_CIRCLE)) {
        const ObjectPoseT& ball = pVision->ball();
        addCircle(ball.RawPos(), CVector(0.0f, 0.0f), stopBallAvoidDist, OBS_CIRCLE_NEW);
    }

    if (drawObs) drawObstacles();
}

void ObstaclesNew::addLongCircle(const CGeoPoint& x0, const CGeoPoint& x1, const CVector& v, double r, int type, bool isDynamic, bool drawObst) {
    if (obs.size() > MAX_OBSTACLESNEW) return;
    ObstacleNew obst;
    obst.setPos(x0, x1);
    obst.setType(type);
    obst.setVel(v);
    obst.setObsRadius(r);
    obst.setDynamic(isDynamic);
    obst.setRobotRadius(robotRadius);
    obst.setValid(true);
    num++;
    obs.push_back(obst);
    if(drawObst)
        drawLongCircle(x0, x1, r);
}

void ObstaclesNew::addCircle(const CGeoPoint& x0, const CVector& v, float r, int type, bool isDynamic, bool drawObst) {
    if (obs.size() > MAX_OBSTACLESNEW) return;
    ObstacleNew obst;
    obst.setPos(x0);
    obst.setType(type);
    obst.setVel(v);
    obst.setObsRadius(r);
    obst.setDynamic(isDynamic);
    obst.setRobotRadius(robotRadius);
    obst.setValid(true);
    num++;
    obs.push_back(obst);
    if(drawObst)
        drawCircle(x0, r);
}

void ObstaclesNew::addRectangle(const CGeoPoint& x0, const CGeoPoint& x1, double r, int type, bool drawObst) {
    if (obs.size() > MAX_OBSTACLESNEW) return;
    ObstacleNew obst;
    obst.setPos(x0, x1);
    obst.setType(type);
    obst.setVel(CVector(0, 0));
    obst.setObsRadius(r);
    obst.setDynamic(false);
    obst.setRobotRadius(robotRadius);
    obst.setValid(true);
    num++;
    obs.push_back(obst);
    if(drawObst)
        drawRect(x0, x1);
}

void ObstaclesNew::drawObstacles() {
    for (ObstacleNew obst : obs) {
        if (obst.getType() == OBS_CIRCLE_NEW) {
            drawCircle(obst.getStart(), obst.getRadius());
        }
        else if (obst.getType() == OBS_RECTANGLE_NEW) {
            drawRect(obst.getStart(), obst.getEnd());
        }
        else {
            drawLongCircle(obst.getStart(), obst.getEnd(), obst.getRadius());
        }
    }
}

// return true if there will be collision
bool ObstaclesNew::check(const CGeoPoint& p) {
    float px = p.x(), py = p.y();
    for (ObstacleNew obst : obs) {
        if (!obst.check(px, py)) return false;
    }
    return true;
}

bool ObstaclesNew::check(const CGeoPoint& p, int & id) {
    float px = p.x(), py = p.y();
    size_t obsNum = obs.size();
    for (int i = 0; i < obsNum; i++) {
        if (!obs[i].check(px, py)) {
            id = i;
            return false;
        }
    }
    return true;
}

bool ObstaclesNew::check(const CGeoPoint& p1, const CGeoPoint& p2) {
    float p1x = p1.x(), p1y = p1.y(), p2x = p2.x(), p2y = p2.y();
    for (ObstacleNew obst : obs) {
        if (!obst.check(p1x, p1y, p2x, p2y)) {
            return false;
        }
    }
    return true;
}

bool ObstaclesNew::check(const CGeoPoint &p1, const CGeoPoint &p2, int &id) {
    float p1x = p1.x(), p1y = p1.y(), p2x = p2.x(), p2y = p2.y();
    size_t obsNum = obs.size();
    for (int i = 0; i < obsNum; i++) {
        if (!obs[i].check(p1x, p1y, p2x, p2y)) {
            id = i;
            return false;
        }
    }
    return true;
}

void ObstaclesNew::changeWorld(const stateNew &curState, double dMax) {
    int numOfObs = obs.size();
    double distToStop, distWithObs;
    double scale = 5;
    double obstacalMove, obstacleLength;
    CGeoPoint segStartNew, segEndNew;
    double uncertainDist = 2.0*10;

    for(int i = 0; i < numOfObs; i++) {
        CVector obstacleVel = obs[i].getVel();
        double obstacleSpeed = obs[i].getVel().mod();
        distWithObs = obs[i].getStart().dist(curState.pos);
        if(obs[i].getType() == OBS_CIRCLE_NEW) {
            if(curState.vel.mod() > ZERO_VEL /*&& curState.vel.mod() < MAX_PROB_VEL*/) { //自身速度不为零，距离很近的障碍物变形
                distToStop = curState.vel.mod2() / (2 * dMax);
                if(distToStop > distWithObs - obs[i].getRadius() - ROBOT_RADIUS) {
                    segStartNew = obs[i].getStart();
                    segEndNew = segStartNew + (-curState.vel) / (curState.vel.mod() * distToStop);
                    obs[i].setPos(segStartNew, segEndNew);
                    obs[i].setType(OBS_LONG_CIRCLE_NEW);
                    addLongCircle(segStartNew, segEndNew, obs[i].getVel(), obs[i].getRadius());
                }
            }
            if(obstacleSpeed > ZERO_VEL /*&& curState.vel.mod() < MAX_PROB_VEL*/) { //障碍物速度不为零，对障碍物未来的位置进行预测，作为新的障碍物加入
                obstacalMove = obstacleSpeed * FRAME_PERIOD * scale;
                segStartNew = obs[i].getStart() + Utils::Polar2Vector(obstacalMove, obs[i].getVel().dir());
                segEndNew = obs[i].getEnd() + Utils::Polar2Vector(obstacalMove, obs[i].getVel().dir());
                obs[i].setPos(segStartNew, segEndNew);
                obs[i].setType(OBS_LONG_CIRCLE_NEW);
                obstacleLength = obstacalMove + uncertainDist;
                addLongCircle(segStartNew + (-obstacleVel / obstacleVel.mod() * obstacleLength / 4), segEndNew + (obstacleVel / obstacleVel.mod() * obstacleLength * 2.5), CVector(0, 0), obs[i].getRadius(), OBS_CIRCLE_NEW);
            }
        }
    }
}
