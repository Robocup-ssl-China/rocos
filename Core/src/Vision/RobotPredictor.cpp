#include "RobotPredictor.h"
#include <utils.h>
#include "RobotsCollision.h"
#include <string.h>
using namespace std;

CRobotPredictor::CRobotPredictor() : _robotLostTime(0), _isHasRotation(true) {
}

CRobotPredictor::CRobotPredictor(bool isHasRotation) : _robotLostTime(0), _isHasRotation(isHasRotation) {
}
void CRobotPredictor::updateVision(int cycle, const VehicleInfoT& player, const ObjectPoseT& ball, bool invert) {
    // !!!!这里好像有问题:
    // 如果是对手车也能识别朝向,就会调用updateOurVision, 但因为在该函数里取不到cmd(只有我方车才有cmd存下来), 结果就不对.
    // 这里需要一个辨别我方车与对方车的方式!!
    RobotVisionData& predictedVision = _visionLogger.getVision(cycle);
    const double x = invert ? -player.pos.x : player.pos.x;
    const double y = invert ? -player.pos.y : player.pos.y;
    const double raw_x = invert ? -player.rawPos.x : player.rawPos.x;
    const double raw_y = invert ? -player.rawPos.y : player.rawPos.y;
    const double vel_x = invert ? -player.vel.x() : player.vel.x();
    const double vel_y = invert ? -player.vel.y() : player.vel.y();
    const double raw_vel_x = invert ? -player.raw_vel.x() : player.raw_vel.x();
    const double raw_vel_y = invert ? -player.raw_vel.y() : player.raw_vel.y();
    const double acc_x = invert ? -player.accelerate.x() : player.accelerate.x();
    const double acc_y = invert ? -player.accelerate.y() : player.accelerate.y();
    //std::cout << "Id:" << realNum << "\tPos: " << x << "," << y << "\tVel:" << player.vel
    //         << "\tdir:" << player.dir << std::endl;
    double dir, rawdir, dirVel, rawDirVel;

    dir = invert ? Utils::Normalize(player.dir + PARAM::Math::PI) : Utils::Normalize(player.dir);
    rawdir = invert ? Utils::Normalize(player.rawdir + PARAM::Math::PI) : Utils::Normalize(player.rawdir);

    if(fabs(player.dirvel) > 6 || fabs(player.raw_dirVel) > 6) {
        dirVel = 0;
        rawDirVel = 0;
    } else {
        dirVel = player.dirvel;
        rawDirVel = player.raw_dirVel;
    }

    predictedVision.cycle = cycle;
    predictedVision.SetRawPos(raw_x, raw_y);
    predictedVision.SetPos(x, y);
    predictedVision.SetDir(dir);
    predictedVision.SetRawDir(rawdir);
    predictedVision.SetValid(player.valid);
    predictedVision.SetAcc(CVector(acc_x, acc_y));
    predictedVision.SetRawVel(CVector(raw_vel_x, raw_vel_y));
    predictedVision.SetRawRotVel(rawDirVel);

    if (_isHasRotation) {//our robot
        if(_commandLogger.commandValid(cycle - 1))
            cmd = _commandLogger.getCommand(cycle - 1);
        else {
            cmd.rot_speed = 0;
            cmd.vel = CVector(0, 0);
        }
        // 用前一帧的命令当做当前的速度(考虑图像实际测得的速度!!! 实验结果表明: 考虑图像速度后整体会慢下来,效果不好)
        CVector updateVel = cmd.vel.rotate(_visionLogger.getVision(cycle - 1).Dir()) * 1;
        predictedVision.SetVel(updateVel);
        // modified by zhyaic 2014.3.26, 此处不能用下发的旋转速度做为此时机器人的旋转速度，尤其是当机器人在y方向有较大分量时
        // 这个地方应引起注意!!!!!!
        predictedVision.SetRotVel(cmd.rot_speed);
    } else {
        predictedVision.SetVel(CVector(vel_x, vel_y));
        predictedVision.SetRotVel(dirVel);
    }
}

void CRobotPredictor::updateCommand(int cycle, const CPlayerCommand * cmd) {
    _commandLogger.setCommand(cycle, cmd->getAffectedVel(), cmd->getAffectedRotateSpeed());
}
