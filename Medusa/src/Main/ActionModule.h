/************************************************************************/
/* Copyright (c) CSC-RL, Zhejiang University							*/
/* Team:			SSL-ZJUNlict										*/
/* HomePage: http://www.nlict.zju.edu.cn/ssl/WelcomePage.html			*/
/************************************************************************/
/* File:	ActionModule.h												*/
/* Brief:	C++ Interface: Action	execution 							*/
/* Func:	Provide an action command send interface					*/
/* Author:	cliffyin, 2012, 08											*/
/* Refer:	NONE														*/
/* E-mail:	cliffyin007@gmail.com										*/
/* Version:	0.0.1														*/
/* Changelog:	2012-08-17	note the code								*/
/************************************************************************/	

#ifndef _ACTION_MODULE_H_
#define _ACTION_MODULE_H_

//////////////////////////////////////////////////////////////////////////
// include files
#include "DecisionModule.h"						// Decision for the match

/**
@brief		比赛动作模块，提供外部调用接口。
@details	根据决策模块的执行结果，依次进行单车的动作控制，下发运动指令。
*/
class CActionModule {
public:

	/// <summary>	构造函数，Constructor. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="pOption">  	The running option. </param>
	/// <param name="pVision">  	[in,out] If non-null, the vision. </param>
	/// <param name="pDecision">	The decision module. </param>

    CActionModule(CVisionModule* pVision,const CDecisionModule* pDecision);

	/// <summary>	析构函数，Finaliser. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>

	~CActionModule(void);

	/// <summary>	小车运动指令下发接口，Action command send interface, specify, Mainly. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="robotIndex">	Real index of the vehicle. </param>
	///
	/// <returns>	true if it succeeds, false if it fails. </returns>

    bool sendAction();

	/// <summary>	停止小车运动下发接口，Stop action command send interface， Wireless off,TODO. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="robotIndex">	Real index of the vehicle. </param>
	///
	/// <returns>	true if it succeeds, false if it fails. </returns>

    bool sendNoAction();

	/// <summary>	停止小车运动下发接口，Stop all command send interface, Error Stop, TODO. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>

	void stopAll();

private:

	/// <summary> 当前比赛运行模式，The running option </summary>
	const COptionModule* _pOption;

	/// <summary> 当前比赛视觉模块，The vision module </summary>
	CVisionModule* _pVision;

	/// <summary> 当前比赛决策模块，The specified decision </summary>
	const CDecisionModule* _pDecision;
};

#endif // ~_ACTION_MODULE_H_
