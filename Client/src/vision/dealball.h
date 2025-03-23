#ifndef DEALBALL_H
#define DEALBALL_H

#include <zos/utils/singleton.h>
#include "globaldata.h"
#include "messageformat.h"
#include "geometry.h"
#include "kalmanfilter.h"

/**
 * @brief main class of process ball
 */
class CDealBall {
  public:
    CDealBall();
    void run();
    void choseBall();
    bool getValid() {
        return validBall;
    }
    double getBallSpeed() {
        return result.ball[0].velocity.mod();
    }
    void updateVel(const Matrix2d& tempMatrix, ReceiveVisionMessage& result);
  private:
    double posDist(CGeoPoint, CGeoPoint);
    bool ballNearVechile(Msg::Ball, double);
    void init();
    void mergeBall();
    Msg::Ball ballSequence[PARAM::BALLNUM][PARAM::CAMERA];
    Msg::Ball lastBall, currentBall;
    double lastPossible, currentPossible;
    ReceiveVisionMessage result;
    int actualBallNum = 0;
    int minBelieveFrame;
    int lostFrame;
    bool validBall;
    double upPossible, downPossible;
    long long _cycle, lastFrame, currentFrame;
    double _lastValidDir;
};
typedef Singleton<CDealBall> DealBall;
#endif // DEALBALL_H
