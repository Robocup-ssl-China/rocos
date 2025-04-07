/**
* @file RobotPredictor.h
* 此文件为机器人位置信息预测类的定义.
* @date $Date: 2004/06/18 01:50:22 $
* @version $Revision: 1.12 $
* @author peter@mail.ustc.edu.cn
*/

#ifndef _ROBOT_PREDICTOR_H_
#define _ROBOT_PREDICTOR_H_

#include "WorldDefine.h"
//#include "FilteredObject.h"
#include "RobotPredictData.h"
#include "RobotPredictError.h"
#include <PlayerCommand.h>
#include <qdebug.h>
/**
* CRobotPredictor.
* 通过机器人的命令结合视觉确定机器人的位置信息
*/
class CRobotPredictor {
  public:
    CRobotPredictor();
    CRobotPredictor (bool isHasRotaion);
    void updateCommand(int cycle, const CPlayerCommand* cmd); // 更新命令
    void updateVision(int cycle, const VehicleInfoT& player, const ObjectPoseT& ball, bool invert); // 更新视觉信息
    void updateOurVision(int cycle, const VehicleInfoT& player, const ObjectPoseT& ball, bool invert, int realNum); // 更新视觉信息
    void updateTheirVision(int cycle, const VehicleInfoT& player, const ObjectPoseT& ball, bool invert, int realNum); // 更新视觉信息

    CRobotCommandLogger& getCommandLogger() {
        return _commandLogger;
    }
    const RobotVisionData& getResult(int cycle) const {
        return _visionLogger.getVision(cycle);
    }
    const RobotRawVisionData& getRawData(int cycle) const {
        return _rawVisionLogger.getVision(cycle);
    }
    RobotVisionData& getData(int cycle) {
        return _visionLogger.getVision(cycle);
    }
    const CVector getRawSpeed(int cycle) const {
        return CVector(2 * _filterData.getVision(cycle).xVel, 2 * _filterData.getVision(cycle).yVel);
    }
    int lostTime() const {
        return _robotLostTime;
    }
    void setIsHasRotation(bool isHasRot) {
        _isHasRotation = isHasRot;
    }
  private:
    int _robotLostTime; // 看不到的周期数
    bool _isHasRotation; // 小车是否具有角度(朝向)信息
    CRobotCommandLogger _commandLogger; // 命令纪录
    CRobotRawVisionLogger _rawVisionLogger; // 原始视觉信息记录
    CRobotRawVisionLogger _filterData;
    CRobotVisionLogger _visionLogger; // 预测过的信息记录
    RobotCommandEffect cmd;
};
#endif // _ROBOT_PREDICTOR_H_
