#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "Trajectory.h"
#include "TrajectorySupport.h"
using namespace std;

//************************************************************************
// zeroFinalVelocityPath solves
// x''(t)+x'(t)=ux(t)
// y''(t)+y'(t)=uy(t)
// theta''(t)+theta'(t)=utheta(t)
// with boundary conditions
// x(0)=x0  x'(0)=vx0 x(tf)=xf  x'(tf)=0
// y(0)=y0  y'(0)=vy0 y(tf)=yf  y'(tf)=0
// theta(0)=theta0  theta'(0)=vtheta0 theta(tf)=thetaf  theta'(tf)=0
// and the constraint ux(t)^2+uy(t)^2=1
// and returns the path
//inputs (dimensional)
//               z0 - starting point
//               zf - destination
//              vz0 - initial velocity (final velocity is zero)
//        frameRate - points in the path are generated with stepsize 1/frameRate
//         maxAccel - maximum translational acceleration
//    maxThetaAccel - maximum angular acceleration
//      maxVelocity - maximum translational velocity
// maxThetaVelocity - maximum angular velocity
//    numberOfSteps - number of points generated along the path (-1 returns the whole path)
//************************************************************************
void zeroFinalVelocityPath( const PlayerPoseT& start,
                            const PlayerPoseT& final,
                            double frameRate,
                            const PlayerCapabilityT& capability,
							PlayerPoseT& nextStep
                          )
{  
	int  i,j,numIter;
	double du,fmid,xmid,u;
	double t1x,t2x,tfx,ux;
	double t1y,t2y,tfy,uy;
	double t1theta,t2theta,tftheta,utheta;
	double dt,angulardt;
	double x,vx,y,vy,theta,vtheta;

	if (capability.maxAccel==0 || capability.maxAngularAccel==0 || capability.maxSpeed==0 || capability.maxAngularSpeed==0){
		cout << "Trajectory code error: invalid scale(s)" << endl;
	}

	double timeScale             = capability.maxSpeed/capability.maxAccel;
	double angularTimeScale      = capability.maxAngularSpeed/capability.maxAngularAccel;
	double lengthScale           = capability.maxSpeed * timeScale;
	double velocityScale         = capability.maxSpeed;
	double angularScale          = capability.maxAngularSpeed * angularTimeScale;						 
	double angularVelocityScale  = capability.maxAngularSpeed;

	//  nondimensionalize positions and velocities
	double oneoverlengthscale = 1/lengthScale;
	double x0      = start.X() * oneoverlengthscale;
	double y0      = start.Y() * oneoverlengthscale;
	double xf      = final.X() * oneoverlengthscale;
	double yf      = final.Y() *oneoverlengthscale;
	double vx0     = start.VelX() / velocityScale;
	double vy0     = start.VelY() / velocityScale;
	double vtheta0 = start.RotVel() / angularVelocityScale;

	double theta0  = start.Dir();
	double thetaf  = final.Dir();

	//finds the shortest "rotational path" between theta0 and thetaf
	double dtheta=thetaf-theta0;
	if (dtheta < -PI)
		dtheta = dtheta + 2*PI;
	if (dtheta > PI)
		dtheta = dtheta - 2*PI;
	thetaf=theta0 + dtheta;

	theta0 = theta0 / angularScale;
	thetaf = thetaf / angularScale;

	//finds the zero of tf(x0,xf,vx0,u)-tf(y0,yf,vy0,sqrt(1-sqr(u))) in (0,1) by bisection
	numIter = 10;//number of iterations performed. error in u is 2^(-numIter)
	u   =  0.999f;
	du  = -0.998f;

	for (j=0; j<numIter; j++) 
	{
		fmid = dtFinal(x0,xf,vx0,y0,yf,vy0,xmid=u+(du *= 0.5));
		if (fmid <= 0.0) u=xmid;
	}

	//now we calculate the path properties
	bangBangPathProperties(x0,xf,vx0,u,t1x,t2x,tfx,ux);
	bangBangPathProperties(y0,yf,vy0,(double)sqrt(1-sqr(u)),t1y,t2y,tfy,uy);

	//theta is independent of x and y so we use bang-bang with u=1
	bangBangPathProperties(theta0,thetaf,vtheta0,1,t1theta,t2theta,tftheta,utheta);

	double nondimFrameRate = frameRate*timeScale;
	double nondimAngularFrameRate = frameRate*angularTimeScale;
	int tfint = (int)ceil(max(tfx,tfy)*nondimFrameRate);
	int tfthetaint = (int)ceil(tftheta*nondimAngularFrameRate);

	dt = 1/nondimFrameRate;
	angulardt = 1/nondimAngularFrameRate;

	int	maxStepNumber=min(max(tfint,tfthetaint), 1); 
	// 只要下一个周期的状态就行了，没有必要多迭代，调试的时候可以把1改成其他值
	// 向前迭代
	for( i=0; i<=maxStepNumber; ++i ){
		// Calculate step along x-axis
		step(x0,xf,vx0,ux,t1x,tfx,dt,i,x,vx);
		x  = lengthScale*x;
		vx = velocityScale*vx;

		// Calculate step along y-axis
		step(y0,yf,vy0,uy,t1y,tfy,dt,i,y,vy);
		y  = lengthScale*y;
		vy = velocityScale*vy;

		// Calculate step along theta-axis
		step(theta0,thetaf,vtheta0,utheta,t1theta,tftheta,angulardt,i,theta,vtheta);
		theta  = angularScale*theta;
		vtheta = angularVelocityScale*vtheta;
		if( i == 1 ){ // 给下一个周期赋值
			nextStep.SetValid(true);
			nextStep.SetPos(x,y);
			nextStep.SetVel(vx,vy);
			nextStep.SetDir(theta);
			nextStep.SetRotVel(vtheta);
		}
	}
}

