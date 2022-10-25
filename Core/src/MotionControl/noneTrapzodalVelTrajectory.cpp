#include <iostream>
#include <math.h>
#include <utils.h>
#include "staticparams.h"
#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include "noneTrapzodalVelTrajectory.h"


//#define _PRINT_DEBUG_MSG
#ifdef _PRINT_DEBUG_MSG
#define PRINT_DEBUG(x) std::cout << #x << " : " << (x) << std::endl;
#define PRINT_MSG(x) std::cout << x << std::endl;
#else 
#define PRINT_DEBUG(x)
#define PRINT_MSG(x)
#endif
namespace{
	bool SHOW_TG_TIME = true;
	
}
//static CVector VEL_LOG[2] = {CVector(-9999,-9999), CVector(-9999, -9999)};
/////////////////////////////////////////////////////////////////////
/// @fn	vector<vector<double>> genTrajZero
///
/// @brief 零速到点规划。基本算法
///       
///     
/// 
/// @author	Yonghai Wu
/// @date	2009-12-9
///
/// @param	t0		initial time = absolute time when this stage begins
/// @param	z0		initial position 
/// @param	zf		final position 
/// @param	vz0		initial velocity. 
/// @param	rev		The reverse. 
/// @param	iter	The iterator. 
/// @param	uza		the acceleration. 
/// @param	uzd		The decelerate. 
/// @param	vzMax	The vz maximum. 
///
/// @return	返回规划得到的路径，路径由多段组成，每段包括以下参数：
/// 
///         z0U: 初始位移
///        vz0U: 下一阶段的初速度
///          az: 这一阶段的加速度
///         rev: 是否改变参考方向
///           t: 这一阶段所用时间
///         t0U: 已用时间（不包括这一阶段）
///
///////////////////////////////////////////////////////////////////////////////////////

vector<vector<double>> genTrajZero(vector<vector<double>> &DataVector,  
                                   double zfU, 
                                   double vz0, 
                                   int rev, 
                                   double uza,
                                   double uzd,
                                   double vzMax,
                                   double tol_pos,
                                   double tol_vel)
{
	//std::cout<<"TrapezoidalVelTrajectory"<<std::endl;
//	double tol = 0.01;   // tolerance for machine precision
	double z0U = 0;
	double vz0U = vz0;
	double az;
	double t=0;
	double t0U = 0;
    double pos_factor = 1.2;
    double vel_factor = 1.2;
    

    
	int numOfDataVector = DataVector.size() ;
	if (  numOfDataVector > 0 )
	{
		t0U = DataVector[numOfDataVector-1][5];
	}
    double zDelta = 0;
    
	while ( (abs (zfU ) >= pos_factor*tol_pos) || ( abs( vz0U ) >= vel_factor*tol_vel ) )
	{
		//std::cout<<"zfU:"<<zfU<<"vz0U:"<<vz0U<<std::endl;
      vz0 = vz0U;
      if ( zfU < 0 ){
        //Destination in wrong direction, need to flip the sign
        PRINT_MSG("Flip direction");
        z0U = 0;
        zfU = -zfU;
        vz0U = -vz0U;
        az = 0;
        t = 0;
        zDelta = 0;
            
        if (rev==1)
          rev = -1;
        else
          rev = 1;
      }
		else if ( vz0U > vzMax ){
          //Case 3: Need to decelerate until vz = vzMax
          PRINT_MSG("Case 3");
          t = ( vz0U - vzMax ) / uzd;
          z0U = 0;
          zDelta = ( vz0U*vz0U - vzMax*vzMax ) / 2.0 / uzd;
          zfU = zfU - zDelta;
          vz0U = vzMax;
          az = -uzd;
		}
		else if ( vz0U < 0 ){
			// 需要反向，先把原来的速度减到零
			//Case 1: Moving in wrong direction, need to decelerate first
			PRINT_MSG("Case 1");
			t = abs( vz0U / uzd );
			zDelta = vz0U*t + uzd*t*t/2.0;
			z0U = 0;
			zfU = zfU-zDelta;
			vz0U = 0; // vz0 + uzd*t; // "correct" solution isn't great, rounding errors
			az = uzd;
		}    
		else if (zfU <= ( vz0U*vz0U / 2.0/ uzd + tol_pos )){
			// 从现在起以最大减速度减速，也会超过目标点
			// Case 2.3: Need to decelerate
			PRINT_MSG("Case 2.3");
			t = vz0U / uzd;
			zDelta = vz0U*vz0U/2.0/uzd;
			z0U = 0;
			zfU = zfU-zDelta;
			vz0U = 0;
			az = -uzd;
		}
		else if ( fabs(vz0U - vzMax) < tol_vel ){
			// 计算保持最大速度的时间，这段时间运动的距离，及到减速时剩下的路程
			//Case 2.2: Cruising at vzMax until you have to decelerate
			PRINT_MSG("Case 2.2");
			z0U = 0;
			zDelta = vzMax*vzMax/2.0/uzd;		
			vz0U = vzMax;
			az = 0;
			double zCruise = zfU - zDelta;
			t = zCruise / vzMax;
			zfU = vzMax*vzMax/2.0/uzd;
		}    
		else{
			// Case 2.1: Accelerate
			PRINT_MSG("Case 2.1");
			// Accelerate to vzMax
			double t1 = (vzMax - vz0U)/uza;
			
			// Accelerate without reaching vzMax
            zDelta = vz0U*vz0U/2.0/uza;
			float temp = (float)( (zfU + zDelta)/(1.0/2/uza + 1.0/2/uzd) );
			double vz2 = sqrt( temp );
			double t2 = (vz2-vz0U)/uza;
			if (t1<t2){
				t = t1;
				z0U = 0;
				zfU = zfU - vz0U*t1 - uza/2.0*t1*t1;
				vz0U = vzMax;
				az = uza;
			}
			else{
				t = t2;
				z0U = 0;
				zfU = zfU- vz0U*t2 - uza/2.0*t2*t2;
				vz0U = vz2;
				az = uza;
			}
		}
		// zfU: 剩余路程
		// vz0: 初速度
		// vz0U: 下一阶段的初速度
		// az: 这一阶段的加速度
		// rev: 是否改变参考方向
		// t: 这一阶段所用时间
		// t0U: 已用时间（不包括这一阶段）
		double data[6] = {z0U, vz0, az, rev, t, t0U};
		vector<double> storeData(data,data+6);
		DataVector.push_back(storeData);
		t0U = t + t0U;   //Initial time for the next stage

		if ( vz0U == vzMax && vzMax < 0 )
			break;
	}
	
	double data[6] = {0, 0, 0, rev, 0, t0U};
	vector<double> storeData(data,data+6);
	DataVector.push_back(storeData);
	return DataVector;
}

