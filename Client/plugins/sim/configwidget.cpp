#include "configwidget.h"
namespace{
    auto ppm = ZSS::pm::instance();
    auto zpm = ZSS::ZParamManager::instance();
}
//#define END_ENUM(parents, name) \
//    parents->addChild(v_##name);
//#define ADD_TO_ENUM(name,str) \
//    v_##name->addItem(str);
//#define ADD_ENUM(parent,type,name,Defaultvalue,namestring) \
//    v_##name = std::tr1::shared_ptr<Var##type>(new Var##type(namestring,Defaultvalue));
//#define ADD_VALUE(parent,type,name,defaultvalue,namestring) \
//    v_##name = std::tr1::shared_ptr<Var##type>(new Var##type(namestring,defaultvalue)); \
//    parent->addChild(v_##name);

#define ADD_VALUE(parent,type,name,defaultvalue,namestring) \
    ppm->loadParam(v_##name,#parent"/"#name,defaultvalue);

#define ADD_CLIENT_VALUE(parent, type, name, defaultvalue, namestring, ratio) \
    v_##name = zpm->value(#parent "/" #namestring, QVariant(ratio * defaultvalue)).to##type() / ratio;

ConfigWidget::ConfigWidget()
{
    ADD_VALUE(game_vars,Int,Robots_Count, 16, "Robots Count")
    ADD_VALUE(field_vars,Double,Field_Line_Width,0.020,"Line Thickness")
    ADD_CLIENT_VALUE(field,Double,Field_Length,9.000,width, 1000.0)
    ADD_CLIENT_VALUE(field,Double,Field_Width,6.000,height, 1000.0)
    ADD_VALUE(field_vars,Double,Field_Rad,0.500,"Radius")
    ADD_VALUE(field_vars,Double,Field_Free_Kick,0.700,"Free Kick Distanse From Defense Area")
    ADD_CLIENT_VALUE(field,Double,Field_Penalty_Width,2.0,penaltyLength, 1000.0)
    ADD_CLIENT_VALUE(field,Double,Field_Penalty_Depth,1.0,penaltyWidth, 1000.0)
    ADD_VALUE(field_vars,Double,Field_Penalty_Point,1.0,"Penalty point")
    ADD_VALUE(field_vars,Double,Field_Margin,0.4,"Margin")
    ADD_VALUE(field_vars,Double,Field_Referee_Margin,0.0,"Referee margin")
    ADD_VALUE(field_vars,Double,Wall_Thickness,0.050,"Wall thickness")
    ADD_VALUE(field_vars,Double,Goal_Thickness,0.020,"Goal thickness")
    ADD_CLIENT_VALUE(field,Double,Goal_Depth,0.200,goalDepth, 1000.0)
    ADD_CLIENT_VALUE(field,Double,Goal_Width,1.000,goalWidth, 1000.0)
    ADD_VALUE(field_vars,Double,Goal_Height,0.160,"Goal height")
    ADD_VALUE(field_vars,Double,overlap,0.20,"Camera Overlap")

    ADD_VALUE(game_vars,String,YellowTeam,"ZJUNlict","Yellow Team")
    ADD_VALUE(game_vars,String,BlueTeam,"ZJUNlict","Blue Team")

    ADD_VALUE(ballg_vars,Double,BallRadius,0.0215,"Radius")
    ADD_VALUE(worldp_vars,Double,DesiredFPS,65,"Desired FPS")
    ADD_VALUE(worldp_vars,Bool,SyncWithGL,false,"Synchronize ODE with OpenGL")
    ADD_VALUE(worldp_vars,Double,DeltaTime,0.016,"ODE time step")
    ADD_VALUE(worldp_vars,Double,Gravity,9.8,"Gravity")
    ADD_VALUE(ballp_vars,Double,BallMass,0.043,"Ball mass");
    ADD_VALUE(ballp_vars,Double,BallFriction,0.05,"Ball-ground friction")
    ADD_VALUE(ballp_vars,Double,BallSlip,1,"Ball-ground slip")
    ADD_VALUE(ballp_vars,Double,BallBounce,0.5,"Ball-ground bounce factor")
    ADD_VALUE(ballp_vars,Double,BallBounceVel,0.1,"Ball-ground bounce min velocity")
    ADD_VALUE(ballp_vars,Double,BallLinearDamp,0.004,"Ball linear damping")
    ADD_VALUE(ballp_vars,Double,BallAngularDamp,0.004,"Ball angular damping")

    ADD_VALUE(comm_vars,String,VisionMulticastAddr,"224.5.23.2","Vision multicast address")  //SSL Vision: "224.5.23.2"
    ADD_VALUE(comm_vars,Int,VisionMulticastPort,10020,"Vision multicast port")
    ADD_VALUE(comm_vars,Int,CommandListenPort,20011,"Command listen port")
    ADD_VALUE(comm_vars,Int,BlueStatusSendPort,30011,"Blue Team status send port")
    ADD_VALUE(comm_vars,Int,YellowStatusSendPort,30012,"Yellow Team status send port")
    ADD_VALUE(comm_vars,Int,sendDelay,0,"Sending delay (milliseconds)")
    ADD_VALUE(comm_vars,Int,sendGeometryEvery,120,"Send geometry every X frames")

    ADD_VALUE(gauss_vars,Bool,noise,false,"Noise")
    ADD_VALUE(gauss_vars,Double,noiseDeviation_x,3,"Deviation for x values")
    ADD_VALUE(gauss_vars,Double,noiseDeviation_y,3,"Deviation for y values")
    ADD_VALUE(gauss_vars,Double,noiseDeviation_angle,2,"Deviation for angle values")

    ADD_VALUE(gauss_vars,Bool,vanishing,false,"Vanishing")
    ADD_VALUE(vanishing_vars,Double,blue_team_vanishing,0,"Blue team")
    ADD_VALUE(vanishing_vars,Double,yellow_team_vanishing,0,"Yellow team")
    ADD_VALUE(vanishing_vars,Double,ball_vanishing,0,"Ball")

    loadRobotsSettings();
}

ConfigWidget::~ConfigWidget() {
}

void ConfigWidget::loadRobotsSettings()
{
    loadRobotSettings(YellowTeam());
    yellowSettings = robotSettings;
    loadRobotSettings(BlueTeam());
    blueSettings = robotSettings;
}

void ConfigWidget::loadRobotSettings(QString team)
{
    QString ss = QString("../config/")+QString("%1.ini").arg(team);
    robot_settings = new QSettings(ss, QSettings::IniFormat);
    robotSettings.RobotCenterFromKicker = robot_settings->value("Geometery/CenterFromKicker", 0.073).toDouble();
    robotSettings.RobotRadius = robot_settings->value("Geometery/Radius", 0.09).toDouble();
    robotSettings.RobotHeight = robot_settings->value("Geometery/Height", 0.13).toDouble();
    robotSettings.BottomHeight = robot_settings->value("Geometery/RobotBottomZValue", 0.02).toDouble();
    robotSettings.KickerZ = robot_settings->value("Geometery/KickerZValue", 0.005).toDouble();
    robotSettings.KickerThickness = robot_settings->value("Geometery/KickerThickness", 0.005).toDouble();
    robotSettings.KickerWidth = robot_settings->value("Geometery/KickerWidth", 0.08).toDouble();
    robotSettings.KickerHeight = robot_settings->value("Geometery/KickerHeight", 0.04).toDouble();
    robotSettings.NoSpiralWidth = robot_settings->value("Geometery/NoSpiralWidth", 0.04).toDouble();
    robotSettings.WheelRadius = robot_settings->value("Geometery/WheelRadius", 0.0325).toDouble();
    robotSettings.WheelThickness = robot_settings->value("Geometery/WheelThickness", 0.005).toDouble();
    robotSettings.Wheel1Angle = robot_settings->value("Geometery/Wheel1Angle", 60).toDouble();
    robotSettings.Wheel2Angle = robot_settings->value("Geometery/Wheel2Angle", 135).toDouble();
    robotSettings.Wheel3Angle = robot_settings->value("Geometery/Wheel3Angle", 225).toDouble();
    robotSettings.Wheel4Angle = robot_settings->value("Geometery/Wheel4Angle", 300).toDouble();

    robotSettings.BodyMass  = robot_settings->value("Physics/BodyMass", 2).toDouble();
    robotSettings.WheelMass = robot_settings->value("Physics/WheelMass", 0.2).toDouble();
    robotSettings.KickerMass= robot_settings->value("Physics/KickerMass",0.02).toDouble();
    robotSettings.KickerDampFactor = robot_settings->value("Physics/KickerDampFactor", 0.2f).toDouble();
    robotSettings.RollerTorqueFactor = robot_settings->value("Physics/RollerTorqueFactor", 0.06f).toDouble();
    robotSettings.RollerPerpendicularTorqueFactor = robot_settings->value("Physics/RollerPerpendicularTorqueFactor", 0.005f).toDouble();
    robotSettings.Kicker_Friction = robot_settings->value("Physics/KickerFriction", 0.8f).toDouble();
    robotSettings.WheelTangentFriction = robot_settings->value("Physics/WheelTangentFriction", 0.8f).toDouble();
    robotSettings.WheelPerpendicularFriction = robot_settings->value("Physics/WheelPerpendicularFriction", 0.05f).toDouble();
    robotSettings.Wheel_Motor_FMax = robot_settings->value("Physics/WheelMotorMaximumApplyingTorque", 0.2f).toDouble();
}
