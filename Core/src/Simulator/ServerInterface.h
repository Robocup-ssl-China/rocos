#ifndef _SERVER_INTERFACE_H_
#define _SERVER_INTERFACE_H_
/************************************************************************/
/*                      和Server的通讯接口                              */
/************************************************************************/
#include "geometry.h"
#include "staticparams.h"
struct RefRecvMsg{
	int blueGoal;
	int yellowGoal;
	int timeRemain;
	int blueGoalie;
	int yellowGoalie;
};
class COptionModule;

struct PosT {
    PosT(): valid(0), x(0), y(0) {}
    int valid;
    float x;
    float y;
    void setValid(int _value) {
        valid = _value;
    }
};

struct VehicleInfoT {
    VehicleInfoT(): dir(0), type(0), dirvel(0) {}
    PosT pos;
    PosT rawPos;
    CVector vel;
    CVector raw_vel;
    float dir;//radius
    float rawdir;
    float dirvel;
    float raw_dirVel;
    CVector accelerate;
    int type;
};
enum ballState {received, touched, kicked, struggle, chip_pass, flat_pass};

struct VisualInfoT {
    VisualInfoT(): cycle(0), BallState(received), BallLastTouch(0), mode(0) {}
    unsigned int cycle;
    VehicleInfoT player[PARAM::TEAMS][PARAM::Field::MAX_PLAYER];
    PosT ball;
    PosT rawBall;
    PosT chipPredict;
    CVector BallVel;
    ballState BallState;
    int BallLastTouch;
    int mode;
    int next_command;
    //unsigned char ourRobotIndexBefore[ PARAM::Field::MAX_PLAYER] = {};
    int before_cycle[ 2 * PARAM::Field::MAX_PLAYER] = {};
    // unsigned char theirRobotIndexBefore[ PARAM::Field::MAX_PLAYER] = {};
    //PosT imageBall;
    PosT ballPlacePosition;
};

class CServerInterface{
public:
	typedef VisualInfoT VisualInfo;
};
#endif //_SERVER_INTERFACE_H_
