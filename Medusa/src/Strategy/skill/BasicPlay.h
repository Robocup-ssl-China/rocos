/************************************************************************/
/* Copyright (c) CSC-RL, Zhejiang University							*/
/* Team:			SSL-ZJUNlict										*/
/* HomePage: http://www.nlict.zju.edu.cn/ssl/WelcomePage.html			*/
/************************************************************************/
/* File:	BasicPlay.h													*/
/* Brief:	C++ Interface: Base class for play 							*/
/* Func:	Provide an base class for stated play						*/
/* Author:	cliffyin, 2012, 08											*/
/* Refer:	NONE														*/
/* E-mail:	cliffyin007@gmail.com										*/
/* Version:	0.0.1														*/
/* Changelog:	2012-08-17	note the code								*/
/************************************************************************/

#ifndef _BASIC_PLAY_H_
#define _BASIC_PLAY_H_

//////////////////////////////////////////////////////////////////////////
// forward declaration
class COptionModule;
class CVisionModule;
class CPlayerTask;

/**
@brief		多车进行配合的基类。
@details	需要实现
*/
class CBasicPlay {
public:
	/**
	@brief	构造函数
	*/
	CBasicPlay() { _score = 0; _done = true; }

	/**
	@brief	执行策略，需要被实现
	@param	pOption	当前比赛运行模式
	@param	pVision	当前比赛视觉模块
	*/
	virtual void executePlay(const COptionModule* pOption, const CVisionModule* pVision) = 0;

	/**
	@brief	切换策略，需要被实现
	@param	pOption	当前比赛运行模式
	@param	pVision	当前比赛视觉模块
	*/
	virtual CBasicPlay* transmitPlay(const COptionModule* pOption, const CVisionModule* pVision) = 0;
	
	/**
	@brief	获取策略名
	*/
	virtual const char* playName() const = 0;

	/**
	@brief	策略是否使用
	*/
	virtual bool applicable() const { return false; }
	
	/**
	@brief	策略是否完成
	*/
	virtual bool done() const { return _done; }

	/**
	@brief	策略分值
	*/
	double score() const { return _score; }
	
	/**
	@brief	策略刷新
	*/
	virtual void refresh() { _done = false; }
	
	/**
	@brief	策略退出
	*/
	virtual void quit() {}

protected:
	/**
	@brief	给小车赋任务
	@param	num 执行小车号
	@param	pTask 当前执行的动作
	@param	priority 任务优先级
	*/
	void setTask(const int num, CPlayerTask* pTask, const int priority);

	/**
	@brief	策略当前分值
	*/
	double _score;

	/**
	@brief	策略是否完成
	*/
	bool _done;
};

#endif // ~_BASIC_PLAY_H_