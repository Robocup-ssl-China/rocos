#include "TrainEnv.h"
#include "CommandInterface.h"

void RLEnvironment::placeRobot(int num, double x, double y, double dir){
    CCommandInterface::instance()->placeRobot(num, x, y, dir);
}
void RLEnvironment::placeBall(double x, double y, double vx, double vy){
    CCommandInterface::instance()->placeBall(x, y, vx, vy);
}