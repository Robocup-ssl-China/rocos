#ifndef _BALL_STATUS_H_
#define _BALL_STATUS_H_

#include "singleton.hpp"
#include "VisionModule.h"
#include "ContactChecker.h"

/************************************************************************/
/*				球的策略性状态数据的读写界面                            */
/* 输入:																*/
/* 1. 传感器信息：	图像模块（VisionModule)  +  双向通讯（RobotSensor)	*/
/* 2. 历史决策信息：踢球状态（KickStatus） + 带球状态（DribbleStatus)	*/
/*																		*/
/* 输出：																*/
/* 1.底层信息： 是否球被踢出（哪方哪个球员）							*/
/*				是否球被控住（哪方哪个球员）							*/
/* 2.高层信息： 传射配合												*/
/************************************************************************/

#define MAX_CMD_STORED 10

/// 哪方触球
enum emWhoTouchedBall{
	WTB_Us = 1,			// 我方
	WTB_Them = 2,		// 对方
	WTB_Unknown = 3		// 不能确定
};

enum BallState{
	None,
	OurBall,
	StandOff,
	WaitAdvance,
	GiveUpAdvance,
};


class CSendCmd{
public:
	CSendCmd() : _number(0), _normalKick(0), _chipKick(0), _dribble(0) {}
	void setKickCmd(int num, int normalKick, int chipKick, unsigned char dribble) {
		_number = num;
		_normalKick = normalKick;
		_chipKick = chipKick;
		_dribble = dribble;
	}
	void clear(void){ _number=0; _normalKick=0; _chipKick=0; _dribble=0; }

	int num() { return _number; }
	double normalKick() { return _normalKick; }
	double chipKick() { return _chipKick; }
	unsigned char dribble() { return _dribble; }

private:
	int _number;			// 车号
	double _normalKick;		// 平射踢出力度
	double _chipKick;		// 挑射踢出力度
	unsigned char _dribble; // 带球档数

};

/// 球状态封装类
class CBallStatus{
public:
	CBallStatus(void);
	~CBallStatus(void){}
	
public:
	// 对外的总更新接口，需要每个周期被调用一次
	void UpdateBallStatus(const CVisionModule* pVision);

	// 获取球运动速度
	CVector ballMovingVel(void) { return _ballMovingVel; }

	CGeoLine ballChipLine(void) { return _ballChipLine; }

	// 球被踢出,便返回true(可能是挑射,也可能是平射)
    bool IsBallKickedOut(void) const { return _isKickedOut; }

	// 球被某个角色踢出,便返回true(可能是挑射,也可能是平射)
    bool IsBallKickedOut(int num) const { return _isKickedOut && num == _kickerNum;}

	// 球踢出,且是挑球指令踢出,才返回true
    bool IsChipKickedOut(void) const { return _isChipKickOut && _isKickedOut; }

	// 球被某个角色踢出,且是挑球指令踢出,才返回true;
    bool IsChipKickedOut(int num) const { return IsChipKickedOut() && num == _kickerNum; }
	// 得到踢球队员的号码
	int getKickerNum() { return _kickerNum; }

	int getBallToucher(){
		return _ballToucher;
	}
	int getLastBallToucher() {
		return _lastBallToucher;
	}
	bool ballToucherChanged(){
		return _ballToucher!=_lastBallToucher;
	}

	
private:
	// 球状态被更新的周期
	int _ballStatusCycle;

	// 更新球运动速度
	void UpdateBallMoving(const CVisionModule* pVision);
	void CheckKickOutBall(const CVisionModule* pVision);

	CVector _ballMovingVel;
	CGeoLine _ballChipLine;

	// 更新哪方最后碰到球
	bool _isKickedOut;
	int _kickerNum;
	bool _isChipKickOut;
	int _ballToucher;
	int _lastBallToucher;
	int _chipCycle;

	ContactChecker _contactChecker;

	// 先将PlayInterface的信息加入BallStatus中
public:
    std::string checkBallState(const CVisionModule* pVision,int meNum=0);
	void clearBallStateCouter();
	void setChipKickState(bool b_set){ _chipkickstate = b_set;}
	bool getChipKickState()const { return _chipkickstate; }
	void setCommand(CSendCmd kickCmd, int cycle);
	void setCommand(int num, int normalKick, int chipKick, unsigned char dribble, int cycle);
	CSendCmd getKickCommand(int num, int cycle) { return _kickCmd[num][cycle % MAX_CMD_STORED]; }
	void clearKickCmd(int num, int cycle){ _kickCmd[num][cycle % MAX_CMD_STORED].clear(); }
	void clearKickCmd(); // 清除所有车指令
	void initializeCmdStored();
private:
	CSendCmd _kickCmd[PARAM::Field::MAX_PLAYER][MAX_CMD_STORED];
	bool _chipkickstate; // true为处在空中,false为已经落地
	int _ballState;
	int _ballStateCouter;
};

typedef Singleton< CBallStatus > BallStatus;

#endif
