#include "PlayerCommandV2.h"
#include <utils.h>
#include <sstream>
#include <CommandInterface.h>
#include <RobotCapability.h>
#include "PlayInterface.h"
#include "TaskMediator.h"

namespace{
	const bool SPEED_MODE = true;
	const double RSPEED_DRIBBLEHIGH = 1;	// 当旋转速度大于此值使用

	const double ACTION_ERROR = 1.0; // 执行误差
}

/************************************************************************/
/*                      CPlayerSpeed                                    */
/************************************************************************/
CPlayerSpeedV2::CPlayerSpeedV2(int number, double xspeed, double yspeed, double rspeed, unsigned char dribble, int flag, bool need_report)
	: CPlayerCommand(number,dribble,need_report)
	,_rspeed(rspeed)
{
	const CRobotCapability* robotCap = RobotCapFactory::Instance()->getRobotCap(1, 1);
	CVector velocity(xspeed, yspeed);

	// 把输出指令限制在小车的运动能力之内 added by qxz 08.06.07
    if (flag == 0) {
        if (number == TaskMediator::Instance()->goalie()) {
            if (velocity.mod() > robotCap->maxSpeedGoalie(0))
                velocity = Utils::Polar2Vector(robotCap->maxSpeedGoalie(0), velocity.dir());
        } else if (TaskMediator::Instance()->leftBack() != 0 && number == TaskMediator::Instance()->leftBack()
            || TaskMediator::Instance()->rightBack() != 0 && number == TaskMediator::Instance()->rightBack()
            || TaskMediator::Instance()->singleBack() != 0 && number == TaskMediator::Instance()->singleBack()
            || TaskMediator::Instance()->sideBack() != 0 && number == TaskMediator::Instance()->sideBack()) {
                if (velocity.mod() > robotCap->maxSpeedBack(0))
                    velocity = Utils::Polar2Vector(robotCap->maxSpeedBack(0), velocity.dir());
        } else {
            if (velocity.mod() > robotCap->maxSpeed(0))
                velocity = Utils::Polar2Vector(robotCap->maxSpeed(0), velocity.dir());
        }
    }
    else {
        if (velocity.mod() > robotCap->maxRushTranslationSpeed())
            velocity = Utils::Polar2Vector(robotCap->maxRushTranslationSpeed(), velocity.dir());
    }

	_xspeed = velocity.x();
	_yspeed = velocity.y();
}

void CPlayerSpeedV2::execute(int realNum){
    CCommandInterface::instance()->setNeedReport(realNum, this->_need_report);
    CCommandInterface::instance()->setSpeed(realNum, dribble(), xspeed(), yspeed(), rspeed());
}
void CPlayerSpeedV2::toStream(std::ostream& os) const
{
	os << xspeed() << "\t" << yspeed() << "\t" << rspeed();
}
CVector CPlayerSpeedV2::getAffectedVel() const
{
	return CVector(xspeed(), yspeed());
}
double CPlayerSpeedV2::getAffectedRotateSpeed() const
{
	return rspeed();
}

/************************************************************************/
/*                         Gyro(陀螺仪)                                 */
/************************************************************************/
void CPlayerGyroV2::execute()
{
    std::cout << "Unknown execute in CPlayerGyroV2" << std::endl;
}
void CPlayerGyroV2::toStream(std::ostream& os) const
{
	os << "rotNum "<< number()<<"radius "<<_radius<<"angle "<<_angle;
}
