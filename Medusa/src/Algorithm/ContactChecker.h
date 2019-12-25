#ifndef _CONTACT_CHECKER_H
#define _CONTACT_CHECKER_H
#include "staticparams.h"
#include <WorldDefine.h>
#include "BallPredictor.h"
#include "RobotPredictor.h"
#include "utils.h"
class CVisionModule;

struct Last_Contact {
    int frame;
    int robotnum;
    int type;
    Last_Contact(): frame(0), robotnum(0), type(0) {}
};

class ContactChecker {
  private:
    int _lastContactNum;
    int _previousNum;
    bool _lastContactReliable;
    //vector<Last_Contact> _ballContact;
    std::vector<PlayerVisionT> AllPlayer;
    Last_Contact _last_contact;

    MobileVisionT _ball;
    MobileVisionT _lastball;
    CVector ball_direction_before;
    CVector ball_direction_after;
    double speed_before ;
    double speed_after ;
    double speed_diff_percept ;
    double angle_to;
    long cycle;
    bool _isBallMoving;
    long _outFieldCycle;
    bool _isBallOutField;
    bool _isJudged;

  public:
    ContactChecker();
    bool isBallOutField() {
        return _isBallOutField;
    }
    void OutFieldJudge(const CVisionModule* pVision);
    int getContactNum() const {
        return _lastContactNum;
    }
    void setContactNum(int num) {
        _lastContactNum = num;
    }
    bool getContactReliable() const {
        return _lastContactReliable;
    }
    void setContactReliable(bool x) {
        _lastContactReliable = x;
    }
    void prepareJudge(const CVisionModule* pVision);
    void judgeOne();
    void judgeTwo();
    void judgeThree();
    void judgeFour();
    void judgeFive();
    void internalReliableJudge();
    void externReliableJudge();

    void ballMovingJudge(const CVisionModule* pVision);

    void ContactCheck(const CVisionModule* pVision);

    void refereeJudge(const CVisionModule* pVision);
};

#endif
