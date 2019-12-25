#include "WorldModel.h"
#include "VisionModule.h"
#include "RobotCapability.h"
#include "ControlModel.h"
#include "RobotSensor.h"
#include "KickStatus.h"
#include "DribbleStatus.h"
#include "GDebugEngine.h"
#include "PlayInterface.h"
#include "BufferCounter.h"

// 默认参数初始化
const int CWorldModel::myDefaultNum = 0;
const int CWorldModel::enemyDefaultNum = 0;

// 自己到球的矢量
const CVector CWorldModel::self2ball(int current_cycle,  int myNum) {
	static int last_cycle[PARAM::Field::MAX_PLAYER] = {-1,-1,-1,-1,-1,-1};
	static CVector _self2ball[PARAM::Field::MAX_PLAYER];

	if (last_cycle[myNum-1] < current_cycle) {
		_self2ball[myNum-1] = _pVision->ball().Pos() - _pVision->ourPlayer(myNum).Pos();
		last_cycle[myNum-1] = current_cycle;
	}

	return _self2ball[myNum-1];
}

// 自己到球的距离
double CWorldModel::self2ballDist(int current_cycle,  int myNum,  int enemyNum) {
    return self2ball(current_cycle, myNum).mod();	// 可以减少计算量
}

// 自己到球的角度
double CWorldModel::self2ballDir(int current_cycle,  int myNum,  int enemyNum) {
    return self2ball(current_cycle, myNum).dir();	// 可以减少计算量
}

const string CWorldModel::CurrentRefereeMsg()
{
	return vision()->getCurrentRefereeMsg();
}
void CWorldModel::SPlayFSMSwitchClearAll(bool clear_flag)
{
    if (! clear_flag) {
        return ;
    }
    // 暂时只有清理 球被提出的状态
    KickStatus::Instance()->resetKick2ForceClose(true,this->vision()->getCycle());
    BallStatus::Instance()->clearKickCmd();
    BufferCounter::Instance()->clear();
    // TODO
    return ;
}