//************************************************************************
double zeroFinalVelocityTheta(const PlayerPoseT& start,
                            const PlayerPoseT& final,
                            double frameRate,
                            const PlayerCapabilityT& capability
                          )
{
	double t1theta,t2theta,tftheta,utheta;

	double angularTimeScale      = capability.maxAngularSpeed / capability.maxAngularAccel;
	double angularScale          = sqr(capability.maxAngularSpeed) / capability.maxAngularAccel;
	double angularVelocityScale  = capability.maxAngularSpeed;

	double theta0  = start.Dir();
	double thetaf  = final.Dir();

	double dtheta=thetaf-theta0;

	while (dtheta < -PI)
		dtheta += 2*PI;
	while (dtheta > PI)
		dtheta -= 2*PI;

	thetaf=theta0 + dtheta;

	theta0 = theta0 / angularScale;
	thetaf = thetaf / angularScale;



	double vtheta0 = start.RotVel()/angularVelocityScale;

	//use bang-bang with u=1 on the theta-axis
	bangBangPathProperties(theta0,thetaf,vtheta0,1,t1theta,t2theta,tftheta,utheta);

	double nondimAngularFrameRate = frameRate*angularTimeScale;
	double angulardt = 1/nondimAngularFrameRate;
	double temp=(double)exp(-angulardt);
	return angularVelocityScale * (temp*vtheta0+utheta*(1-temp));
}

