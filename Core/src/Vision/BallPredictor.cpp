#include "BallPredictor.h"
#include <utils.h>
#include <fstream>
#include <istream>
#include "geometry.h"
#include "WorldDefine.h"
#include "GDebugEngine.h"
#include "parammanager.h"
namespace {
const int MAX_BALL_LOST_TIME = 30;

bool LOG_BALL_SPEED = false;
std::ofstream ballSpeedLog;
}


CBallPredictor::CBallPredictor() : _cycle(0), _errorSpeed(false), _ballInvalidMovedCycle(0), _ballLostTime(2000), _hasCollision(false), _visibility(0), _activity(0), _lastRawBallPos(0, 0) {
    ZSS::ZParamManager::instance()->loadParam(LOG_BALL_SPEED, "Debug/BallLog", false);
}

CBallPredictor::~CBallPredictor() {
    if ( ballSpeedLog.is_open() )
        ballSpeedLog.close();
}

void CBallPredictor::updateVision( const CServerInterface::VisualInfo& vInfo, bool invert) {
    // 每次处理一个_visionLogger里的数据，只用来保留预测输出
    // 该函数的处理结果就是更新thisCycle帧里面的球信息
    BallVisionData& thisCycle = _visionLogger.getVision(vInfo.cycle);
    const int invertFactor = invert ? -1 : 1;

    thisCycle.SetVel(vInfo.BallVel * invertFactor);
    thisCycle.SetRawPos(vInfo.rawBall.x * invertFactor, vInfo.rawBall.y * invertFactor);
    thisCycle.SetChipPredict(vInfo.chipPredict.x, vInfo.chipPredict.y);
    thisCycle.SetPos(vInfo.ball.x * invertFactor,  vInfo.ball.y * invertFactor);
    thisCycle.SetValid(vInfo.ball.valid);
    thisCycle.cycle =  vInfo.cycle;
    return;
}

void CBallPredictor::predictLost(int cycle) {
    BallVisionData& thisCycle = _visionLogger.getVision(cycle);
    if (_ballLostTime < MAX_BALL_LOST_TIME /*&& _visionLogger.visionValid(cycle - 1)*/
       ) {
        BallVisionData& thisCycle = _visionLogger.getVision(cycle);
        const BallVisionData& lastCycle = _visionLogger.getVision(cycle - 1);

        thisCycle.SetPos(lastCycle.Pos() + lastCycle.Vel() / PARAM::Vision::FRAME_RATE); // 位置
        thisCycle.SetVel(lastCycle.Vel() * ( 1 + PARAM::Field::BALL_DECAY / PARAM::Vision::FRAME_RATE )); // 速度
        thisCycle.SetValid(true);
    } else {
        // 很长时间没有看到球了,保持原来的位置?
        thisCycle.SetValid(false); // 信息不可用
    }
    //_ballFilter.reset();

    return ;
}

bool CBallPredictor::checkValid(int cycle) {
    if (! _visionLogger.visionValid(cycle)) {
        return false;
    }

    if (! _visionLogger.visionValid(cycle - 1) || ! _visionLogger.getVision(cycle - 1).Valid()) {
        return true;	//如果上个周期预测不可用,那么总是信任这个周期的信息
    }

    const double BALL_OUT_BUFFER = (PARAM::Rule::Version == 2003) ? -20 : -50;
    const double MAX_BALL_MOVE_DIST_PER_CYCLE = 30; //每一祯球的最大位移
    BallVisionData& thisCycle = _visionLogger.getVision(cycle);
    const BallVisionData& lastCycle = _visionLogger.getVision(cycle - 1);
    if (!Utils::IsInField(thisCycle.RawPos(), BALL_OUT_BUFFER)) {
        return false; // 球不可能在场外,故信息不对
    }

    if ((thisCycle.RawPos() - lastCycle.RawPos()).mod2() > MAX_BALL_MOVE_DIST_PER_CYCLE * MAX_BALL_MOVE_DIST_PER_CYCLE) {
        if (++_ballInvalidMovedCycle < 5) {
            thisCycle.SetRawPos(lastCycle.RawPos() + lastCycle.Vel() / PARAM::Vision::FRAME_RATE);//处理视觉杂点的过程，根据试验修正
            return false; // 不可能这么快
        }
    } else {
        _ballInvalidMovedCycle = 0;
    }

    return true;
}

void CBallPredictor::setCollisionResult(int cycle, const ObjectPoseT& ball) {
    ObjectPoseT& oldBall = _visionLogger.getVision(cycle);
    oldBall = ball;
    _hasCollision = true;

    return ;
}
