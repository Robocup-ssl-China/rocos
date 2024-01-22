#include "Factory.h"
#include <TaskMediator.h>

#include <misc_types.h>
#include "CommandFactory.h"
#include "GDebugEngine.h"

/************************************************************************/
/*								concrete skill							*/
/************************************************************************/
//1.atom skills(TODO)
#include "GotoPosition.h"
#include "SmartGotoPosition.h"

//2.basic skills(TODO)
#include "StopRobot.h"
#include "Speed.h"
#include "OpenSpeed.h"

// test
#include "Goalie.h"
#include "Touch.h"
/************************************************************************/
/*                      TaskFactoryV2									*/
/* 请注意:																*/
/* 上层（CStatedScriptPlay) 通过调用 CBasicPlay 中的 setTask() 设定任务	*/
/* 并将每个策略执行小车对应的任务存放在 TaskMediator 里面				*/
/* 执行（ActionModule）通过任务的 execute() 接口生成任务小车对应的指令	*/
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
// an template used as a task pool for storing tasks for executing player
template < class CTaskType >
CPlayerTask* CTaskFactoryV2::MakeTask(const TaskT& task )
{
	static CTaskType taskPool[PARAM::Field::MAX_PLAYER];
	taskPool[task.executor].reset(task);
	return &taskPool[task.executor];
}

//////////////////////////////////////////////////////////////////////////
// current now debugged skill for game
CPlayerTask* CTaskFactoryV2::GotoPosition(const TaskT& task) {
	return MakeTask< CGotoPositionV2 >(task);
}

CPlayerTask* CTaskFactoryV2::SmartGotoPosition(const TaskT& task) {
    return MakeTask< CSmartGotoPositionV2 >(task);
}
CPlayerTask* CTaskFactoryV2::Goalie(const TaskT& task) {
    return MakeTask< CGoalie >(task);
}

CPlayerTask* CTaskFactoryV2::Touch(const TaskT& task) {
    return MakeTask< CTouch >(task);
}

CPlayerTask* CTaskFactoryV2::StopRobot(const TaskT& task) {
	return MakeTask< CStopRobotV2 >(task);
}

CPlayerTask	* CTaskFactoryV2::Speed(const TaskT& task) {
	return MakeTask< CSpeed >(task);
}

CPlayerTask	* CTaskFactoryV2::OpenSpeed(const TaskT& task) {
	return MakeTask< COpenSpeed >(task);
}

