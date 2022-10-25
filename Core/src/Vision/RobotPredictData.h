#ifndef _ROBOT_PREDICT_DATA_H_
#define _ROBOT_PREDICT_DATA_H_
#include <geometry.h>
#include "WorldDefine.h"
#include <iostream>
#include <stdio.h>
#include <cmath>
// 视觉信息
struct RobotRawVisionData{
	RobotRawVisionData():cycle(0),x(0),y(0),dir(0),xVel(0),yVel(0),rotVel(0){}
	int cycle;
	double x; // x 坐标
	double y; // y 坐标
	double dir; // 身体朝向
	double xVel; // x 方向速度
	double yVel; // y 方向速度
	double rotVel; // 旋转速度
};
// 记录机器人的原始视觉信息
class CRobotRawVisionLogger{
public:
	bool visionValid(int cycle) const { return _vision[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS].cycle == cycle; }
	const RobotRawVisionData& getVision(int cycle) const { return _vision[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS]; }
	void setVision(int cycle, double x, double y, double dir, double xVel, double yVel, double rotVel)
	{
		_vision[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS].cycle = cycle;
		_vision[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS].x = x;
		_vision[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS].y = y;
		_vision[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS].dir = dir;
		_vision[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS].xVel = xVel;
		_vision[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS].yVel = yVel;
		_vision[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS].rotVel = rotVel;
	}
private:
	static const int MAX_INFORMATIONS = 16; // 保存n个周期的信息
	RobotRawVisionData _vision[MAX_INFORMATIONS];
};
// 命令
struct RobotCommandEffect{
    RobotCommandEffect():cycle(0),rot_speed(0){}
	int cycle; // 所在的周期，用来验证是否有效
	CVector vel; // 平移速度
	double rot_speed; // 旋转速度
};
// 记录机器人的命令
class CRobotCommandLogger{
public:
	bool commandValid(int cycle) const { return _command[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS].cycle == cycle; }
	const RobotCommandEffect& getCommand(int cycle) const { return _command[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS];}
	void setCommand(int cycle, const CVector& vel, double rot_speed)
	{
		_command[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS].cycle = cycle;

        if ( std::isfinite(vel.mod()) && std::isfinite(rot_speed) ){
			_command[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS].vel = vel;
			_command[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS].rot_speed = rot_speed;
		}
		else{
			printf("%.2f , %.2f, %.2f command error!\n", vel.x(), vel.y(), rot_speed);
			_command[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS].vel = CVector();
			_command[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS].rot_speed = 0.0;
		}
	}
	void print(int cycle){
        std::cout<<" cycle: "<<cycle<<std::endl;
        for ( int i = 0; i < MAX_INFORMATIONS; i++) {
		std::cout<<"i:"<<i<<" RobotComandLogger: cycle = "<< _command[(cycle-i+MAX_INFORMATIONS) % MAX_INFORMATIONS].cycle << " velx: "<<_command[i].vel.x()
			<<" vely: "<<_command[i].vel.y()<<" rotvel: "<<_command[i].rot_speed<<std::endl;
		}
	}
private:
	static const int MAX_INFORMATIONS = 16; // 保存n个周期的信息
	RobotCommandEffect _command[MAX_INFORMATIONS];
};
// 记录预测过的视觉信息
struct RobotVisionData : public PlayerVisionT{
    RobotVisionData():cycle(0){}
	int cycle;
};
class CRobotVisionLogger{
public:
    CRobotVisionLogger(){}
	bool visionValid(int cycle) const { return _vision[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS].cycle == cycle; }
	const RobotVisionData& getVision(int cycle) const { return _vision[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS]; }
	RobotVisionData& getVision(int cycle) { return _vision[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS]; }
	void setVision(int cycle, const RobotVisionData& vision, int realNum)
	{
		_vision[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS] = vision;
		_vision[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS].cycle = cycle;
	}
private:
	static const int MAX_INFORMATIONS = 16; // 保存n个周期的信息
	RobotVisionData _vision[MAX_INFORMATIONS];
};
// 记录统计信息
struct RobotVisionErrorData{
    RobotVisionErrorData():cycle(0),distanceTravelled(0),rotationTravelled(0),predictedRotationalError(0),predictedTranslationalError(0),normalizedTranslationalError(0),normalizedRotationalError(0),resetTranslationalVelocity(0),resetRotationalVelocity(0){}
	int cycle; // 所在的周期
	double distanceTravelled; // 走的距离
	double rotationTravelled; // 转的角度
	double predictedTranslationalError; // 平动误差
	double predictedRotationalError; // 转动误差
	double normalizedTranslationalError;
	double normalizedRotationalError;
	bool resetTranslationalVelocity;
	bool resetRotationalVelocity;
};
class CRobotVisionErrorLogger{
public:
	bool errorValid(int cycle) const { return _error[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS].cycle == cycle; }
	const RobotVisionErrorData& getError(int cycle) const { return _error[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS];}
	RobotVisionErrorData& getError(int cycle) { return _error[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS];}
	void setError(int cycle, const RobotVisionErrorData& error)
	{
		_error[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS] = error;
		_error[(cycle+MAX_INFORMATIONS) % MAX_INFORMATIONS].cycle = cycle;
	}
private:
	static const int MAX_INFORMATIONS = 16; // 保存n个周期的信息
	RobotVisionErrorData _error[MAX_INFORMATIONS];
};
#endif // _ROBOT_PREDICT_DATA_H_
