/************************************************************************/
/*							各种各样的类型定义                          */
/************************************************************************/

#ifndef _MISC_TYPES_H_
#define _MISC_TYPES_H_

#include <geometry.h>
#include <vector>

/// 底层跑点走位控制方法
enum CTRL_METHOD{	
	CMU_TRAJ	= 1,						// CMU			非零速到点
	ZERO_FINAL	= 2,						// Cornell		零速度到点
	ZERO_TRAP	= 3,						// ZJUNlict		零速度到点
	NONE_TRAP	= 4,						// ZJUNLict		非零速到点
};

/// 球员状态任务结构体
struct PlayerStatus{
	/// 构造函数 ： 任务参数默认初始化
	PlayerStatus(): flag(0),
					pos(0.0,0.0),angle(0.0),vel(0.0,0.0),rotvel(0.0),
                    max_acceleration(0.0),max_deceleration(0.0),max_rot_acceleration(0.0),max_speed(0.0),max_rot_speed(0.0),rotdir(0),
                    is_specify_ctrl_method(false),specified_ctrl_method(ZERO_FINAL),
					needkick(true),ispass(false),ischipkick(false),kickprecision(0.0),kickpower(0.0),chipkickpower(0.0),
                    speed_x(0.0),speed_y(0.0),rotate_speed(0.0),path_plan_in_circle(false),path_plan_circle_center(CGeoPoint(1e8, 1e8)),path_plan_circle_radius(1e8),
                    specify_path_plan_area(false){ }

	/// 标签
	int flag;														// 任务标签
    int kick_flag;

	/// 跑位参数 ： 用于底层运动控制 , 默认使用
	CGeoPoint pos;													// 全局目标跑位点
	double angle;													// 全局目标到点朝向
	CVector vel;													// 全局目标到点平动速度	
	double rotvel;													// 全局目标到点转动速度
	int rotdir;														// 旋转的方向
	bool needReport;												// 是否需要返回信息
	/// 运动参数 ： 用于底层运动控制 ，指定标签辅助
	double max_acceleration;										// 最大加速度
	double max_deceleration;										// 最大减速度
    double max_rot_acceleration;
    double max_speed;
    double max_rot_speed;
	bool force_manual_set_running_param = false;					// 是否强制手动设置运动参数

	/// 控制方法 ： 用于底层运动控制 ，指定控制方法
	bool is_specify_ctrl_method;									// 是否指定运动控制方法
	CTRL_METHOD specified_ctrl_method;								// 指定的运动控制方法
	
	/// 踢球参数 ： 用于平射挑射控制 ，默认使用
	bool needkick;													// 踢球动作执行开关
	bool ispass;													// 是否进行传球
	bool ischipkick;												// 挑球还是平射
	bool isDirectNoCalibrationKick;									// 是否是直接下发踢球时间参数给硬件（一般用于标定）
	double directNoCalibrationKickPower;							// 直接下发踢球时间参数给硬件的时间
	double kickprecision;											// 踢球朝向精度
	double kickpower;												// 踢球力度
	double chipkickpower;											// 挑球力度	

	/// 速度参数 ： 用于直接速度控制 ，指定标签辅助 
	double speed_x;													// 全局x方向平动速度
	double speed_y;													// 全局y方向平动速度
	double rotate_speed;											// 转动速度
	
    ///规划参数 ： 用于将路径规划限制在一个圆内
    bool path_plan_in_circle;
    CGeoPoint path_plan_circle_center;
    double path_plan_circle_radius;

    bool specify_path_plan_area;

	/// 标签常量
	// 默认的flags
	static const int NOTHING						= 0x00000000; // 没有flag
	// 行走的flags
//	static const int SLOWLY							= 0x00000001; // 慢速
//	static const int QUICKLY						= 0x00000002; // 快速（（躲避碰撞，守门员扑球，截球）
//	static const int DO_NOT_STOP					= 0x00000008; // 用最快速度行走 --xxx---
	static const int DODGE_BALL						= 0x00000010; // 躲避球
//	static const int POS_ONLY						= 0x00000020; // 只考虑位置,不考虑角度
	static const int AVOID_SHOOTLINE				= 0x00000040; // 躲避射门线
//	static const int NOT_DRIBBLE					= 0x00000080; // 在Factory中强制不开吸球
	static const int DRIBBLING						= 0x00000100; // 走的过程中带球
	static const int DODGE_OUR_DEFENSE_BOX			= 0x00000200; // 不要进入己方禁区
	static const int NOT_AVOID_THEIR_VEHICLE		= 0x00000400; // 不躲避对方车
//    static const int ACCURATELY					= 0x00000800; // 精确行走，不考虑是否已经到达
	static const int TURN_AROUND_FRONT				= 0x00001000; // 以车体正前方为中心旋转 --xxx---
//    static const int DODGE_REFEREE_AREA			= 0x00002000; // 躲避开球50cm区域
//    static const int SPECIFY_SPEED				= 0x00004000; // 指定速度
//    static const int SPECIFY_ROTATE_SPEED			= 0x00008000; // 指定旋转速度
//    static const int SPECIFY_ACCELERATION			= 0x00010000; // 指定加速度
//    static const int SPECIFY_ROTATE_ACCELERATION	= 0x00020000; // 指定旋转加速度
    static const int BREAK_THROUGH					= 0x00040000; // 带球过车的朝向flag
	static const int NOT_AVOID_OUR_VEHICLE			= 0x00080000; // 不躲避我方车
    static const int AVOID_STOP_BALL_CIRCLE         = 0x00100000; // 躲避stop时球周围的环形区域
    static const int AVOID_THEIR_BALLPLACEMENT_AREA = 0x00200000; // 躲避their ballplacement的环形区域
    static const int AVOID_HALF_FIELD               = 0x00400000; // kickoff不能越过半场

    //kick flag
    static const int KICK                           = 0x00000001;
    static const int CHIP                           = 0x00000002;
    static const int DRIBBLE                        = 0x00000004;
    static const int SAFE                           = 0x00000008;
	// 处理球的flags
//	static const int SPECIFY_DIRECTION				= 0x01000000; // 指定身体朝向
    static const int MIN_DSS                        = 0x02000000; // 忽略挡住目标点的队员
    static const int NOT_AVOID_PENALTY				= 0x04000000; // 自动放球 added by Wayne in 2018/6/6
	static const int ALLOW_DSS						= 0x08000000; // DSS的开启开关
    static const int FORCE_KICK         			= 0x80000000; // 强制开踢球
    static const int FREE_KICK      				= 0x20000000; // 面向某个点

};

/// 球状态结构
struct stBallStatus{
	stBallStatus() : receiver(0){ }
	CGeoPoint pos;				// 出球的目标点
	int receiver;				// 接球的队员号码
	int Sender;                 // 出球者号码（added by shizhy)
	double angle;
	bool front;
	double avoid_dist = -1;
};

/// 任务结构
struct TaskT{
	TaskT() : executor(0){ }
	TaskT(const int executor) : executor(executor){ }
	int executor;				// 执行任务的队员号码
	PlayerStatus player;		// 队员的状态
	stBallStatus ball;			// 球的状态
};

#endif	// ～_MISC_TYPES_H_
