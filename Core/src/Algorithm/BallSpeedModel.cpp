#include "BallSpeedModel.h"
#include "VisionModule.h"
#include "staticparams.h"
#include <cmath>
#include <string>
#include <iostream>
#include <sstream> 
#include "parammanager.h"
using namespace std;

namespace{
	// _DEC > 0 from friction
	static double _DEC;
}

CBallSpeedModel::CBallSpeedModel():_ballVel(0,0),_ballPos(-9999,-9999){
    bool IS_SIMULATION;
    ZSS::ZParamManager::instance()->loadParam(IS_SIMULATION,"Alert/IsSimulation",false);
    if (IS_SIMULATION)
		ZSS::ZParamManager::instance()->loadParam(_DEC, "AlertParam/BallDec_Sim", 1100);
	else
		ZSS::ZParamManager::instance()->loadParam(_DEC, "AlertParam/BallDec_Real", 400);
}
CBallSpeedModel::~CBallSpeedModel(){
}
double CBallSpeedModel::timeForDist(const double dist){
	return std::get<0>(predictForDist(dist));
}
CVector CBallSpeedModel::speedForDist(const double dist){
	return std::get<1>(predictForDist(dist));
}
std::tuple<double, CVector> CBallSpeedModel::predictForDist(const double dist){
	this->update();
	auto maxDist = _ballVel.mod() * _ballVel.mod() / (2 * _DEC);
	if (dist > maxDist) {
		return {99999, CVector(0,0)};
	}
	auto v0 = _ballVel.mod();
	auto v1 = sqrt(v0 * v0 - 2 * _DEC * dist);
	auto time = (v0 - v1) / _DEC;
	return {time, Utils::Polar2Vector(v1, _ballVel.dir())};
}
ObjectPoseT CBallSpeedModel::poseForTime(const double time){
	this->update();
	ObjectPoseT predictPose;
	auto v0 = _ballVel.mod();
	auto v1 = std::max(v0 - _DEC * time, 0.0);
	auto runtime = std::min(v0 / _DEC, time);
	double dist = (v0 + v1) * runtime / 2;
	predictPose.SetPos(_ballPos + Utils::Polar2Vector(dist, _ballVel.dir()));
	predictPose.SetVel(Utils::Polar2Vector(v1, _ballVel.dir()));
	return predictPose;
}

void CBallSpeedModel::update(){
	if(lastCycle == vision->getCycle()) return;
	lastCycle = vision->getCycle();

	_ballPos = vision->ball().Valid() ? vision->ball().RawPos() : vision->ball().Pos();
	_ballVel = vision->ball().Vel();
}
