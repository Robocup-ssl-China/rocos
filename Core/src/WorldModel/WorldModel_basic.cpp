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
namespace{
    const double beta = 1.0;
    bool deltaMarginKickingMetric(int current_cycle, double gt, double delta, double mydir, int myNum)
    {
        // 计算当前的margin并做记录
        double gl = Utils::Normalize(gt - delta);	// 目标方向向左偏置射门精度阈值
        double gr = Utils::Normalize(gt + delta);	// 目标方向向右偏置射门精度阈值
        double current_margin = max(Utils::Normalize(mydir - gl), Utils::Normalize(gr - mydir));
        return (current_margin > 0 && current_margin < beta*2*delta);
    }
    ///> 关于控球
    // 控球控制阈值
    const bool Allow_Start_Dribble = true;
    const double Allow_Start_Dribble_Dist = 6.0 * PARAM::Vehicle::V2::PLAYER_SIZE;
    const double Allow_Start_Dribble_Angle = 5.0 * PARAM::Vehicle::V2::KICK_ANGLE;
}
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

    // not used, replaced by new lua bufcnt
    BufferCounter::Instance()->clear();
    // TODO
    return ;
}
bool CWorldModel::IsBallKicked(int num){
    return (RobotSensor::Instance()->IsKickerOn(num) != 0);
}
bool CWorldModel::IsInfraredOn(int num){
    return (RobotSensor::Instance()->IsInfraredOn(num));
}
int CWorldModel::InfraredOnCount(int num){
    return (RobotSensor::Instance()->infraredOn(num));
}
int CWorldModel::InfraredOffCount(int num){
    return (RobotSensor::Instance()->infraredOff(num));
}
bool CWorldModel::KickDirArrived(int current_cycle, double kickdir, double kickdirprecision, int myNum){
    const PlayerVisionT& me = this->_pVision->ourPlayer(myNum);
    return ::deltaMarginKickingMetric(current_cycle,kickdir,kickdirprecision,me.Dir(),myNum);
}
