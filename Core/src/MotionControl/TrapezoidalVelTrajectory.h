#ifndef TRAPEZOIDAL_VEL_TRAJECTORY_H
#define TRAPEZOIDAL_VEL_TRAJECTORY_H
#include <WorldDefine.h>
#include <vector>



using namespace std;   
vector<vector<double>> tg_recurs(double t0, 
								 double z0, 
								 double zf,
								 double vz0, 
								 int rev, 
								 int iter,
								 double uza, 
								 double uzd, 
								 double vzMax);

vector<vector<double>> tg_recurs(double t0, 
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
								 double tol_vel);

void buildPath(const vector< vector<double> > & storeData,
			   double& z,
			   double& vz,
			   double& azList,
			   vector< vector<double> >& pathList);

void buildPath(const vector<vector<double>> & storeData,
			   double& z, 
			   double& vz, 
			   double& azList);

double syncTG(const PlayerPoseT& start,
			  CGeoPoint finalPos,
			  const PlayerCapabilityT& capability);

void trapezoidalVelocityPath( const PlayerPoseT& start,
						   const PlayerPoseT& final,
						   double frameRate,
						   const PlayerCapabilityT& capability,
						   PlayerPoseT& nextStep
						   );

void trapezoidalVelocityPath( const PlayerPoseT& start,
							 const PlayerPoseT& final,
							 double frameRate,
							 const PlayerCapabilityT& capability,
							 PlayerPoseT& nextStep,
							 vector< vector<double> >& pathList
							 );
#endif