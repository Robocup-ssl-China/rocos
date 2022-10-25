#ifndef _DYNAMICS_SAFETY_SEARCH_H_
#define _DYNAMICS_SAFETY_SEARCH_H_
#include <VisionModule.h>
#include <GDebugEngine.h>
#include <RobotCapability.h>
#include "cmu/vector.h"
#include <math.h>
#include <utils.h>
#include <geometry.h>
#include <singleton.h>
#include "QuadraticEquation.h"
#include "CubicEquation.h"
#include "QuarticEquation.h"
/************************************************************************/
/* 轨迹类：储存小车的运行轨迹，并且判断与其它轨迹之间是否碰撞				 
/************************************************************************/
class Trajectory {
public: 
	Trajectory(){_qlength = 0; _limitTime = EPSILON;}
	~Trajectory(){}
	void MakeTrajectory(const int player,const vector2f pos,const vector2f vel, const vector2f acc,
						const float t0,const float tc,const float tend,
						const float Radius,const float DMAX, const int type);
	bool CheckTrajectory(const Trajectory& T1);
	CQuadraticEquation q[3]; // 二次方程式的实例
	int _qlength;
	int _type; //小车的类型
	int _carnum;
	float Radius;
	float _limitTime;
	float _tend;
};
/************************************************************************/
/* 动态安全搜索类，检查当前加速下，小车是否会与障碍物相撞，如果相撞则随机获
   一个较优的安全加速度
/************************************************************************/
class CDynamicSafetySearch {
public:
	CDynamicSafetySearch();
	~CDynamicSafetySearch() {}
	CVector SafetySearch(const int player,CVector Vnext, const CVisionModule* pVision, 
						 const int t_priority, const CGeoPoint target, 
						 const int flags,const float stoptime, double max_acc);
private:
	bool CheckAccel(const int player, vector2f acc,const CVisionModule* pVision, 
					vector2f& crash_car_dir, int& crash_car_num, int& type,const float limitTime);
	float Evaluation(const vector2f tempacc, const int player);
	bool CheckRobot(const int player, vector2f pi, vector2f vi,vector2f Ai, 
					const int obstacle, vector2f pj, vector2f vj, vector2f Aj,int type,const float limitTime);
	vector2f RandomAcc(float start_angle,float end_angle,float max_radius); //在加速度空间中随机寻找加速度
	vector2f OutOfTrouble(const int player, vector2f pi, vector2f dir,float dist,const CVisionModule* pVision); //从死区跳出
	vector2f CVector2vector2f(const CVector vec1);
	vector2f CGeoPoint2vector2f(const CGeoPoint pos);
	CVector vector2f2CVector(const vector2f vec1);
	CGeoPoint vector2f2CGeoPoint(const vector2f pos1);
	void refresh_priority(const int player,const int t_priority, const vector2f target, const vector2f pos);

	float _e;             // 权值
	float _gamma;         // 小车的行驶时间
	vector2f _pos;        // 小车当前的位置
	vector2f _vel;        // 小车当前的速度
	vector2f _nvel;       // 小车下一时刻的速度
	vector2f _acc;        // 小车的加速度
	vector2f _t_acc;      // 临时加速度
	vector2f last_acc[PARAM::Field::MAX_PLAYER]; // 六辆小车
	float last_e[PARAM::Field::MAX_PLAYER];      // 储存上次的最优的评价
	int priority[PARAM::Field::MAX_PLAYER];      // 判断小车避障的优先集
	bool find_flag[PARAM::Field::MAX_PLAYER];    // 是否随机找解
	float _stoptime;
	float _DECMAX;          // 小车最大的减速度
	float _AECMAX;          // 小车最大的加速度
	float _VAMX;          // 小车最大的速度
	float _C;             // 程序执行一个周期的时间
	int _CNum;            // 预测多少个周期后的事情
	int _flag;
};
typedef NormalSingleton< CDynamicSafetySearch > DynamicSafetySearch;
#endif 
