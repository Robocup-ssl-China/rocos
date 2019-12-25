#ifndef DEALROBOT_H
#define DEALROBOT_H
#include "kalmanfilter.h"
#include "kalmanfilterdir.h"

#include <singleton.hpp>
#include <messageformat.h>
/**
 * @brief main class of process robot
 */
class CDealRobot {
  public:
    CDealRobot();
    void run();
    void mergeRobot();
    void selectRobot();
    void updateVel(int team, ReceiveVisionMessage& result);
//    bool updateCommand(Robot_Command command);

  private:
    Robot robotSeqence[2][PARAM::ROBOTMAXID][PARAM::CAMERA];
    ReceiveVisionMessage result;
    Robot sortTemp[2][PARAM::ROBOTMAXID];
    Robot lastRobot[2][PARAM::ROBOTMAXID];
    Robot currentRobot[2][PARAM::ROBOTMAXID];
    void init();
    double posDist(CGeoPoint, CGeoPoint);
    double calculateWeight(int camID, CGeoPoint roboPos);
    void sortRobot(int);
    bool isOnField(CGeoPoint);
    KalmanFilter _kalmanFilter[2][PARAM::ROBOTMAXID];
    KalmanFilter _dirFilter[2][PARAM::ROBOTMAXID];
    bool filteDir;
    int validNum[2];
    int minBelieveFrame, ourMaxLostFrame, theirMaxLostFrame, fieldWidth, fieldHeight;
    double upPossible, decidePossible, ourDownPossible, theirDownPossible;
};
typedef Singleton <CDealRobot> DealRobot;

#endif // DEALROBOT_H
