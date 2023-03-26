#ifndef _PLAYER_COMMAND_H_
#define _PLAYER_COMMAND_H_
#include <ServerInterface.h>
#include <misc_types.h>
#include <string>
#include <ostream>

class CPlayerCommand{
public:
	CPlayerCommand(int number = 0, unsigned char dribble = 0,bool need_report = false) : _number(number), _dribble(dribble), _need_report(need_report){}
	virtual ~CPlayerCommand() { }
	// 执行动作
    virtual void execute(int){}
	// 动作的效果
	virtual CVector getAffectedVel() const { return CVector(0,0); } // 动作的效果
	virtual double getAffectedRotateSpeed() const { return 0; } // 动作的效果
	virtual void toStream(std::ostream& os) const { os << "Invalid Command"; } // 输出为字符串
	friend std::ostream& operator << (std::ostream& os, const CPlayerCommand& cmd)
	{
		cmd.toStream(os);
		return os;
	}
	int number() const { return _number; } // 队员的号码
	unsigned char dribble() const { return _dribble; } // 是否带球
private:
	int _number; ///<执行者的号码
	unsigned char _dribble; ///<是否同时带球
protected:
	bool _need_report = false;
};
#endif
