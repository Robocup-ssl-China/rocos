#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "TrajectorySupport.h"
#include <iostream>
//************************************************************************
double distance(double x1, double y1, double x2, double y2)
{
  return (double)sqrt ( sqr(x2-x1) + sqr(y2-y1) );
}

//************************************************************************		
void bangBangPathProperties(double z0,
                            double zf,
                            double vz0,
                            double ubar,
                            double &t1,
                            double &t2,
                            double &tf,
                            double &u)
{
    double c = z0-zf+vz0;

    if (ubar<0.01) 
      u=0;
    else 
      u = ubar*sgn(vz0/ubar-sgn(c)*((double)exp(fabs(c/ubar))-1));

    if (u==0)
      t1=t2=tf=u=0;
    else
    {
      t2 = (double)log(1+sqrt(1+exp(c/u)*(vz0/u-1)));
      t1 = t2-c/u;
      tf = t1+t2;
    }
}
//************************************************************************
double tFinal(double z0, double zf, double vz0, double ubar)
{
  double t1,t2,tf,u;
  bangBangPathProperties(z0,zf,vz0,ubar,t1,t2,tf,u);
  return tf;
}
//************************************************************************
double dtFinal(double x0, double xf, double vx0, double y0, double yf, double vy0,double u)
{
    return tFinal(x0,xf,vx0,u)-tFinal(y0,yf,vy0,(double)sqrt(1-sqr(u)));
}
//************************************************************************
// step returns position (z) and velocity (vz) at t=timestep*dt
// from the solution of
// z''(t)+z(t)'=uz 0<=t<t1z
// z''(t)+z(t)'=-uz t1z<=t<tfz
// z(0)=z0  z'(0)=vz0 z(tfz)=zf  z'(tfz)=0
// also z(t)=zf and z'(t)=0 tfz<t
//inputs (nondimensional)
//          z0 - starting point
//          zf - destination
//         vz0 - initial velocity (final velocity is zero)
//          uz - control effort
//         t1z - length of the first interval
//         tfz - total time
//          dt - length of timestep
//    timestep - number of timestep
//************************************************************************
void step(double z0, 
          double zf, 
          double vz0,
          double uz,
          double t1z,
          double tfz,
          double dt,
          int timestep,
          double& z,     // Return variable
          double& vz     // Return variable
          )
{
    double t=(double)timestep*dt;
    double expmt=(double)exp(-t);  
    if (t<=t1z)
    {
		/*std::cout<<"accelrate!!!!!!!!!!!"<<std::endl;*/
        z=uz*(t-1)+expmt*(uz-vz0)+z0+vz0;
        vz=uz+expmt*(vz0-uz);
    }
    else if (t<=tfz)
    {
		/*std::cout<<"dec!!!!!!!!!!!!!"<<std::endl;*/
        double exptfz=(double)exp(tfz);
        z=uz*(1-t+tfz-expmt*exptfz)+zf;
        vz=uz*(expmt*exptfz-1);
    }
    else
    {
		/*std::cout<<"STOP!!!!!!!!!!!!!!!!"<<std::endl;*/
        z=zf;
        vz=0;
    }
}