//////////////////////////////////////////////////////////////////////////
// define the namespace used to provide interface for task calling
namespace PlayerRole {
	CPlayerTask* makeItStop(const int num, const int flags)
	{
		static TaskT playerTask;
		playerTask.executor = num;
		playerTask.player.flag = flags;
		return TaskFactoryV2::Instance()->StopRobot(playerTask);
	}
	CPlayerTask* makeItGoto(const int num, const CGeoPoint& target, const double dir, const int flags, const int sender)
	{
		static TaskT playerTask;
		playerTask.executor = num;
		playerTask.player.pos = target;
        playerTask.player.vel = CVector(0.0,0.0);
		playerTask.player.rotvel = 0;
		playerTask.player.angle = dir;
		playerTask.player.flag = flags;
		playerTask.ball.Sender = sender;
		return TaskFactoryV2::Instance()->SmartGotoPosition(playerTask);
	}
	CPlayerTask* makeItGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotvel, const int flags, const int sender)
	{
		static TaskT playerTask;
		playerTask.executor = num;
		playerTask.player.pos = target;
		playerTask.player.angle = dir;
		playerTask.player.vel = vel;
		playerTask.player.rotvel = rotvel;
		playerTask.player.flag = flags;
		playerTask.ball.Sender = sender;
		return TaskFactoryV2::Instance()->SmartGotoPosition(playerTask);
	}

    CPlayerTask* makeItGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotVel, const double maxAcc, const double maxRotAcc, const double maxSpeed, const double maxRotSpeed, const int flags, const int sender) {
        static TaskT playerTask;
        playerTask.executor = num;
        playerTask.player.pos = target;
        playerTask.player.angle = dir;
        playerTask.player.vel = vel;
        playerTask.player.rotvel = rotVel;
        playerTask.player.max_acceleration = maxAcc;
        playerTask.player.max_rot_acceleration = maxRotAcc;
        playerTask.player.max_speed = maxSpeed;
        playerTask.player.max_rot_speed = maxRotSpeed;
        playerTask.player.flag = flags;
        playerTask.ball.Sender = sender;
        return TaskFactoryV2::Instance()->SmartGotoPosition(playerTask);
    }


	CPlayerTask* makeItSimpleGoto(const int num, const CGeoPoint& target, const double dir, const int flags)
	{
		static TaskT playerTask;
        playerTask.executor = num;
		playerTask.player.pos = target;
		playerTask.player.vel = CVector(0.0,0.0);
		playerTask.player.rotvel = 0;
		playerTask.player.angle = dir;
		playerTask.player.flag = flags;
		return TaskFactoryV2::Instance()->GotoPosition(playerTask);
	}


	CPlayerTask* makeItSimpleGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotvel, const int flags)
	{
		static TaskT playerTask;
		playerTask.executor = num;
		playerTask.player.pos = target;
		playerTask.player.angle = dir;
		playerTask.player.vel = vel;
		playerTask.player.rotvel = rotvel;
		playerTask.player.flag = flags;
		return TaskFactoryV2::Instance()->GotoPosition(playerTask);
	}

    CPlayerTask* makeItSimpleGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotVel, const double maxAcc, const double maxRotAcc, const double maxSpeed, const double maxRotSpeed, const int flags) {
        static TaskT playerTask;
        playerTask.executor = num;
        playerTask.player.pos = target;
        playerTask.player.angle = dir;
        playerTask.player.vel = vel;
        playerTask.player.rotvel = rotVel;
        playerTask.player.max_acceleration = maxAcc;
        playerTask.player.max_rot_acceleration = maxRotAcc;
        playerTask.player.max_speed = maxSpeed;
        playerTask.player.max_rot_speed = maxRotSpeed;
        playerTask.player.flag = flags;
        return TaskFactoryV2::Instance()->GotoPosition(playerTask);
    }

    CPlayerTask* makeItSmartGoto(const int num, const CGeoPoint& target, const double dir, const int flags)
    {
        static TaskT playerTask;
        playerTask.executor = num;
        playerTask.player.pos = target;
        playerTask.player.vel = CVector(0.0,0.0);
        playerTask.player.rotvel = 0;
        playerTask.player.angle = dir;
        playerTask.player.flag = flags;
        return TaskFactoryV2::Instance()->SmartGotoPosition(playerTask);
    }

    CPlayerTask* makeItSmartGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotvel, const int flags)
    {
        static TaskT playerTask;
        playerTask.executor = num;
        playerTask.player.pos = target;
        playerTask.player.angle = dir;
        playerTask.player.vel = vel;
        playerTask.player.rotvel = rotvel;
        playerTask.player.flag = flags;
        return TaskFactoryV2::Instance()->SmartGotoPosition(playerTask);
    }

    CPlayerTask* makeItSmartGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotVel, const double maxAcc, const double maxRotAcc, const double maxSpeed, const double maxRotSpeed, const int flags) {
        static TaskT playerTask;
        playerTask.executor = num;
        playerTask.player.pos = target;
        playerTask.player.angle = dir;
        playerTask.player.vel = vel;
        playerTask.player.rotvel = rotVel;
        playerTask.player.max_acceleration = maxAcc;
        playerTask.player.max_rot_acceleration = maxRotAcc;
        playerTask.player.max_speed = maxSpeed;
        playerTask.player.max_rot_speed = maxRotSpeed;
        playerTask.player.flag = flags;
        return TaskFactoryV2::Instance()->SmartGotoPosition(playerTask);
    }

	CPlayerTask* makeItRun(const int num, const double speedX, const double speedY, const double rotSpeed, int flags)
	{
		static TaskT playerTask;
		playerTask.executor = num;
		playerTask.player.speed_x = speedX;
		playerTask.player.speed_y = speedY;
		playerTask.player.rotate_speed = rotSpeed;
		playerTask.player.flag = flags;
		return TaskFactoryV2::Instance()->Speed(playerTask);
	}
    CPlayerTask* makeItOpenRun(const int num, const double speedX, const double speedY, const double rotSpeed, int flags, bool needReport)
    {
        static TaskT playerTask;
        playerTask.executor = num;
        playerTask.player.speed_x = speedX;
        playerTask.player.speed_y = speedY;
        playerTask.player.rotate_speed = rotSpeed;
        playerTask.player.flag = flags;
        playerTask.player.needReport = needReport;
        return TaskFactoryV2::Instance()->OpenSpeed(playerTask);
    }
}
