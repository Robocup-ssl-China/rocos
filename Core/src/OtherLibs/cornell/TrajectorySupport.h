#ifndef TRAJECTORY_SUPPORT_H
#define TRAJECTORY_SUPPORT_H

#include "TrajectoryStructs.h"

inline double sqr(double x) { return x*x; }
inline int sgn(double a) { return (a>0)?1:((a<0)?-1:0); }
inline bool isZero(double f) { return (fabs(f)<0.00001)?true:false; }
inline bool nonZero(double f) { return !isZero(f); }
inline double sgnf(double a) { return (a>0)?1.0f:((a<0)?-1.0f:0.0f); }

#ifndef PI
  #define PI	3.1415926f
#endif

//************************************************************************
double distance(double x1, double y1, double x2, double y2);

//************************************************************************
void bangBangPathProperties(double z0,
                            double zf,
                            double vz0,
                            double ubar,
                            double &t1,
                            double &t2,
                            double &tf,
                            double &u);

//************************************************************************
double tFinal(double z0, double zf, double vz0, double ubar);


//************************************************************************
double dtFinal(double x0, double xf, double vx0, double y0, double yf, double vy0,double u);


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
          );

#endif //TRAJECTORY_SUPPORT_H
