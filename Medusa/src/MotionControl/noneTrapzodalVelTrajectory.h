//////////////////////////////////////////////////////////////////////
// @file:	algorithm\noneTrapzodalVelTrajectory.h
// @brief:  非零速到点轨迹规划算法
//          基于运动学分析的运动控制方法。其算法的核心思想如下：
//         该方法假设机器人在加速和减速过程中，机器人的加速度是恒定不变的。
//     在这个假设的基础上，根据机器人的初始状态, 将机器人的运动分为三
//     大类：
//        1: 初速度与位移方向不一致，先将减速到 0
//        2: 初速度与位移方向一致
//           2.1: 初速度小于最大速度 Vmax, 且加速至最大速度时位移不会
//         超过目标点，则先加速至最大速度。
//           2.2: 保持最大速度运动
//           2.3: 以最大减速度减速，也会超过目标点，则以最大减速减速
//        3: 初速大于最大速度，先减速到 Vmax
//        
//      参考文献:
//           Trajectory generation and control for four wheeled
//           omnidirectional vehicles.

///           Oliver Purwin, Raffaello D'Andrea. Robotics and
//           Autonomous Systems 54 (2006) 13–22 
// summary:	Declares the nonetrapzodal vel trajectory class
/////////////////////////////////////////////////////////////////////

#ifndef NONE_TRAPEZOIDAL_VEL_TRAJECTORY_H
#define NONE_TRAPEZOIDAL_VEL_TRAJECTORY_H
#include <WorldDefine.h>
#include <vector>


using namespace std;  

vector<vector<double>> genTrajZero(vector<vector<double>> &DataVector, 
                                   double zfU, 
                                   double vz0, 
                                   int rev, 
                                   double uza, 
                                   double uzd, 
                                   double vzMax,
                                   double tol_pos,
                                   double tol_vel);

vector<vector<double>> genTrajNone(vector<vector<double>> &DataVector, 
                                   double zfU,
                                   double vz0, 
                                   double vzf,
                                   int rev, 
                                   double uza, 
                                   double uzd, 
                                   double vzMax,
                                   double tol_pos,
                                   double tol_vel);

vector<vector<double>> tg_recurs(double t0, 
								 double z0, 
								 double zf,
								 double vz0, 
								 double vzf,
								 int rev, 
								 int iter,
								 double uza, 
								 double uzd, 
								 double vzMax);

void buildPathNone(const vector< vector<double> > & storeData,
			   double& z,
			   double& vz,
			   double& azList,
			   vector< vector<double> >& pathList);

void buildPathNone(const vector<vector<double>> & storeData,
			   double& z, 
			   double& vz, 
			   double& azList);

double syncTGNone(const PlayerPoseT& start,
			  CGeoPoint finalPos,
			  const PlayerCapabilityT& capability);



void nonetrapezoidalVelocityPath( const PlayerPoseT& start,
							 const PlayerPoseT& final,
							 double frameRate,
							 const PlayerCapabilityT& capability,
							 PlayerPoseT& nextStep,
							 vector< vector<double> >& pathList
							 );
#endif
