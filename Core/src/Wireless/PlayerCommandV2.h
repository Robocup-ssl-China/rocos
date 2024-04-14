#ifndef _PLAYER_COMMAND_V2_H_
#define _PLAYER_COMMAND_V2_H_
#include <PlayerCommand.h>
/************************************************************************/
/*                         Speed                                        */
/************************************************************************/
class CPlayerSpeedV2 : public CPlayerCommand{
public:
    CPlayerSpeedV2(int number, double xspeed, double yspeed, double rspeed, unsigned char dribble, int flag = 0, bool need_report = false);
    virtual void execute(int realNum);	// 2015/7/11 add by bykang
	virtual CVector getAffectedVel() const;
	virtual double getAffectedRotateSpeed() const;
	virtual void toStream(std::ostream& os) const;
protected:
	double xspeed() const{ return _xspeed; }
	double yspeed() const{ return _yspeed; }
	double rspeed() const{ return _rspeed; }
	int commandType() const;
private:
	double _xspeed;
	double _yspeed;
	double _rspeed;
};

/************************************************************************/
/*                         Gyro(陀螺仪)                                 */
/************************************************************************/
class CPlayerGyroV2 : public CPlayerCommand{
public:
	CPlayerGyroV2(int number, unsigned char dribble, unsigned int radius, double angle,  double rspeed)
		: CPlayerCommand(number, dribble), _radius(radius), _angle(angle), _rspeed(rspeed){ }
    virtual void execute();
	virtual void toStream(std::ostream& os) const;
protected:
	double radius() const { return _radius; }
	double angle() const { return _angle; }
	double rspeed() const { return _rspeed; }
private:
	unsigned int _radius;	// 旋转半径
	double _angle;			// 旋转角度
	double _rspeed;			// 旋转速度
};
#endif
