#ifndef DEALROBOT_H
#define DEALROBOT_H
#include "kalmanfilter.h"
#include "kalmanfilterdir.h"

#include <zos/utils/singleton.h>
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
    Msg::Robot robotSeqence[2][PARAM::ROBOTMAXID][PARAM::CAMERA];
    ReceiveVisionMessage result;
    Msg::Robot sortTemp[2][PARAM::ROBOTMAXID];
    Msg::Robot lastRobot[2][PARAM::ROBOTMAXID];
    Msg::Robot currentRobot[2][PARAM::ROBOTMAXID];
    void init();
    double posDist(CGeoPoint, CGeoPoint);
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
