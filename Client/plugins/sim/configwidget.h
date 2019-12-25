/*
grSim - RoboCup Small Size Soccer Robots Simulator
Copyright (C) 2011, Parsian Robotic Center (eew.aut.ac.ir/~parsian/grsim)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include "parammanager.h"

namespace ZSS{
class ParamManagerSimulator:public ParamManager{
public:
    ParamManagerSimulator():ParamManager("zss_simulator.ini"){}
    ~ParamManagerSimulator(){}
};
typedef Singleton<ParamManagerSimulator> pm;
}

#define DEF_VALUE(type,Type,name)  \
    type v_##name; \
    inline type name() { return v_##name; }

#define DEF_ENUM(type,name)  \
    DEF_VALUE(type,type,name)

class RobotSettings {
public:
    //geometeric settings
    double RobotCenterFromKicker;
    double RobotRadius;
    double RobotHeight;
    double BottomHeight;
    double KickerZ;
    double KickerThickness;
    double KickerWidth;
    double KickerHeight;
    double NoSpiralWidth;
    double WheelRadius;
    double WheelThickness;
    double Wheel1Angle;
    double Wheel2Angle;
    double Wheel3Angle;
    double Wheel4Angle;
    //physical settings
    double BodyMass;
    double WheelMass;
    double KickerMass;
    double KickerDampFactor;
    double RollerTorqueFactor;
    double RollerPerpendicularTorqueFactor;
    double Kicker_Friction;
    double WheelTangentFriction;
    double WheelPerpendicularFriction;
    double Wheel_Motor_FMax;
};

class ConfigWidget
{ 
public:
  ConfigWidget();
  virtual ~ConfigWidget();

  QSettings* robot_settings;
  RobotSettings robotSettings;
  RobotSettings blueSettings;
  RobotSettings yellowSettings;

  /*    Geometry/Game Vartypes   */
  DEF_VALUE(int,Int,Robots_Count)
  DEF_VALUE(double,Double,Field_Line_Width)
  DEF_VALUE(double,Double,Field_Length)
  DEF_VALUE(double,Double,Field_Width)
  DEF_VALUE(double,Double,Field_Rad)
  DEF_VALUE(double,Double,Field_Free_Kick)
  DEF_VALUE(double,Double,Field_Penalty_Width)
  DEF_VALUE(double,Double,Field_Penalty_Depth)
  DEF_VALUE(double,Double,Field_Penalty_Point)
  DEF_VALUE(double,Double,Field_Margin)
  DEF_VALUE(double,Double,Field_Referee_Margin)
  DEF_VALUE(double,Double,Wall_Thickness)
  DEF_VALUE(double,Double,Goal_Thickness)
  DEF_VALUE(double,Double,Goal_Depth)
  DEF_VALUE(double,Double,Goal_Width)
  DEF_VALUE(double,Double,Goal_Height)
  DEF_VALUE(double,Double,overlap)
  DEF_VALUE(QString,String,YellowTeam)
  DEF_VALUE(QString,String,BlueTeam)
  DEF_VALUE(double,Double,BallRadius)
  DEF_VALUE(double,Double,BallMass)
  DEF_VALUE(double,Double,BallFriction)
  DEF_VALUE(double,Double,BallSlip)
  DEF_VALUE(double,Double,BallBounce)
  DEF_VALUE(double,Double,BallBounceVel)
  DEF_VALUE(double,Double,BallLinearDamp)
  DEF_VALUE(double,Double,BallAngularDamp)

  DEF_VALUE(bool,Bool,SyncWithGL)
  DEF_VALUE(double,Double,DesiredFPS)
  DEF_VALUE(double,Double,DeltaTime)
  DEF_VALUE(int,Int,sendGeometryEvery)
  DEF_VALUE(double,Double,Gravity)
  DEF_VALUE(QString,String,VisionMulticastAddr)
  DEF_VALUE(int,Int,VisionMulticastPort)  
  DEF_VALUE(int,Int,CommandListenPort)
  DEF_VALUE(int,Int,BlueStatusSendPort)
  DEF_VALUE(int,Int,YellowStatusSendPort)
  DEF_VALUE(int,Int,sendDelay)
  DEF_VALUE(bool,Bool,noise)
  DEF_VALUE(double,Double,noiseDeviation_x)
  DEF_VALUE(double,Double,noiseDeviation_y)
  DEF_VALUE(double,Double,noiseDeviation_angle)
  DEF_VALUE(bool,Bool,vanishing)
  DEF_VALUE(double,Double,ball_vanishing)
  DEF_VALUE(double,Double,blue_team_vanishing)
  DEF_VALUE(double,Double,yellow_team_vanishing)
  DEF_VALUE(QString, String, plotter_addr)
  DEF_VALUE(int, Int, plotter_port)
  DEF_VALUE(bool, Bool, plotter)
  void loadRobotSettings(QString team);
public Q_SLOTS:  
  void loadRobotsSettings();
};

#endif // CONFIGWIDGET_H