//************************************************************************
void fastPath( const PlayerPoseT& start,
               const PlayerPoseT& final,
               double frameRate,
               const PlayerCapabilityT& capability,
			   PlayerPoseT& nextStep,
			   const double finalVelDir
			   )
{
	int  i;
	double x,vx,y,vy;

	if ( capability.maxAccel==0 || capability.maxSpeed==0 ){
		cout << "Trajectory code error: invalid scale(s)" << endl;
	}
	//fastPath能实现非零速度到点，在到达指定速度后保持匀速
	if (start.Pos().dist(final.Pos())<4)
    {
		vx = start.VelX();
		x = start.X()+vx/frameRate;
		vy =  start.VelY();
		y = start.Y()+vy/frameRate;
		double theta = start.Dir();
		nextStep.SetValid(true);
		nextStep.SetPos(x,y);
		nextStep.SetVel(vx,vy);
		nextStep.SetDir(theta);
		nextStep.SetRotVel(0);
		/*std::cout<<"keep velocity!!!!!!!!!!"<<endl;*/
    }
	else{
		double timeScale             = capability.maxSpeed / capability.maxAccel;
		double lengthScale           = sqr(capability.maxSpeed) / capability.maxAccel;
		double velocityScale         = capability.maxSpeed;

		//  nondimensionalize positions and velocities
		double x0      = start.X() / lengthScale;
		double y0      = start.Y() / lengthScale;
		double theta0  = start.Dir();
//		double thetaf  = final.Dir();
		double vx0     = start.VelX() / velocityScale;
		double vy0     = start.VelY() / velocityScale;
		double xf      = final.X() / lengthScale;
		double yf      = final.Y() / lengthScale;

		double dx=xf-x0;
		double dy=yf-y0;
		double t=0;
		double dt=0.01f;
		double func=0;
		double temp=0;
		//下面几行代码是求bangbang控制过程的加速（减速）时间
		while(func>=0)
		{
			t+=dt;
			temp=1-(double)exp(-t);
			func=sqr( dx-(vx0*temp) ) + sqr( dy- (vy0*temp) ) - sqr(temp-t);
		}
		double phi = (double)atan2((dy-vy0*temp) / (t-temp), (dx-vx0*temp) / (t-temp));
		//指定末速度方向时，控制角度相应变化，这样可以使得末速度方向为给定值
		//此时规划出来的轨迹理论上是一条折线
        if (finalVelDir)
        {
			phi = finalVelDir;
			//std::cout<<"final vel dir"<<finalVelDir<<endl;
        }
		double nondimFrameRate = frameRate*timeScale;
		int tfint = (int)ceil(t*nondimFrameRate);

		dt = 1/nondimFrameRate;
		int maxStepNumber = min(tfint, 1);
		// 只要下一个周期的状态就行了，没有必要多迭代，调试的时候可以把1改成其他值
		// 向前迭代一步
		for( i=0; i<=maxStepNumber; ++i ){
			// Calculate step along x-axis
			step(x0,xf,vx0,(double)cos(phi),t,t,dt,i, x, vx);
			x  = lengthScale*x;
			vx = velocityScale*vx;

			// Calculate step along y-axis
			step(y0,yf,vy0,(double)sin(phi),t,t,dt,i,y,vy);
			y  = lengthScale*y;
			vy = velocityScale*vy;
			if( i == 1 ){
				nextStep.SetValid(true);
				nextStep.SetPos(x,y);
				nextStep.SetVel(vx,vy);   
				nextStep.SetDir(theta0);
				nextStep.SetRotVel(0);
				if (vx==0 ||vy==0)
				{
					std::cout<<"zero!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
				}
			}
		}
		
	}
}
//************************************************************************
double expectedPathTime( const PlayerPoseT& start, const CGeoPoint& final, double maxAccel, double maxVelocity)
{
	int  j,numIter;
	double du,fmid,xmid,u;
	double t1x,t2x,tfx,ux;
	double t1y,t2y,tfy,uy;

	if (maxAccel==0 || maxVelocity==0)
		cout << "Trajectory code error: invalid scale(s)" << endl;

	double timeScale             = maxVelocity/maxAccel;
	double lengthScale           = sqr(maxVelocity)/maxAccel;
	double velocityScale         = maxVelocity;

	//  nondimensionalize positions and velocities
	double x0      = start.X()/lengthScale;
	double y0      = start.Y()/lengthScale;
	double vx0     = start.VelX()/velocityScale;
	double vy0     = start.VelY()/velocityScale;
	double xf      = final.x()/lengthScale;
	double yf      = final.y()/lengthScale;

	//finds the zero of tf(x0,xf,vx0,u)-tf(y0,yf,vy0,sqrt(1-sqr(u))) in (0,1) by bisection
	numIter = 10;//number of iterations performed
	u   =  0.999f;
	du  = -0.998f;

	for (j=1; j<=numIter; j++) 
	{
		fmid = dtFinal(x0,xf,vx0,y0,yf,vy0,xmid=u+(du *= 0.5));
		if (fmid <= 0.0) u=xmid;
	}

	//now we calculate the path properties
	bangBangPathProperties(x0,xf,vx0,u,t1x,t2x,tfx,ux);

	bangBangPathProperties(y0,yf,vy0,(double)sqrt(1 - u*u),t1y,t2y,tfy,uy);
	return timeScale * max(tfx,tfy);
}

