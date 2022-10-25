#include "BallSpeedModel.h"
#include "staticparams.h"
#include <cmath>
#include <string>
#include <iostream>
#include <sstream> 
#include "parammanager.h"
using namespace std;

namespace{
	static int FRICTION;
}
static double oneFromFive(const double x1,const double y1,const double x2,const double y2,const double x3);

CBallSpeedModel::CBallSpeedModel():_ballVel(0,0),_ballPos(-9999,-9999){
    bool IS_SIMULATION;
    ZSS::ZParamManager::instance()->loadParam(IS_SIMULATION,"Alert/IsSimulation",false);
    if (IS_SIMULATION)
        ZSS::ZParamManager::instance()->loadParam(FRICTION,"AlertParam/Friction4Sim",800);
    else
        ZSS::ZParamManager::instance()->loadParam(FRICTION,"AlertParam/Friction4Real",1520);
}
CBallSpeedModel::~CBallSpeedModel(){
}

//todo
CVector CBallSpeedModel::speedForTime(double frame, const CVisionModule* pVision ){
	update(pVision);
    return speedForTime_FM(frame);
}

//todo
CVector CBallSpeedModel::speedForDist(double dist, const CVisionModule* pVision) {
    update(pVision);
    return speedForDist_FM(dist);
}

//use new speed mode
double CBallSpeedModel::timeForDist(double dist, const CVisionModule* pVision ){
    update(pVision);
    return timeForDist_FM(dist);
}

//use new speed mode
CGeoPoint CBallSpeedModel::posForTime(double frame, const CVisionModule* pVision ){
    update(pVision);
    return posForTime_FM(frame);
}
CVector CBallSpeedModel::speedForTime_FM(double frame) {
	auto vel = _ballVel.mod() - 0.5 * FRICTION * frame*1.0 / PARAM::Vision::FRAME_RATE;
	if (vel < 0) vel = 0;
	return _ballVel / _ballVel.mod() * vel;
}
CVector CBallSpeedModel::speedForDist_FM(double dist) {
	auto v = _ballVel.mod();
	auto t = v*v - FRICTION*dist;
	if (t <= 0) return CVector(0, 0);
	auto vel = std::sqrt(t);
	return _ballVel / _ballVel.mod() * vel;
}
double CBallSpeedModel::timeForDist_FM(double dist) {
	auto v = _ballVel.mod();
	auto a = 0.25*FRICTION;
	auto b = -v;
	auto c = dist;
	auto delta = b*b - 4 * a*c;
	if (delta < 0) return -1;
    return 1.0 / (2 * a)*(-b - std::sqrt(delta));
}
CGeoPoint CBallSpeedModel::posForTime_FM(double frame) {
	auto v0 = _ballVel.mod();
	auto v1 = v0 - 2 * FRICTION*frame/PARAM::Vision::FRAME_RATE;
	double d;
	if (v1 < 0)
		d = v0*v0 / FRICTION;
	else
		d = (v0 + v1)*frame / PARAM::Vision::FRAME_RATE / 2;
	return _ballPos + (_ballVel / _ballVel.mod()*d);
}
//todo
//double CBallSpeedModel::CalKickInitSpeed(const double dist)
//{
//	double vt = 100;
//	if (dist < 100) {
//		vt = 120;
//	} else if (dist >= 100 && dist < 200) {
//		vt = 200;
//	} else if (dist >= 200 && dist < 300) {
//		vt = 350;
//	} else if (dist >= 300 && dist < 400) {
//		vt = 350;
//	} else if (dist >= 400) {
//		vt = 350;
//	}
//	vt = vt+100;//让传球速度变小一点
//	double ballVO = sqrt(vt * vt + 2 * 20 * dist);
//	//迭代法计算初速度
//	for (int i = 1;i<=3;i++){
//		if(ballVO > 240){
//			cal_acc = 10;
//		} else if(ballVO > 190 && ballVO <= 240){
//			cal_acc = ( 0.0025 * ballVO + 0.18 ) * 60;
//		} else if(ballVO > 140 && ballVO <= 190){
//			cal_acc = ( (0.0025 + ( 190 - ballVO) *0.00001)  * ballVO + 0.18 ) * 60;
//		} else if (ballVO > 10 && ballVO <= 140){
//			cal_acc = ( 0.003  * ballVO + 0.18 ) * 60;
//		}
//		ballVO = sqrt(vt * vt + 2 * cal_acc * dist);
//	}
//	return ballVO;
//}
void CBallSpeedModel::update( const CVisionModule* pVision ){
	//lastBallPos = _ballPos;
    _ballPos = pVision->ball().Valid() ? pVision->ball().RawPos() : pVision->ball().Pos();
    _ballVel = pVision->ball().Vel();
}

//double CBallSpeedModel::getAcc(double speed) const{
//	if(speed >= 500) return 1.7;
//	if(speed > 300) return speed/200.0 - 0.8;	//300 - 500 line : (300,0.7) , (500,1.7)
//	if(speed >= 0.7) return 0.7;
//	if(speed >= 0) return speed;
//	cout << "BallSpeedModel.cpp : Speed Mod < 0 in getAcc Function" << endl;
//	return -1;
//}

//static double oneFromFive(const double x1,const double y1,const double x2,const double y2,const double x3){
//	return (y2-y1)/(x2-x1)*(x3-x2)+y2;
//}
