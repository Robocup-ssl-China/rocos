/************************************************************************/
/* Copyright (c) CSC-RL, Zhejiang University							*/
/* Team:			SSL-ZJUNlict										*/
/* HomePage: http://www.nlict.zju.edu.cn/ssl/WelcomePage.html			*/
/************************************************************************/
/* File:	ActionModule.h												*/
/* Brief:	C++ Implementation: Action	execution						*/
/* Func:	Provide an action command send interface					*/
/* Author:	cliffyin, 2012, 08											*/
/* Refer:	NONE														*/
/* E-mail:	cliffyin007@gmail.com										*/
/************************************************************************/

#include "ActionModule.h"
#include <KickStatus.h>
#include <DribbleStatus.h>
#include <TaskMediator.h>
#include <PlayerCommandV2.h>
#include <PlayInterface.h>
#include <CommandFactory.h>
#include <PathPlanner.h>
#include "Semaphore.h"
#include "CommandInterface.h"
extern Semaphore decision_to_action;

CActionModule::CActionModule(CVisionModule* pVision, const CDecisionModule* pDecision)
    : _pVision(pVision), _pDecision(pDecision) {
}

CActionModule::~CActionModule(void) {

}

// 用于当场上有的车号大于5的情况

bool CActionModule::sendAction() {
    decision_to_action.Wait();
    /************************************************************************/
    /* 第一步：遍历小车，执行赋予的任务，生成动作指令                       */
    /************************************************************************/
    for (int vecNum = 0; vecNum < PARAM::Field::MAX_PLAYER; ++ vecNum) {
        // 获取当前小车任务
        CPlayerTask* pTask = TaskMediator::Instance()->getPlayerTask(vecNum);
        // 没有任务，跳过
        if (nullptr == pTask) {
            continue;
        }

        // 执行skill，任务层层调用执行，得到最终的指令：<vx vy w> + <kick dribble>
        // 执行的结果：命令接口（仿真-DCom，实物-CommandSender） + 指令记录（运动-Vision，踢控-PlayInterface)
        bool dribble = false;
        CPlayerCommand* pCmd = nullptr;
        pCmd = pTask->execute(_pVision);

        // 跑：有效的运动指令
        if (pCmd) {
            dribble = pCmd->dribble() > 0;
            // 下发运动 <vx vy w>
            pCmd->execute(vecNum); //this by Wang
            // 记录指令
            _pVision->setPlayerCommand(pCmd->number(), pCmd);
        }

        // 踢：有效的踢控指令
        double kickPower = 0.0;
        double chipkickDist = 0.0;
        double passdist = 0.0;
        if (KickStatus::Instance()->needKick(vecNum)) {
            // 更新踢相关参数
            kickPower = KickStatus::Instance()->getKickPower(vecNum);
            chipkickDist = KickStatus::Instance()->getChipKickDist(vecNum);
            passdist = KickStatus::Instance()->getPassDist(vecNum);
            // 涉及到平/挑射分档，这里只关系相关参数，实际分档请关注 CommandSender
            CPlayerKickV2 kickCmd(vecNum, kickPower, chipkickDist, passdist, dribble);
            // 机构动作 <kick dribble>
            kickCmd.execute(vecNum);
        } else {
            CPlayerKickV2 kickCmd(vecNum, 0, 0, 0, dribble);
            kickCmd.execute(vecNum);
        }

    }

    /************************************************************************/
    /* 第二步：指令清空处理                                                 */
    /************************************************************************/
    // 清除上一周期的射门指令
    KickStatus::Instance()->clearAll();
    // 清除上一周期的控球指令
    DribbleStatus::Instance()->clearDribbleCommand();
    // 清除上一周期的障碍物标记
    CPathPlanner::resetObstacleMask();

    // 实物模式设定模式进行指令下发
    CCommandInterface::instance()->sendCommands();

    return true;
}