double expectedFullPathTime(const PlayerPoseT& start,
							const PlayerPoseT& final,
							const PlayerCapabilityT& capability)
{
	double t1theta,t2theta,tftheta,utheta;
    
	if (capability.maxAccel==0 || capability.maxAngularAccel==0 || capability.maxSpeed==0 || capability.maxAngularSpeed==0){
		cout << "Trajectory code error: invalid scale(s)" << endl;
	}

	CGeoPoint target = CGeoPoint( final.Pos().x(), final.Pos().y());
	double moveTime = expectedPathTime(start, target, capability.maxAccel, capability.maxSpeed);

	
	double angularTimeScale      = capability.maxAngularSpeed/capability.maxAngularAccel;
	double angularScale          = capability.maxAngularSpeed * angularTimeScale;						 
	double angularVelocityScale  = capability.maxAngularSpeed;

	
	double vtheta0 = start.RotVel() / angularVelocityScale;

	double theta0  = start.Dir();
	double thetaf  = final.Dir();

	//finds the shortest "rotational path" between theta0 and thetaf
	double dtheta=thetaf-theta0;
	if (dtheta < -PI)
		dtheta = dtheta + 2*PI;
	if (dtheta > PI)
		dtheta = dtheta - 2*PI;
	thetaf=theta0 + dtheta;

	theta0 = theta0 / angularScale;
	thetaf = thetaf / angularScale;

	//theta is independent of x and y so we use bang-bang with u=1
	bangBangPathProperties(theta0,thetaf,vtheta0,1,t1theta,t2theta,tftheta,utheta);


	double tfthetaint = (tftheta*angularTimeScale);

	return max( tfthetaint, moveTime );
}

//=================================================
// PROPORTIONAL GAIN FUNCTIONS

void proportionalGainPath(const PlayerPoseT& start,
                          const PlayerPoseT& final,
                          double frameRate,
                          const PlayerCapabilityT& capability,
						  PlayerPoseT& nextStep
                         )
{

	double x0      = start.X();
	double y0      = start.Y();
	double theta0  = start.Dir();
	double xf      = final.X();
	double yf      = final.Y();
	double thetaf  = final.Dir();
//	double vx0     = start.VelX();
//	double vy0     = start.VelY();
//	double vtheta0 = start.RotVel();

	double dtheta=thetaf-theta0;//convert it!

	while (dtheta < -PI)
		dtheta += 2*PI;
	while (dtheta > PI)
		dtheta -= 2*PI;

	thetaf = theta0 + dtheta;

	double dt = 1/frameRate;

	double vx = capability.maxAccel * (xf-x0);
	double vy = capability.maxAccel * (yf-y0);
	double vtheta = capability.maxAngularAccel * dtheta;

	double speed = (double)sqrt(vx*vx + vy*vy);

	// Saturate translational velocity
	if (speed > capability.maxSpeed)
	{
		double scale=capability.maxSpeed/speed;
		vx = scale*vx;
		vy = scale*vy;
	}

	// Saturate angular velocity
	if (vtheta > capability.maxAngularSpeed)
	{
		vtheta = capability.maxAngularSpeed;
	}
	else if (vtheta < -capability.maxAngularSpeed)
	{
		vtheta = -capability.maxAngularSpeed;
	}

	nextStep.SetPos(x0+dt*vx,y0+dt*vy);
	nextStep.SetVel(vx,vy);
	nextStep.SetDir(theta0+dt*vtheta);
	nextStep.SetRotVel(vtheta);
}
//=================================================
double proportionalGainTheta(const PlayerPoseT& start,
                           const PlayerPoseT& final,
                           double frameRate,
                           const PlayerCapabilityT& capability
                          )
{

	double theta0  = start.Dir();
	double thetaf  = final.Dir();

	double dtheta=thetaf-theta0;

	if (dtheta < -PI)
		dtheta = dtheta + 2*PI;
	if (dtheta > PI)
		dtheta = dtheta - 2*PI;

	double nextAngularVelocity = capability.maxAngularAccel * dtheta;

	// Saturate angular velocity
	if (nextAngularVelocity > capability.maxAngularSpeed)
	{
		nextAngularVelocity = capability.maxAngularSpeed;
	}
	else if (nextAngularVelocity < -capability.maxAngularSpeed)
	{
		nextAngularVelocity = -capability.maxAngularSpeed;
	}
	return nextAngularVelocity;
}
