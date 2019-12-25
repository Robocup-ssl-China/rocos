/**
* @file BallPredictor.h
* 此文件为球位置信息预测类定义.
* @date $Date: 2004/06/20 11:52:30 $
* @version $Revision: 1.6 $
* @author peter@mail.ustc.edu.cn
*/

#ifndef _BALL_PREDICTOR_H_
#define _BALL_PREDICTOR_H_
#include <geometry.h>
#include "WorldDefine.h"
#include <vector>
#include <server.h>
#include "staticparams.h"
#include <ServerInterface.h>
namespace {
const int VALID_NUM = 7;   //  做最小二乘需要的帧数
const double MAX_SPEED =  1000;
const double MAX_DIST = 1000.0 / PARAM::Vision::FRAME_RATE * 4;
const int MAX_LOGS = 16;
}

class BallVisionData : public MobileVisionT {
  public:
    BallVisionData(): cycle(-1) {}
    int cycle;
};

class CBallVisionLogger {
  public:
    CBallVisionLogger() {}
    bool visionValid(int cycle) const {
        return _vision[cycle % MAX_INFORMATIONS].cycle == cycle;
    }
    const BallVisionData& getVision(int cycle) const {
        return _vision[(cycle + MAX_INFORMATIONS) % MAX_INFORMATIONS];
    }
    BallVisionData& getVision(int cycle) {
        return _vision[(cycle + MAX_INFORMATIONS) % MAX_INFORMATIONS];
    }
    void setVision(int cycle, const BallVisionData& vision) {
        _vision[cycle % MAX_INFORMATIONS] = vision;
        _vision[cycle % MAX_INFORMATIONS].cycle = cycle;
    }

  private:
    static const int MAX_INFORMATIONS = MAX_LOGS; // 保存n个周期的信息
    BallVisionData _vision[MAX_INFORMATIONS]; //保存预测的历史信息
};
/**
* CBallPredictor.
* 根据视觉预测球的位置信息
*/
class CBallPredictor {
  public:
    CBallPredictor();
    ~CBallPredictor();
    void updateVision( const CServerInterface::VisualInfo& vInfo, bool invert);// 更新视觉信息
//    void updateVision(int cycle, const PosT& pos, const CVector& vel, bool invert, bool);
    int ballLostTime() const {
        return _ballLostTime;
    }
    BallVisionData& getData(int cycle) {
        return _visionLogger.getVision(cycle);
    }
    const MobileVisionT& getResult(int cycle) const {
        return _visionLogger.getVision(cycle);
    }
    void setCollisionResult(int cycle, const MobileVisionT& ball);
    int visibility() const {
        return _visibility;
    }
    int activity() const {
        return _activity;
    }
    void setPos(const CGeoPoint & pos) {
        _visionLogger.getVision(_cycle).SetPos(pos);
    }
    void setPos(int cycle, const CGeoPoint & pos) {
        _visionLogger.getVision(cycle).SetPos(pos);
    }
    void setVel(int cycle, const CVector & vel) {
        _visionLogger.getVision(cycle).SetVel(vel);
    }
    bool isSpeedError()const {
        return _errorSpeed;
    }
  protected:
    void predictLost(int cycle); // 球看不到的时候预测球的位置
    bool checkValid(int cycle); // 去掉不合理的情况
  private:
    CBallVisionLogger _visionLogger;
    MobileVisionT _ballLinePredictData[60];
    int _ballLostTime; // 看不到球的次数
    int _ballInvalidMovedCycle; // 球的信息不合理的周期数
    int _visibility, _activity; // 可见度和活动度
    bool _hasCollision;
    int _cycle;
    bool _errorSpeed;
    CGeoPoint _lastRawBallPos; //filted
    PosT _visionDataQueue[VALID_NUM];  // 历史数据
};
#endif // _BALL_PREDICTOR_H_

