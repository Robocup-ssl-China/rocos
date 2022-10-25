#ifndef _ROBOT_PREDICT_ERROR_H_
#define _ROBOT_PREDICT_ERROR_H_
#include "RobotPredictData.h"
class CRobotPredictError{
public:
	CRobotPredictError() : translationalPathLength(0), rotationalPathLength(0), translationalErrorCount(0), rotationalErrorCount(0){ } 
	bool resetTranslationalVelocity(int cycle) const
	{
		if( _visionErrorLogger.errorValid(cycle) ){
			return _visionErrorLogger.getError(cycle).resetTranslationalVelocity;
		}
		return false;
	}
	bool resetRotationalVelocity(int cycle) const
	{
		if( _visionErrorLogger.errorValid(cycle) ){
			return _visionErrorLogger.getError(cycle).resetRotationalVelocity;
		}
		return false;
	}
	void update(int cycle, const CRobotRawVisionLogger& rawVisionLogger, const CRobotVisionLogger& visionLogger);
protected:
	void calculatePredictionError(int cycle, const CRobotRawVisionLogger& rawVisionLogger, const CRobotVisionLogger& visionLogger);
	void evaluatePredictionError(int cycle, const CRobotRawVisionLogger& rawVisionLogger, const CRobotVisionLogger& visionLogger);
private:
	CRobotVisionErrorLogger _visionErrorLogger;
	double translationalPathLength;
	double rotationalPathLength;
	int translationalErrorCount;
	int rotationalErrorCount;
};
#endif // _ROBOT_PREDICT_ERROR_H_