#ifndef _TASK_FACTORY_V2_H_
#define _TASK_FACTORY_V2_H_

#include <singleton.hpp>
#include <misc_types.h>
#include "staticparams.h"
/************************************************************************/
/*                         CTaskFactoryV2                               */
/************************************************************************/

class CPlayerTask;
class CTaskFactoryV2{
public:
    CPlayerTask* GotoPosition(const TaskT& task);
    CPlayerTask* SmartGotoPosition(const TaskT& task);
	CPlayerTask* StopRobot(const TaskT& task);
	CPlayerTask* Speed(const TaskT& task); // by zc
    CPlayerTask* OpenSpeed(const TaskT& task);
    CPlayerTask* Goalie(const TaskT& task);
    CPlayerTask* Touch(const TaskT& task);
    CPlayerTask* GetBall(const TaskT& task);
protected:
	template < class CTaskType >
	CPlayerTask* MakeTask( const TaskT& task );
};
typedef Singleton<CTaskFactoryV2> TaskFactoryV2;
/************************************************************************/
/*                 一些包裹(wrap)函数, 方便调用tasks                    */
/************************************************************************/

//! 调用skill的接口
namespace PlayerRole{
	extern CPlayerTask* makeItStop(const int num, const int flags = 0);
    extern CPlayerTask* makeItGoto(const int num, const CGeoPoint& target, const double dir, const int flags = 0, const int sender = 0);
    extern CPlayerTask* makeItGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotvel, const int flags = 0, const int sender = 0);
    extern CPlayerTask* makeItGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotVel, const double maxAcc, const double maxRotAcc, const double maxSpeed = 0, const double maxRotSpeed = 0, const int flags = 0, const int sender = 0);
    extern CPlayerTask* makeItSimpleGoto(const int num, const CGeoPoint& target, const double dir, const int flags = 0);
    extern CPlayerTask* makeItSimpleGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotvel, const int flags=0);
    extern CPlayerTask* makeItSimpleGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotVel, const double maxAcc, const double maxRotAcc, const double maxSpeed = 0, const double maxRotSpeed = 0, const int flags = 0);

    extern CPlayerTask* makeItSmartGoto(const int num, const CGeoPoint& target, const double dir, const int flags = 0);
    extern CPlayerTask* makeItSmartGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotvel, const int flags=0);
    extern CPlayerTask* makeItSmartGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotVel, const double maxAcc, const double maxRotAcc, const double maxSpeed = 0, const double maxRotSpeed = 0, const int flags = 0);

    extern CPlayerTask* makeItRun(const int num, const double speedX, const double speedY, const double rotSpeed, int flags = 0);
    extern CPlayerTask* makeItOpenRun(const int num, const double speedX, const double speedY, const double rotSpeed, int flags = 0, bool needReport = false);
    extern CPlayerTask* makeItGetBall(const int num, const CGeoPoint& pos, const double power, const double precision, const int flag = 0, const int skill_flag = 0);
}

#endif // _TASK_FACTORY_V2_H_
