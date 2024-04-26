#ifndef _WORLD_MODEL_H_
#define _WORLD_MODEL_H_

#include "OptionModule.h"
#include "VisionModule.h"
#include "KickStatus.h"
#include "BufferCounter.h"
#include "BallStatus.h"
#include <singleton.hpp>
extern bool record_run_pos_on;
///
/// @file   WoldModel.h
/// @author Penghui Yin <cliffyin@zju.edu.cn>
/// @date   Sun Oct 22 15:26:36 2010
/// 
/// @brief  对于视觉模块的进一步处理，提供视觉的高层信息，协同视觉模块
///			进一步维护世界模型，其中包含以下几方面的内容：
///			1.基于基本数据的带参条件真值获取
///				见WorldModel_con.cpp，函数接口对外开放，方便条件获取
///         2.视觉模块数据预处理，组合计算得到一些基本数据
///				见WorldModel_basic.cpp，函数接口对外开放，方便调用
///			3.基于视觉信息的预测计算
///				见WorldModel_pre.cpp，函数接口对外开放，方便数据获取
///			4.基于视觉信息的各种高层算法，生成一些最优数据
///				见WorldModel_alg.cpp，函数接口对外开放，方便数据获取
///			5.基于视觉信息的工具函数
///				见WorldModel_utils.cpp，函数接口对外开放，方便数据获取


/************************************************************************/
/*							WorldModel.h                                */
/************************************************************************/

class CWorldModel{
public:
	CWorldModel() : _pVision(0), _pOption(0){ }
	void registerVision(const CVisionModule* pVision) { _pVision = pVision; }
	void registerOption(const COptionModule* pOption) { _pOption = pOption; }
	const CVisionModule* vision() const { return _pVision; }
	const COptionModule* option() const { return _pOption; }
    bool	IsBallKicked(int);
    bool    IsInfraredOn(int);
    int     InfraredOnCount(int);
    int     InfraredOffCount(int);
    bool	KickDirArrived (int current_cycle, double kickdir, double kickdirprecision, int myNum = myDefaultNum);

private:
	const CVisionModule* _pVision;
	const COptionModule* _pOption;
	PlayerVisionT me;
	PlayerVisionT he;
	ObjectPoseT ball;


/************************************************** WorldModel_con ***************************************************/
/// To be continue...
public:
/************************************************** WorldModel_basic ***************************************************/
/*
	1.球
	2.我方小车
	3.对方小车
	4.双方小车
	5.静态环境
	6. ... ...
*/
private:
	// 默认我方小车号
	const static int myDefaultNum;
	// 默认对方小车号
    const static int enemyDefaultNum;
public:
    void SPlayFSMSwitchClearAll(bool clear_flag=false);
	// 自己到球的矢量
    const CVector	self2ball					(int current_cycle, int myNum = myDefaultNum);
	// 自己到球的距离
    double	self2ballDist				(int current_cycle, int myNum = myDefaultNum, int enemyNum = enemyDefaultNum);
	// 自己到球的角度
    double	self2ballDir				(int current_cycle, int myNum = myDefaultNum, int enemyNum = enemyDefaultNum);
	
/************************************************** WorldModel_utils ***************************************************/
/// To be continue...
    const std::string	CurrentRefereeMsg();

public:
};

typedef Singleton< CWorldModel > WorldModel;

#define WORLD_MODEL WorldModel::Instance()

#endif // _WORLD_MODEL_H_
