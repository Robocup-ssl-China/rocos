#include "CollisionSimulator.h"
#include <utils.h>
#include <GDebugEngine.h>

void CCollisionSimulator::reset(const CGeoPoint& ballRawPos, const CVector& ballVel)
{
	_ball.SetValid(true);
	_ball.SetRawPos(ballRawPos);
	_ball.SetPos(ballRawPos);
	_ball.SetVel(ballVel);
	_hasCollision = false;
}
void CCollisionSimulator::simulate(PlayerVisionT robot, const double time)
{
	const double timeStep = 0.005; // 模拟的迭代时间
	double timeSimulated = 0;
	const double boxSize = PARAM::Field::BALL_SIZE + PARAM::Vehicle::V2::PLAYER_SIZE;
	const double dribbleBarSize = (PARAM::Field::BALL_SIZE + PARAM::Vehicle::V2::PLAYER_SIZE * std::sin(PARAM::Vehicle::V2::HEAD_ANGLE/2) * 2); // 带球装置的宽度
	const double dribbleBarDist = PARAM::Vehicle::V2::PLAYER_SIZE * std::cos(PARAM::Vehicle::V2::HEAD_ANGLE/2) + PARAM::Field::BALL_SIZE;
	if( !_hasCollision ){
		while( !_hasCollision && timeSimulated < time - 0.001 ){
			timeSimulated += timeStep;
			robot.SetPos(robot.Pos() + robot.Vel() * timeStep);
			robot.SetDir(robot.Dir() + robot.RotVel() * timeStep);
			_ball.SetPos(_ball.Pos() + _ball.Vel() * timeStep);
			// 看是否碰撞了
			const CVector player2ball(_ball.Pos() - robot.Pos());
			const double ballDist = player2ball.mod();
			if( ballDist < boxSize){
				const double ballAbsDir = player2ball.dir(); // 求的绝对角度
				const double ballRelDir = Utils::Normalize( ballAbsDir - robot.Dir()); // 球的相对角度
				const double absBallRelDir = std::abs(ballRelDir);
				const double ballVerticalDist = ballDist * std::sin(ballRelDir);
				//原先absBallRelDir < PARAM::Math::PI*38/180 .实际车的控球角度只有15度左右,现改为PI*15/180试试,2009.1.14
				if( absBallRelDir < PARAM::Math::PI*10.1/180 /*&& std::abs(ballVerticalDist)  < dribbleBarSize*/ ){
					if ( ballDist - dribbleBarDist/cos(absBallRelDir) < 0.1 && 
						ballDist - dribbleBarDist/cos(absBallRelDir) > 0 ){
						_hasCollision = true;
						_ball.SetPos(robot.Pos() + Utils::Polar2Vector(dribbleBarDist/cos(absBallRelDir), ballAbsDir));
					} else if ( ballDist - dribbleBarDist/cos(absBallRelDir) < 0 ){
						_hasCollision = true;
                        double minRelDir = std::min(absBallRelDir, PARAM::Vehicle::V2::DRIBBLE_ANGLE);
						double minAbsDir = robot.Dir() + Utils::Sign(ballRelDir)*minRelDir;
						_ball.SetPos(robot.Pos() + Utils::Polar2Vector(dribbleBarDist/cos(minRelDir), minAbsDir));
					} else {
						_hasCollision = false;
					}
				}else if( absBallRelDir > PARAM::Math::PI*10/180){
					_ball.SetPos(robot.Pos() + Utils::Polar2Vector(boxSize, ballAbsDir));
				}

				if( _hasCollision){
					_ballRelToRobot = (_ball.Pos() - robot.Pos()).rotate(-robot.Dir());
					//_ball.SetRawPos(robot.RawPos() + (_ball.Pos() - robot.Pos()));
			//		_ball.SetVel(robot.Vel());
				}
			}
		}
	}
	if( _hasCollision){
		const double timeRemnant = time - timeSimulated; // 剩下的时间
		robot.SetPos(robot.Pos() + robot.Vel() * timeRemnant);
		robot.SetDir(robot.Dir() + robot.RotVel() * timeRemnant);
		_ball.SetPos(robot.Pos() + _ballRelToRobot.rotate(robot.Dir()));
	//	_ball.SetVel(robot.Vel());
		_ball.SetValid(true);
	}
}
