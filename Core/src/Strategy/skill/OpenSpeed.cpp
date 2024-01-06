#include "OpenSpeed.h"
#include "CommandFactory.h"
#include "DribbleStatus.h"
#include "VisionModule.h"
#include <cmath>
#include <fstream>

#include <iostream>

COpenSpeed::COpenSpeed() {

}

CPlayerCommand* COpenSpeed::execute(const CVisionModule* pVision) {
	int num = task().executor;
	double speedX = task().player.speed_x; // x方向平动速度
	double speedY = task().player.speed_y; // y方向平动速度
	double speedR = task().player.rotate_speed; // 转动速度
	double dribblePower = DribbleStatus::Instance()->getDribbleCommand(num);
	int flag = task().player.flag;
	if (flag & PlayerStatus::DRIBBLING && dribblePower < 0.5){
        dribblePower = 3.0;
	}
	bool needReport = task().player.needReport;
	return CmdFactory::Instance()->newCommand(CPlayerSpeedV2(num, speedX, speedY, speedR, dribblePower, flag, needReport));
}
