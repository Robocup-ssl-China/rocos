#include "RobotPredictError.h"
#include "staticparams.h"
#include <cmath>
#include <utils.h>
#include <fstream>
namespace{
	const double TRANSLATIONAL_ERROR_NOISE_CONSTANT  = 1.0;  // 厘米
	const double ROTATIONAL_ERROR_NOISE_CONSTANT     = 0.05;   // 弧度
	const double MAX_TRANSLATIONAL_ERROR             = 1.5;
	const double MAX_ROTATIONAL_ERROR                = 0.07;
	const int MAX_TRANSLATIONAL_ERROR_COUNT         = 3;
	const int MAX_ROTATIONAL_ERROR_COUNT            = 3;
	const int NUM_FRAMES_TO_AVERAGE                 = 3;
}
void CRobotPredictError::update(int cycle, const CRobotRawVisionLogger& rawVisionLogger, const CRobotVisionLogger& visionLogger)
{
	RobotVisionErrorData& errorData = _visionErrorLogger.getError(cycle);
	errorData.cycle = cycle;
	errorData.resetRotationalVelocity = errorData.resetTranslationalVelocity = false; // 初始化
	calculatePredictionError(cycle, rawVisionLogger, visionLogger);
	evaluatePredictionError(cycle, rawVisionLogger, visionLogger);
}
void CRobotPredictError::calculatePredictionError(int cycle, const CRobotRawVisionLogger& rawVisionLogger, const CRobotVisionLogger& visionLogger)
{
	if (rawVisionLogger.visionValid(cycle) ) {
		// 通过原始视觉信息计算机器人运动的距离
		const RobotRawVisionData& thisRawVision = rawVisionLogger.getVision(cycle); // 这个周期的原始视觉信息
		RobotVisionErrorData& errorData = _visionErrorLogger.getError(cycle);
		int count = 1;
		errorData.distanceTravelled = errorData.rotationTravelled = 0;
		while (count < PARAM::Latency::TOTAL_LATED_FRAME) {
			if( rawVisionLogger.visionValid(cycle - count) ){
				const RobotRawVisionData& lastRawVision = rawVisionLogger.getVision(cycle - count);
				errorData.distanceTravelled = std::sqrt( (thisRawVision.x - lastRawVision.x) * (thisRawVision.x - lastRawVision.x) + (thisRawVision.y - lastRawVision.y) * (thisRawVision.y - lastRawVision.y));
				// 2B错误 modified by zhyaic	2012.5.15
				errorData.rotationTravelled = std::abs(Utils::Normalize(thisRawVision.dir - lastRawVision.dir));
				break; // 计算完成
			}else{
				count++; // 向前退一个周期
			}
		}
		int predictedCycle = cycle -  PARAM::Latency::TOTAL_LATED_FRAME; // 当前原始视觉信息对应的n个周期前的预测过的视觉信息
		// 计算预测的误差
		if ( visionLogger.visionValid(predictedCycle) ){
			const PlayerVisionT& predictedVision = visionLogger.getVision(predictedCycle);
			if( predictedVision.Valid() ){
				errorData.predictedTranslationalError = std::sqrt( (thisRawVision.x - predictedVision.X()) * (thisRawVision.x - predictedVision.X()) + (thisRawVision.y - predictedVision.Y()) * (thisRawVision.y - predictedVision.Y()));
				errorData.predictedRotationalError = std::abs(Utils::Normalize(thisRawVision.dir - predictedVision.Dir()));
			}else{
				errorData.predictedTranslationalError = errorData.predictedRotationalError = 0;
			}
		}

		// Update path length
		translationalPathLength += errorData.distanceTravelled;
		rotationalPathLength += errorData.rotationTravelled;

		// 减去那个周期的信息，不知道为什么，fixme!!
		if ( _visionErrorLogger.errorValid(predictedCycle) ) {
			const RobotVisionErrorData& predictedCycleError = _visionErrorLogger.getError(predictedCycle);
			translationalPathLength  -= predictedCycleError.distanceTravelled;
			rotationalPathLength  -= predictedCycleError.rotationTravelled;
		}

		errorData.normalizedTranslationalError = ( errorData.predictedTranslationalError ) / ( TRANSLATIONAL_ERROR_NOISE_CONSTANT + translationalPathLength );
		errorData.normalizedRotationalError = 	( errorData.predictedRotationalError ) / ( ROTATIONAL_ERROR_NOISE_CONSTANT + rotationalPathLength );
	} 
}
void CRobotPredictError::evaluatePredictionError(int cycle, const CRobotRawVisionLogger& rawVisionLogger, const CRobotVisionLogger& visionLogger)
{
//	const RobotRawVisionData& thisRawVision = rawVisionLogger.getVision(cycle); // 这个周期的原始视觉信息
	const RobotVisionErrorData& errorData = _visionErrorLogger.getError(cycle);

	if ( errorData.normalizedTranslationalError > MAX_TRANSLATIONAL_ERROR ) {
		translationalErrorCount++;
	}else if (translationalErrorCount > 0) {
		translationalErrorCount--;
	}

	if ( errorData.normalizedRotationalError > MAX_ROTATIONAL_ERROR ) {
		rotationalErrorCount++;
	}else if (translationalErrorCount > 0) {
		rotationalErrorCount--;
	}

    if ( rotationalErrorCount > MAX_ROTATIONAL_ERROR_COUNT ){
//		int predictedCycle = cycle -  PARAM::Latency::TOTAL_LATED_FRAME; // 当前原始视觉信息对应的n个周期前的预测过的视觉信息
		for (int j=PARAM::Latency::TOTAL_LATED_FRAME; j >= 1; --j) {
			if( _visionErrorLogger.errorValid(cycle - j) ){
				RobotVisionErrorData& formerErrorData = _visionErrorLogger.getError(cycle - j);
				formerErrorData.resetRotationalVelocity = true;
				formerErrorData.resetTranslationalVelocity = true;
			}
		}
		rotationalErrorCount = 0;
    }else if ( translationalErrorCount > MAX_TRANSLATIONAL_ERROR_COUNT){
		for (int j=PARAM::Latency::TOTAL_LATED_FRAME; j >= 1; --j) {
			if( _visionErrorLogger.errorValid(cycle - j) ){
				RobotVisionErrorData& formerErrorData = _visionErrorLogger.getError(cycle - j);
				formerErrorData.resetTranslationalVelocity = true;
			}
		}
		translationalErrorCount = 0;
	}
}
