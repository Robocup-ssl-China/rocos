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
#include "staticparams.h"
#include <VisionTypeDef.h>
namespace {
const int VALID_NUM = 7;   //  做最小二乘需要的帧数
const double MAX_SPEED =  1000;
const double MAX_DIST = 1000.0 / PARAM::Vision::FRAME_RATE * 4;
const int MAX_LOGS = 16;
}

class BallVisionData : public ObjectPoseT {
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
    CBallPredictor() = default;
    ~CBallPredictor() = default;
    void updateVision( const VisualInfoT& vInfo, bool invert);// 更新视觉信息
    BallVisionData& getData(int cycle) {
        return _visionLogger.getVision(cycle);
    }
    const ObjectPoseT& getResult(int cycle) const {
        return _visionLogger.getVision(cycle);
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
//   protected:
    // bool checkValid(int cycle); // 去掉不合理的情况
  private:
    CBallVisionLogger _visionLogger;
    ObjectPoseT _ballLinePredictData[60];
    int _cycle;
};
#endif // _BALL_PREDICTOR_H_

