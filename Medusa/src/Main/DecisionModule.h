/************************************************************************/
/* Copyright (c) CSC-RL, Zhejiang University							*/
/* Team:			SSL-ZJUNlict										*/
/* HomePage: http://www.nlict.zju.edu.cn/ssl/WelcomePage.html			*/
/************************************************************************/
/* File:	DecisionModule.h											*/
/* Brief:	C++ Interface: Decision	execution 							*/
/* Func:	Provide a decision interface for Strategy, Play selecting	*/
/* Author:	cliffyin, 2012, 08											*/
/* Refer:	NONE														*/
/* E-mail:	cliffyin007@gmail.com										*/
/* Version:	0.0.1														*/
/* Changelog:	2012-08-17	note the code								*/
/************************************************************************/

#ifndef _DECISION_MODULE_H_
#define _DECISION_MODULE_H_

//////////////////////////////////////////////////////////////////////////
// include files
#include <OptionModule.h>					/// Options for the match
#include <VisionModule.h>					/// Vision for the match

//////////////////////////////////////////////////////////////////////////
// forward declaration
class CPlayerTask;

/**
@brief		比赛【单车模式和团队模式】决策模块，提供外部调用接口。
@details	根据当前的比赛模式，结合视觉等模块选取合适的多车Play并分配任务。
*/
class CDecisionModule {
public:

	/// <summary>	构造函数，Constructor. </summary>
	///
	/// <remarks>	ZjuNlict, 2011-7-25. </remarks>
	///
	/// <param name="pOption">	The running option. </param>
	/// <param name="pVision">	[in,out] If non-null, the vision. </param>

    CDecisionModule(CVisionModule* pVision);

	/// <summary>	析构函数，Finaliser. </summary>
	///
	/// <remarks>	ZjuNlict, 2011-7-25. </remarks>

	~CDecisionModule(void);

	/// <summary>	外部执行接口，Executes the decision operation. </summary>
	///
	/// <remarks>	ZjuNlict, 2011-7-25. </remarks>
	///
	/// <param name="visualStop">	The visual stop flag. </param>

	void DoDecision(const bool visualStop);
protected:

	/// <summary>	停止对应的小车的运动，Stop the robot. </summary>
	///
	/// <remarks>	ZjuNlict, 2011-7-25. </remarks>
	///
	/// <param name="num">	Vehicle num. </param>

	void GenerateTasks(const bool visualStop);

	/// <summary>	多车配合模式，Executes the team mode operation. </summary>
	///
	/// <remarks>	ZjuNlict, 2011-7-25. </remarks>

	void DoTeamMode();

	/// <summary>	任务小车进行任务规划，Plan tasks for every task-assigned vehicle. </summary>
	///
	/// <remarks>	ZjuNlict, 2011-7-25. </remarks>

	void PlanTasks();

    //生成截球点和截球时间信息
    //add by Wang in 2018/5/31
    void calculateModuleRun();

private:
	/// <summary> 当前比赛运行模式，The running option </summary>
//	const COptionModule* _pOption;

	/// <summary> 当前比赛视觉模块，The vision module </summary>
	CVisionModule* _pVision;
};

#endif // ~_DECISION_MODULE_H_