///////////////////////////////////////////////////////////////////////////////////////
/// @fn	
///
/// @brief	非零速到点规划。
///
/// @author	Yonghai Wu
/// @date	2009-12-9
///
/// @param	t0		initial time = absolute time when this stage begins
/// @param	z0		initial position 
/// @param	zf		final position 
/// @param	vz0		initial velocity.
/// @param  vzf     final velocity. 
/// @param	rev		The reverse. 
/// @param	iter	The iterator. 
/// @param	uza		the acceleration. 
/// @param	uzd		The decelerate. 
/// @param	vzMax	The vz maximum. 
///
/// @return	返回规划得到的路径，路径由多段组成，每段包括以下参数：
/// 
////////////////////////////////////////////////////////////////////////////////
vector< vector<double> > genTrajNone(vector<vector<double>> &DataVector,
                                     double zfU,
                                     double vz0,
                                     double vzf,
                                     int rev,
                                     double uza,
                                     double uzd,
                                     double vzMax,
                                     double tol_pos,
                                     double tol_vel)
{
	// zfU: 剩余路程
	// vz0: 初速度
	// vz0U: 下一阶段的初速度
	// az: 这一阶段的加速度
	// rev: 是否改变参考方向
	// t: 这一阶段所用时间
	// t0U: 已用时间（不包括这一阶段）
	const double tol = 0.01;   // tolerance for machine precision
	double z0U = 0;
	double vz0U = vz0;
	double az;
	double t=0;
	double t0U = 0;
	int numOfDataVector = DataVector.size() ;
	if (  numOfDataVector > 0 )
	{
		t0U = DataVector[numOfDataVector-1][5];
	}

	int numOfStep = 0;  // 
//	bool canReach = true;
    double zDelta = 0;
   
	/// check if the robot can reach the target

	while ( ((fabs(zfU)>= 1.2*tol_pos) || (fabs(vz0U-vzf)>= 1.2*tol_pos)) && (numOfStep < 5) )
	{
      vz0 = vz0U;
      if ( numOfStep > 4 )
        std::cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~zfU:"<<zfU<<"vz0U:"<<vz0U<<std::endl;
      if ( zfU < 0 ){
        PRINT_MSG("Flip direction");
        z0U = 0;
        zfU = -zfU;
        vz0U = -vz0U; 
        vzf = -vzf;
        az = 0;
        t = 0;
        zDelta = 0;
        rev = rev == 1 ? -1:1;
      }
      else if ( vz0U  > vzMax ){
        // 需要反向，先把原来的速度减到零
        PRINT_MSG("Case 3");
        t = (vz0U-vzMax)/uzd;
        z0U = 0;
        zDelta = (vz0U*vz0U-vzMax*vzMax)/2.0/uzd;
        zfU = zfU - zDelta;
        vz0U = vzMax;
        az = -uzd;
      }
      else if ( vz0U < 0 ){
        // 需要反向，先把原来的速度减到零
        PRINT_MSG("Case 1");
        t = fabs(vz0U/uzd);
        zDelta = vz0U*t + uzd*t*t/2.0;
        z0U = 0;
        zfU = zfU-zDelta;
        vz0U = 0; // vz0 + uzd*t; // "correct" solution isn't great, rounding errors
        az = uzd;
      }    
      else if ( vz0U > vzf && zfU <= ((vz0U*vz0U - vzf*vzf)/2.0/uzd + tol_pos) ){
        // 从现在起以最大减速度减速，也会超过目标点
        // Case 2.3: Need to decelerate
        PRINT_MSG("Case 2.3");
        zDelta = (vz0U*vz0U - vzf*vzf)/2.0/uzd;
        t = (vz0U-vzf)/uzd;
        zfU = zfU-zDelta;
        vz0U = vzf;
        z0U = 0;
        az = -uzd;
      }
      else if ( fabs(vz0U - vzMax) < tol_vel ){
        // 计算保持最大速度的时间，这段时间运动的距离，及到减速时剩下的路程
        //Case 2.2: Cruising at vzMax until you have to decelerate
        PRINT_MSG("Case 2.2");
        z0U = 0;
        zDelta = (vzMax*vzMax - vzf*vzf)/2.0/uzd;		
        vz0U = vzMax;
        az = 0;
        double zCruise = zfU - zDelta;
        t = zCruise / vzMax;
        zfU = (vzMax*vzMax - vzf*vzf)/2.0/uzd;
      }    
      else{
        // Case 2.1: Accelerate
        PRINT_MSG("Case 2.1");
        // Accelerate to vzMax
        double t1 = (vzMax - vz0U)/uza;
        
        // Accelerate without reaching vzMax
        zDelta = vz0U*vz0U/2.0/uza + vzf*vzf/2.0/uzd;
		float temp = (float)( (zfU + zDelta)/(1.0/2/uza + 1.0/2/uzd) );
		double vz2 = sqrt( temp );
        double t2 = (vz2 - vz0U)/uza;
        if (t1<t2){
          t = t1;
          z0U = 0;
          zfU = zfU - vz0U*t1 - uza/2.0*t1*t1;
          vz0U = vzMax;
          az = uza;
        }
        else{
          t = t2;
          z0U = 0;
          zfU = zfU- vz0U*t2 - uza/2.0*t2*t2;
          vz0U = vz2;
          az = uza;
        }
      }
      
      numOfStep++;
      //PRINT_DEBUG( numOfStep );
		
      double data[6] = {z0U, vz0, az, rev, t, t0U};
      vector<double> storeData(data,data+6);
      DataVector.push_back(storeData);
      t0U = t + t0U;   //Initial time for the next stage

      if ( vz0U == vzMax && vzMax < 0 )
        break;
    }
    //std::cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~numOfStep "<<numOfStep<<endl;
    double data[6] = {0, 0, 0, rev, 0, t0U};
    vector<double> storeData(data,data+6);
    DataVector.push_back(storeData);
    return DataVector;
}

