#include "Speed.h"
#include <CommandFactory.h>
#include "VisionModule.h"
#include "DribbleStatus.h"

CSpeed::CSpeed()
{

}

void CSpeed::plan(const CVisionModule* pVision)
{

}

CPlayerCommand* CSpeed::execute(const CVisionModule* pVision)
{
	int myNum = task().executor;
	double myDir = pVision->ourPlayer(myNum).Dir();
	double speed_x = task().player.speed_x; // x方向平动速度
	double speed_y = task().player.speed_y; // y方向平动速度
	double rotSpeed = task().player.rotate_speed; // 转动速度
	double dribblePower = DribbleStatus::Instance()->getDribbleCommand(myNum);
	/*task().player.flag & PlayerStatus::DRIBBLING? DRIBBLE_LOWEST: DRIBBLE_DISABLED*/ // 吸球力度
	CVector globalVel(speed_x, speed_y); // 全局坐标系中的速度矢量
	CVector localVel = globalVel.rotate(-myDir);
	return CmdFactory::Instance()->newCommand(CPlayerSpeedV2(myNum, localVel.x(), localVel.y(), rotSpeed, dribblePower));
}
