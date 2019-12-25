#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include "TrajectoryStructs.h"
#include <WorldDefine.h>

double distance(double x1, double y1, double x2, double y2);

//=================================================
// TIMING FUNCTIONS

double expectedPathTime( const PlayerPoseT& start, const CGeoPoint& final, double maxAccel, double maxVelocity );

double expectedFastPathTime( const PlayerPoseT& start, const CGeoPoint& final, double maxAccel, double maxVelocity );

// 考虑平动和朝向的到点时间估计
double expectedFullPathTime(const PlayerPoseT& start,
              const PlayerPoseT& final,
              double frameRate,
              const PlayerCapabilityT& capability);
//=================================================
// TRAJECTORY GENERATION FUNCTIONS

void zeroFinalVelocityPath( const PlayerPoseT& start,
                            const PlayerPoseT& final,
                            double frameRate,
                            const PlayerCapabilityT& capability,
              PlayerPoseT& nextStep
                          );

void fastPath( const PlayerPoseT& start,
               const PlayerPoseT& final,
               double frameRate,
               const PlayerCapabilityT& capability,
        PlayerPoseT& nextStep,
        const double finalVelDir = 0  
        );

//=================================================
// TRAJECTORY GENERATION FUNCTIONS

double zeroFinalVelocityTheta(const PlayerPoseT& start,
                const PlayerPoseT& final,
                double frameRate,
                const PlayerCapabilityT& capability
                );

//=================================================
// PROPORTIONAL GAIN FUNCTIONS

void proportionalGainPath(const PlayerPoseT& start,
                          const PlayerPoseT& final,
                          double frameRate,
                          const PlayerCapabilityT& capability,
              PlayerPoseT& nextStep
                         );


double proportionalGainTheta(const PlayerPoseT& start,
                           const PlayerPoseT& final,
                           double frameRate,
                           const PlayerCapabilityT& capability
                          );


#endif // TRAJECTORY_H