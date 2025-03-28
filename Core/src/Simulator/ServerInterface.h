#ifndef _SERVER_INTERFACE_H_
#define _SERVER_INTERFACE_H_

#include "geometry.h"
#include "staticparams.h"
struct RefRecvMsg{
	int blueGoal;
	int yellowGoal;
	int timeRemain;
	int blueGoalie;
	int yellowGoalie;
};

struct VehicleInfoT {
    bool valid = false;
    Point2D pos;
    Point2D rawPos;
    CVector vel;
    CVector raw_vel;
    float dir = 0;
    float rawdir = 0;
    float dirvel = 0;
    float raw_dirVel;
    CVector accelerate;
    int type = 0;
};

enum BallState {received, touched, kicked, struggle, chip_pass, flat_pass};
struct BallInfoT{
    bool valid = false;
    Point2D pos;
    Point2D rawPos;
    Point2D chipPredict;
    CVector vel;
    BallState state = BallState::received;
    int lastTouch = 0;
    Point2D placementPos;
};

struct VisualInfoT {
    VisualInfoT(): cycle(0), mode(0) {}
    unsigned int cycle;
    VehicleInfoT player[PARAM::TEAMS][PARAM::Field::MAX_PLAYER];
    BallInfoT ball;
    int mode;
    int next_command;
    int before_cycle[ 2 * PARAM::Field::MAX_PLAYER] = {};
};

#endif //_SERVER_INTERFACE_H_