vector< vector<double> > tg_recurs(double t0,
								   double z0,
								   double zf,
								   double vz0,
								   double vzf,
								   int rev,
								   int iter,
								   double uza,
								   double uzd,
								   double vzMax,
								   double tol_pos,
								   double tol_vel)
{
  	const double tol = 0.01;   // tolerance for machine precision
	//const double tol_pos = 1;	//pos_error	
	//const double tol_vel = 2;	//vel_error
	double zfU = zf;
	double z0U = 0;
	double vz0U = vz0;
	double az;
	double t=0;
	double t0U = 0;
    double zfU2;
    
	vector<vector<double> >  DataVector;
    DataVector.clear();
	iter = iter + 1;
//	int numOfStep = 0;  //
//	bool canReach = true;
//    double zDelta = 0;

    bool isGotoBehindFirst = false;
    
	if (fabs(z0)!=0)
	{
		// Adjust offset for z0
		zfU = zf-z0;
		z0U = 0;
		vz0U = vz0;
		az = 0;
		t = 0;
	}
    /// 以位移的方向为正
    if (zfU<0 )
    {
      // Destination in wrong direction, need to flip the sign
      // display('Flip direction');
      z0U = 0;
      zfU = -zfU;
      vz0U = -vz0U;
      vzf = -vzf;
      az = 0;
      t = 0;
      rev = rev == 1 ? -1 : 1;
        
      double data[6] = { z0U, vz0, az, rev, t, t0U };
      vector<double> storeData(data, data + 6 );
      DataVector.push_back( storeData );
      t0U = t + t0U;   //Initial time for the next stage  
    }
    
    int revold = rev;
    double vf = vzf;
    double zMinDist1, zMinDist2;

    /// 达到末速度的最短距离
    zMinDist1 = (vf*vf)/2/uza;
    /// 末速度大于 0
    if ( vf > 0 ){
      if ( vz0U > 0 )
      { /// 初速度大于 0
        double d =  (vf*vf-vz0U*vz0U);
        /// 末速度与初速度的关系
        /// 如果初速度比末速度小，则 zMinDist2 等于最短加速距离
        /// 如果初速度比末速度大，则 zMinDist2 等于最短减速距离
        zMinDist2 = vf > vz0U ? d/2/uza : -d/2/uzd;
      }
      else{
        /// 初速度小于 0
        zMinDist2 = (vf*vf)/2/uza - (vz0U*vz0U)/2/uzd;
      }
      /// 如果达到末速度的所需的最短加速距离小于 zfU
      /// 机器人需先后退到 zfU - zMinDist1， 然后再加速
      /// 
      if ( zMinDist2 > zfU )
      {
        zfU2 = zfU - zMinDist1;
		genTrajNone(DataVector,
			zfU2,
			vz0U,
			0.0,
			rev,
			uza,
			uzd,
			vzMax,
			tol_pos,
			tol_vel);
        zfU = zMinDist1;
        vz0U = 0.0;   
        rev = revold;
        isGotoBehindFirst = true;
	//	std::cout<<" backup first 1 ~~~~~~~~~~~~~~~"<<endl;
      }
    }
    else{ /// 末速度小于零
      if ( vz0U > 0 )
      { /// 初速大于零
        zMinDist2 = zfU-vz0U*vz0U/2/uzd;
        if (zMinDist2 < -zMinDist1){
          zfU2 = vz0U*vz0U/2/uzd;
          zfU = zMinDist2; 
        }
        else{
          zfU2 = zfU+zMinDist1;
          zfU = -zMinDist1;
        }
      }
      else{ ///初速度小于零
        zMinDist2 = zfU+vz0U*vz0U/2/uzd;
        zfU2 = zfU+zMinDist1;
        zfU = -zMinDist1;
      }
      
	  genTrajNone(DataVector,
		  zfU2,
		  vz0U,
		  0.0,
		  rev,
		  uza,
		  uzd,
		  vzMax,
		  tol_pos,
		  tol_vel);
        vz0U = 0.0;
		
        rev = revold;
        isGotoBehindFirst = true;
        //std::cout<<" backup first 2 ~~~~~~~~~~~~~~~"<<endl;
    }
	
  
			genTrajNone(DataVector,
                              zfU,
                              vz0U,
                              vzf,
                              rev,
                              uza,
                              uzd,
                              vzMax,
                              tol_pos,
                              tol_vel);
  
  return DataVector;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	void buildPath(const vector<vector<double>>& storeData,double& z, double& vz,
/// 	double& azList)
///
/// @brief	根据函数 <tg_recurs> 的结果返回下一周期的位移 z，速度 vz 和加速度 azList。
///           
///         1. 根据算法的基本假设，加速度恒定，因此加速度 a = storeData[stage-1][2];
///         2. 速度 v = v0 + a*dt
///         3. 位移 z= z0 + v*dt
/// 
/// @author	Yonghai Wu
/// @date	2009-12-9
///
/// @param	storeData		Information describing the pathe. 
/// @param [in,out]	z		the next postion. 
/// @param [in,out]	vz		the next velocity. 
/// @param [in,out]	azList	the next acceration. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void buildPathNone(const vector<vector<double>>& storeData,double& z, double& vz, double& azList)
{
	double num_stages = storeData.size();
	vector<double> data = storeData[num_stages-1];
	double Duration = data[5]; // 运动控制总时间
	int stage = 0;
	z = storeData[0][0];
	int tfint = Duration*PARAM::Vision::FRAME_RATE;
	int	maxStepNumber=(std::min)(tfint, 1);
	for (int index = 0;index<=maxStepNumber;index++){
		if (index>=Duration*PARAM::Vision::FRAME_RATE)
			continue;
		while (index>=storeData[stage][5]*PARAM::Vision::FRAME_RATE)
			stage = stage + 1;		
		double t0 = storeData[stage-1][5] ;
		double vz0 = storeData[stage-1][1];
		double az  = storeData[stage-1][2];
		double rev = storeData[stage-1][3];
		azList = rev*az;	
		vz = rev*vz0 + rev*az * (index*1.0/PARAM::Vision::FRAME_RATE-t0);
		if (index!=0)
			z+=vz0/PARAM::Vision::FRAME_RATE;	
	}
	// This is to prevent errors when duration = 0
	if (tfint == 0){
		//std::cout<<"duration = 0!!!!!"<<endl;
		z      = storeData[0][0];
		vz     = storeData[0][1];
		azList = storeData[0][2];
		//std::cout<<"tfint = 0 "<<z<<" "<<vz<<" "<<azList<<endl;
	}	
	//std::cout<<"stage:"<<stage<<" "<<z<<" "<<vz<<" "<<vz/PARAM::Vision::FRAME_RATE<<endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	void buildPath(const vector< vector<double> >& storeData, double& z, double& vz,
/// 	double& azList, vector< vector<double> >& pathList)
///
///    根据函数 <tg_recurs> 的结果返回下一周期的位移 z，速度 vz 和加速度 azList 和整条路径 pathList。
///           
///         1. 根据算法的基本假设，加速度恒定，因此加速度 a = storeData[stage-1][2];
///         2. 速度 v(t) = v0 + a*dt
///         3. 位移 z(t) = z0 + v(t)*dt
///
/// @author	Yonghai Wu
/// @date	2009-12-9
///
/// @param	storeData			Information describing the store. 
/// @param [in,out]	z			the z coordinate. 
/// @param [in,out]	vz			the vz. 
/// @param [in,out]	azList		list of azs. 
/// @param [in,out]	pathList	list of pathes. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void buildPathNone(const vector< vector<double> >& storeData,
			   double& z,
			   double& vz,
			   double& azList,
			   vector< vector<double> >& pathList)
{
	double num_stages = storeData.size();
	vector<double> data = storeData[num_stages-1];
	double Duration = data[5]; // 运动控制总时间

	if ( abs(Duration) < 0.00001 )
	{
		double data[3] = { z, vz, azList };
		vector<double> storedata(data, data+3);
		pathList.push_back(storedata);
		return;
	}
	int stage = 0;
	double z_t, vz_t, azList_t;
	z_t = storeData[0][0];
	int tfint = Duration*PARAM::Vision::FRAME_RATE;
	int	maxStepNumber = /*(std::max)(tfint, 1) */(std::min)( 1000, (std::max)(tfint, 1));
	int indexStep = 3;
	for (int index = -1;index<=maxStepNumber;){
		if( index < 1 )
			index ++;
		else
			index += indexStep;

		if (index>=Duration*PARAM::Vision::FRAME_RATE)
			continue;
		while (index>=storeData[stage][5]*PARAM::Vision::FRAME_RATE)
			stage = stage + 1;		
		double t0  = storeData[stage-1][5];
		double vz0 = storeData[stage-1][1];
		double az  = storeData[stage-1][2];
		double rev = storeData[stage-1][3];
		azList_t = rev*az;	
		vz_t = rev*vz0 + rev*az * (index*1.0/PARAM::Vision::FRAME_RATE-t0);
		if (index!=0 ){
			if( index == 1 )
				z_t+=vz_t/PARAM::Vision::FRAME_RATE;
			else 
				z_t+=indexStep*vz_t/PARAM::Vision::FRAME_RATE;
		}

        if ( 1 == index)
        {
             z = z_t;
			 vz = vz_t;
			 azList = azList_t;
			 //std::cout<<"stage:"<<stage<<" index: "<<index<<" "<<z<<" "<<vz<<" "<<azList<<endl;
        }
        double data[3] = { z_t, vz_t, azList_t };
		vector<double> storedata(data, data+3);
		pathList.push_back(storedata);
	//	std::cout<<"stage:"<<stage<<" index: "<<index<<" "<<z<<" "<<vz<<" "<<azList<<endl;

		
	// This is to prevent errors when duration = 0
	if (tfint <= 1){
		//std::cout<<"duration = 0!!!!!"<<endl;
		z      = storeData[0][0];
		vz     = storeData[0][1];
		azList = storeData[0][2];
		//std::cout<<"tfint = 0"<<z<<" "<<vz<<" "<<azList<<endl;
		//std::cout<<z<<" "<<vz<<" "<<azList<<endl;
		}

	}
	}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	double syncTG(const PlayerPoseT& start, CGeoPoint finalPos,
/// 	const PlayerCapabilityT& capability)
///
/// @brief	采用二分法来分配 x 和 y  方向的最大速度和最大加速度，以达到 x, y 方向所用的时间
///         相等. 二分法搜索的变量为 alpha, 其变化范围是 [0, pi/2] 
///
/// @author	Yonghai Wu
/// @date	2009-12-9
///
/// @param	start		The start. 
/// @param	finalPos	The final position. 
/// @param	capability	The capability. 
///
/// @return	期望的整条路径所用的时间. 
////////////////////////////////////////////////////////////////////////////////////////////////////

double syncTGNone(const PlayerPoseT& start, CGeoPoint finalPos,const PlayerCapabilityT& capability)
{
	// Do a binary search to find the proper control effort, such that the times for the
	// trajectories in x and y direction are the same.
	// The variable to vary is "alpha", which runs from 0 to pi/2
	//The return value is the expected path time.
	const double factor = 1.2;

	double x0      = start.X();
	double y0      = start.Y();
	double xf      = finalPos.x();
	double yf      = finalPos.y();
	double vx0     = start.VelX();
	double vy0     = start.VelY();
    
	double maxSpeed = capability.maxSpeed;
	double maxAccel = capability.maxAccel;
//	double maxAngleSpeed = capability.maxAngularSpeed;
//	double maxAngleAccel = capability.maxAngularAccel;

	int numIter = 10;       //number of iterations performed. error in u is 2^(-numIter)
	double u   =  1.568;    // approx. PI/2
	double du  = -1.568;
    double alpha,durX,durY;
	for (int j=1;j<=numIter;j++)
	{
		du = du*0.5;
		alpha = u+du;
		double axMax = sin(alpha)*maxAccel;
		double ayMax = cos(alpha)*maxAccel;
		double vxMax = sin(alpha)*maxSpeed;
		double vyMax = cos(alpha)*maxSpeed;

		//Find duration for x-direction    
		int iter = 0;
		double t0 = 0;
		double rev = 1;
		vector<vector<double> > storeDataX = tg_recurs(t0, x0, xf, vx0, 1,rev, iter, axMax, factor*axMax, vxMax);
		double num_stagesX = storeDataX.size();
		durX = storeDataX[num_stagesX-1][5];
		//Find duration for y-direction    
		iter = 0;
		t0 = 0;
		rev = 1;
		vector<vector<double> > storeDataY = tg_recurs(t0, y0, yf, vy0, 1,rev, iter, ayMax, factor*ayMax, vyMax);
		double num_stagesY = storeDataY.size();
		durY =  storeDataY[num_stagesY-1][5];
		double tfDelta = durX-durY;
		//std::cout<<"alpha:"<<alpha<<"durx:"<<durX<<"dury:"<<durY<<endl;
		if (tfDelta <= 0.0)
			u=alpha;
	}
	double duration = (durX+durY)/2;
	return duration;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	void trapezoidalVelocityPath( const PlayerPoseT& start, const PlayerPoseT& final,
/// 	double frameRate, const PlayerCapabilityT& capability, PlayerPoseT& nextStep,
/// 	vector< vector<double> >& pathList)
///
/// @brief	非零速到点路径规划函数。算法流程如下：
///           1. 采用二分法，同步 x 和 y 方向的位移
///           2. 独立计算转动方向的所化时间 t_theta, 取 t_x, t_y, t_theta 中的最大值
///              为最后所花的时间
///
/// @author	Yonghai Wu
/// @date	2010-3-24
///
/// @param	start				The start. 
/// @param	final				The final. 
/// @param	frameRate			The frame rate. 
/// @param	capability			The capability. 
/// @param [in,out]	nextStep	the next step.  下一周期的位移和速度
/// @param [in,out]	pathList	list of pathes. 整条路径
////////////////////////////////////////////////////////////////////////////////////////////////////

void nonetrapezoidalVelocityPath( const PlayerPoseT& start,
							 const PlayerPoseT& final,
							 double frameRate,
							 const PlayerCapabilityT& capability,
							 PlayerPoseT& nextStep,
							 vector< vector<double> >& pathList)
	{
	

	double x0      = start.X();
	double y0      = start.Y();
	double xf      = final.X();
	double yf      = final.Y();
	double vx0     = start.VelX();
	double vy0     = start.VelY();
	double vxf     = final.VelX();
	double vyf     = final.VelY();
	double vtheta0 = start.RotVel();
	double vthetaf = final.RotVel();
	double theta0  = start.Dir();
	double thetaf  = final.Dir();

	double x = x0,vx = vx0 ,ax = 0,y = y0,vy = vy0,ay=0,theta = theta0,vtheta = vtheta0,atheta = 0;
	if (capability.maxAccel==0
		|| capability.maxAngularAccel==0
		|| capability.maxSpeed==0
		|| capability.maxAngularSpeed==0){
			cout << "trapezoidalVelocityPath code error: invalid scale(s)" << endl;
		}
	/*std::cout<<" maxacc: "<<capability.maxAccel<<" maxdec: "<<capability.maxDec<<" maxangacc: "<<capability.maxAngularAccel
		<<" maxangdec: "<<capability.maxAngularDec<<" maxspeed: "<<capability.maxSpeed<<endl;*/
	
	double maxSpeed = capability.maxSpeed;
	double maxAccel = capability.maxAccel;
	double maxAngleSpeed = capability.maxAngularSpeed;
	double maxAngleAccel = capability.maxAngularAccel;
	double maxDec = capability.maxDec;
	double maxAngleDec = capability.maxAngularDec;

	//finds the shortest "rotational path" between theta0 and thetaf
	double dtheta=thetaf-theta0;
	if (dtheta < -PARAM::Math::PI)
		dtheta = dtheta + 2*PARAM::Math::PI;
	if (dtheta > PARAM::Math::PI)
		dtheta = dtheta - 2*PARAM::Math::PI;
	thetaf=theta0 + dtheta;

	//double controlAlpha =
	// syncTG(x0,y0,vx0,vy0,maxAccel,maxAccel,maxSpeed); Do a binary
	// search to find the proper control effort, such that the times
	// for the trajectories in x and y direction are the same. The
	// variable to vary is "alpha", which runs from 0 to pi/2

	//  syncTG(start, final, capability);


	int numIter = 10;       //number of iterations performed. error in u is 2^(-numIter)
	double u   =  1.568;    // approx. PI/2
	double du  = -1.568;
	double alpha,durX,durY;
	vector<vector<double> > storeDataX,storeDataY;
	int iter = 0;
	double t0 = 0;
    double rev = 1;
	float normOfFinalSpeed =  sqrt(vxf*vxf + vyf*vyf);
		if ( normOfFinalSpeed > maxSpeed*0.9)
		{
			//std::cout<<"nonetrap: final speed is too big"<<endl;
            vxf = vxf/normOfFinalSpeed*maxSpeed*0.9;
			vyf = vyf/normOfFinalSpeed*maxSpeed*0.9;
		}
	for (int j=1;j<=numIter;j++)
		{	
			storeDataX.clear();
			storeDataY.clear();
		du = du*0.5;
		alpha = u+du;
		//alpha = PI/2;
		double axMax = sin(alpha)*maxAccel;
		double ayMax = cos(alpha)*maxAccel;
		double adxMax = sin(alpha)*maxDec;
		double adyMax = cos(alpha)*maxDec;
		double vxMax = sin(alpha)*maxSpeed;
		double vyMax = cos(alpha)*maxSpeed;
		
		//std::cout<<axMax<<" "<<ayMax<<endl;
		if ( vxMax < abs(vxf) ){
			vxMax =abs(vxf);
		    vyMax = sqrt(maxSpeed*maxSpeed - vxMax*vxMax);       
		}
		if ( vyMax < abs(vyf) ){
			vyMax = abs(vyf);
		    vxMax = sqrt(maxSpeed*maxSpeed - vyMax*vyMax);

		}
		//Find duration for x-direction    
		int iter = 0;
		double t0 = 0;
		double rev = 1;

		//////////////////////////////////////////////////////////////////////////

		storeDataX = tg_recurs(t0,
			x0,
			xf,
			vx0,
			vxf,
			rev,
			iter,
			axMax,
			adxMax,
			vxMax);
		double num_stagesX = storeDataX.size();
		durX = storeDataX[num_stagesX-1][5];
		
		//Find duration for y-direction    
		iter = 0;
		t0 = 0;
		rev = 1;
		storeDataY = tg_recurs(t0,
			y0,
			yf,
			vy0,
			vyf,
			rev,
			iter,
			ayMax,
			adyMax,
			vyMax);
		double num_stagesY = storeDataY.size();
		////////////////////////////////////////////////////////////////////////////
		
		
		////////////////////////////////////////////////////////////////////////////
		durY =  storeDataY[num_stagesY-1][5];
		double tfDelta = durX-durY;
		
		//std::cout<<"alpha:"<<alpha<<"durx:"<<durX<<"dury:"<<durY<<endl;
		if (tfDelta <= 0.0001 )
			{
			//std::cout<<j<<" alpha: "<<alpha<<" durx: "<<durX<<" dury: "<<durY<<endl;
			u=alpha;
			//     break;
			}
		if( fabs(tfDelta) < 0.001 )
			break;

		
		}
	


	//////////////////////////////////////////////////////////////////////////
//	_usecTimer.start();
	//////////////////////////////////////////////////////////////////////////

	//PRINT_DEBUG(alpha);
	//PRINT_DEBUG(durX);
	//PRINT_DEBUG(durY);

	vector<vector<double> >  storeDataTheta = tg_recurs(t0,
		theta0,
		thetaf,
		vtheta0,
		vthetaf,
		rev,
		iter,
		maxAngleAccel,
		maxAngleDec,
		maxAngleSpeed);

	vector< vector<double> > pathListR, pathListX, pathListY;
	buildPathNone(storeDataTheta,theta,vtheta,atheta,pathListR);
	buildPathNone(storeDataX,x,vx,ax, pathListX);
	buildPathNone(storeDataY,y,vy,ay, pathListY);
	

//	double duration = durX > durY ? durX : durY; // 运动控制总时间

	int pathStepX = pathListX.size();
	int pathStepY = pathListY.size();
	int pathStepR = pathListR.size();
	//std::cout<<" path len: r "<<pathStepR<<std::endl;
	////PRINT_DEBUG(pathStepX);
	//PRINT_DEBUG(pathStepY);
	//PRINT_DEBUG(pathStepR);
   
	nextStep.SetValid(true);
	nextStep.SetPos(x,y);
	nextStep.SetVel(vx,vy);
	nextStep.SetDir(theta);
	nextStep.SetRotVel(vtheta);
	//std::cout<<"~~ vx0: "<<vx0<<" vxf: "<<vxf<<" x0: "<<x0<<" xf: "<<xf<<" ax: "<<ax<<" t= "<<pathStepX<<std::endl;
	//std::cout<<"~~ vY0: "<<vy0<<" vyf: "<<vy0<<" y0: "<<y0<<" yf: "<<yf<<" yx: "<<ay<<" t= "<<pathStepY<<std::endl;
	// std::cout<<"~~ vx: "<<vx<<" vy: "<<vy<<std::endl;
	//std::cout<<" num of X: "<<pathStepX<<" num of Y: "<<pathStepY<<" num of R: "<<pathStepR<<endl;
	int maxPathStep = max( pathStepX, max(pathStepY, pathStepR) );
	//PRINT_DEBUG(maxPathStep);

	/// save every step of the path
	for (int i=0; i < maxPathStep; i++)
		{
		double data[7] = {0};
		data[0] = i;

		if ( i >= pathStepX)
			{
			if ( pathStepX > 0)
			{
				data[4] = pathListX[pathStepX-1][1];
				data[1] = pathListX[pathStepX-1][0]+(i-pathStepX+1)*data[4]/PARAM::Vision::FRAME_RATE;
			} 
			else
			{
				data[4] = vx0;
				data[1] = 0;
			}
			
			}
		else{
			data[1] = pathListX[i][0];
			data[4] = pathListX[i][1];
			}
		if ( i >= pathStepY)
			{
				if ( pathStepY > 0 )
				{
					data[5] = pathListY[pathStepY-1][1];
					data[2] = pathListY[pathStepY-1][0]+(i-pathStepY+1)*data[5]/PARAM::Vision::FRAME_RATE;
				} 
				else
				{
					data[2] = 0;
					data[5] = vy0;
				}
			
			}
		else{
			data[2] = pathListY[i][0];
			data[5] = pathListY[i][1];
			}

		if ( i >= pathStepR)
			{
				if ( pathStepR > 0)
				{
					data[6] = pathListR[pathStepR-1][1];
					data[3] = pathListR[pathStepR-1][0]+(i-pathStepR+1)*data[6]/PARAM::Vision::FRAME_RATE;
				} 
				else
				{
					data[3] = 0;
					data[6] = vtheta0;
				}
			
			}
		else{
			data[3] = pathListR[i][0];
			data[6] = pathListR[i][1];
			}
		vector<double>  storeData(data, data+7);
		pathList.push_back(storeData);
		//  PRINT_DEBUG(pathList.size());
		}
	}
