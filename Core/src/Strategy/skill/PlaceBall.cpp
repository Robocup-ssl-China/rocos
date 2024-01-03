#include "PlaceBall.h"
#include "VisionModule.h"
#include "RobotSensor.h"
void CPlaceBall::plan(const CVisionModule* pVision){
    if (pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
    }
    const int runner = task().executor;
    bool frared = RobotSensor::Instance()->IsInfraredOn(runner);
    CPlayerTask::plan(pVision);
}