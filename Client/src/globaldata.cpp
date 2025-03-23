#include "globaldata.h"
#include "parammanager.h"
#include "staticparams.h"
namespace  {
auto vpm = ZSS::VParamManager::instance();
auto zpm = ZSS::ZParamManager::instance();
}
CGlobalData::CGlobalData(): ctrlC(false)
    , maintain(ZSS::Athena::Vision::MAINTAIN_STORE_BUFFER)
    , processBall(ZSS::Athena::Vision::BALL_STORE_BUFFER)
    , processRobot(ZSS::Athena::Vision::ROBOT_STORE_BUFFER) {
}

double CGlobalData::calculateWeight(const int camID, const CGeoPoint& pos){
    return 1.0;
}
// MARKTODO
// double CDealBall::calculateWeight(int camID, CGeoPoint ballPos) {
//     SingleCamera camera = GlobalData::instance()->cameraMatrix[camID];
//     if (ballPos.x() > camera.leftedge.max && ballPos.x() < camera.rightedge.max &&
//             ballPos.y() > camera.downedge.max && ballPos.y() < camera.upedge.max)
//         return 1;
//     else if (ballPos.x() < camera.leftedge.max && ballPos.x() > camera.leftedge.min)
//         return abs(ballPos.x() - camera.leftedge.min) / abs(camera.leftedge.max - camera.leftedge.min);
//     else if (ballPos.x() > camera.rightedge.max && ballPos.x() < camera.rightedge.min)
//         return abs(ballPos.x() - camera.rightedge.min) / abs(camera.rightedge.max - camera.rightedge.min);
//     else if (ballPos.y() < camera.downedge.max && ballPos.y() > camera.downedge.min)
//         return abs(ballPos.y() - camera.downedge.min) / abs(camera.downedge.max - camera.downedge.min);
//     else if (ballPos.y() > camera.upedge.max && ballPos.y() < camera.upedge.min)
//         return abs(ballPos.y() - camera.upedge.min) / abs(camera.upedge.max - camera.upedge.min);
//     else return 1e-8;//to deal with can see out of border situation
// }