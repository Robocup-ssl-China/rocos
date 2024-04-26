#include "utils.h"
#include "WorldModel.h"
#include "staticparams.h"
#include <GDebugEngine.h>
#include <iostream>

namespace Utils{
    double dirDiff(const CVector& v1, const CVector& v2) { return fabs(Normalize(v1.dir() - v2.dir()));}
	double Normalize(double angle)
	{
		if (fabs(angle) > 10 ){
			cout<<angle<<" Normalize Error!!!!!!!!!!!!!!!!!!!!"<<endl;
			return 0;
		}
        static const double M_2PI = PARAM::Math::PI * 2;
		// 快速粗调整
		angle -= (int)(angle / M_2PI) * M_2PI; 
		
		// 细调整 (-PI,PI]
		while( angle > PARAM::Math::PI ) {
			angle -= M_2PI;
		}

		while( angle <= -PARAM::Math::PI ) {
			angle += M_2PI;
		}

		return angle;
	}
	CVector Polar2Vector(double m,double angle)
	{
		return CVector(m*std::cos(angle),m*std::sin(angle));
	}
	double VectorDot(const CVector& v1, const CVector& v2)
	{
		return v1.x()*v2.x() + v1.y()*v2.y();
	}
	bool InBetween(const CGeoPoint& p,const CGeoPoint& p1,const CGeoPoint& p2)
	{
		return p.x() >= (std::min)(p1.x(),p2.x()) && p.x() <= (std::max)(p1.x(),p2.x())
			&& p.y() >= (std::min)(p1.y(),p2.y()) && p.y() <= (std::max)(p1.y(),p2.y());
	}
	bool InBetween(double v,double v1,double v2)
	{
		return (v > v1 && v < v2) || (v < v1 && v > v2);
	}
	bool InBetween(const CVector& v, const CVector& v1, const CVector& v2, double buffer)
	{
		
		double d = v.dir(), d1 = v1.dir(), d2 = v2.dir();
		return AngleBetween(d, d1, d2, buffer);
	}

	bool AngleBetween(double d, double d1, double d2, double buffer)
	{
		using namespace PARAM::Math;
		// d, d1, d2为向量v, v1, v2的方向弧度

		// 当v和v1或v2的角度相差很小,在buffer允许范围之内时,认为满足条件
		double error = (std::min)(std::fabs(Normalize(d-d1)), std::fabs(Normalize(d-d2)));
		if (error < buffer) {
			return true;
		}

		if (std::fabs(d1 - d2) < PI) {
			// 当直接相减绝对值小于PI时, d应该大于小的,小于大的
			return InBetween(d, d1, d2);
		}
		else {
			// 化为上面那种情况
			return InBetween(Normalize(d+PI), Normalize(d1+PI), Normalize(d2+PI));
		}
	}

	CGeoPoint MakeInField(const CGeoPoint& p,const double buffer){
        auto new_p = p;
        if (new_p.x() < buffer - PARAM::Field::PITCH_LENGTH / 2) new_p.setX(buffer - PARAM::Field::PITCH_LENGTH / 2);
        if (new_p.x() > PARAM::Field::PITCH_LENGTH / 2 - buffer) new_p.setX(PARAM::Field::PITCH_LENGTH / 2 - buffer);
        if (new_p.y() < buffer - PARAM::Field::PITCH_WIDTH / 2) new_p.setY(buffer - PARAM::Field::PITCH_WIDTH / 2);
        if (new_p.y() > PARAM::Field::PITCH_WIDTH / 2 - buffer) new_p.setY(PARAM::Field::PITCH_WIDTH / 2 - buffer);
        return new_p;
	}
	//modified by Wang in 2018/3/17
	bool InOurPenaltyArea(const CGeoPoint& p, const double buffer) {
        // rectangle penalty
        return (p.x() < -PARAM::Field::PITCH_LENGTH / 2 +
                PARAM::Field::PENALTY_AREA_DEPTH + buffer
                &&
                std::fabs(p.y()) <
                PARAM::Field::PENALTY_AREA_WIDTH / 2 + buffer);
    }
	//modified by Wang in 2018/3/17
	bool InTheirPenaltyArea(const CGeoPoint& p, const double buffer) {
            // rectanlge penalty
        return (p.x() >
                PARAM::Field::PITCH_LENGTH / 2 -
                PARAM::Field::PENALTY_AREA_DEPTH - buffer
                &&
                std::fabs(p.y()) <
                PARAM::Field::PENALTY_AREA_WIDTH / 2 + buffer);
    }


    bool InTheirPenaltyAreaWithVel(const PlayerVisionT& me, const double buffer) {
        CVector vel = me.Vel();
        CGeoPoint pos = me.Pos();
//        GDebugEngine::Instance()->gui_debug_x(pos + Polar2Vector(pow(vel.mod(), 2) / (2 * 400), vel.dir()));
        if (me.Vel().mod() < 30) return InTheirPenaltyArea(me.Pos(), buffer);
        if (InTheirPenaltyArea(pos + Polar2Vector(pow(vel.mod(), 2) / (2 * 400), vel.dir()), buffer)) {
            return true;
        }
        else return false;
    }

    bool IsInField(const CGeoPoint p, double buffer) {
        return (p.x() > buffer - PARAM::Field::PITCH_LENGTH / 2 && p.x() < PARAM::Field::PITCH_LENGTH / 2 - buffer &&
                p.y() > buffer - PARAM::Field::PITCH_WIDTH / 2 && p.y() < PARAM::Field::PITCH_WIDTH / 2 - buffer);
    }
    bool IsInFieldV2(const CGeoPoint p, double buffer) {
        return (IsInField(p, buffer) && !Utils::InOurPenaltyArea(p, buffer) && !Utils::InTheirPenaltyArea(p, buffer));
    }

	//modified by Wang in 2018/3/21
	CGeoPoint MakeOutOfOurPenaltyArea(const CGeoPoint& p, const double buffer) {
        if(WorldModel::Instance()->CurrentRefereeMsg() == "OurBallPlacement")
            return p;
        // rectangle penalty
        //右半禁区点
        if (p.y() > 0) {
            //距离禁区上边近，取上边投影
            if (-PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH - p.x() < PARAM::Field::PENALTY_AREA_WIDTH / 2 - p.y())
                return CGeoPoint(-PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH + buffer, p.y());
            //距离禁区右边近，取右边投影
            else return CGeoPoint(p.x(), PARAM::Field::PENALTY_AREA_WIDTH / 2 + buffer);
        }
        //左半禁区点
        else {
            //距离禁区上边近，取上边投影
            if (-PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH - p.x() < p.y() - (-PARAM::Field::PENALTY_AREA_WIDTH / 2) )
                return CGeoPoint(-PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH + buffer, p.y());
            //距离禁区左边近，取左边投影
            else return CGeoPoint(p.x(), -PARAM::Field::PENALTY_AREA_WIDTH / 2 - buffer);
        }
	}

	//modified by Wang in 2018/3/17
    CGeoPoint MakeOutOfTheirPenaltyArea(const CGeoPoint& p, const double buffer,const double dir) {
        // rectangle penalty
        if(WorldModel::Instance()->CurrentRefereeMsg() == "OurBallPlacement")
            return p;
        CGeoPoint newPoint = p;
        if(fabs(dir) < 1e4) {
            double normDir = Utils::Normalize(dir);
            double adjustStep = 2.0;
            CVector adjustVec = Polar2Vector(adjustStep, normDir);
            newPoint = newPoint + adjustVec;
            while(InTheirPenaltyArea(newPoint, buffer) && newPoint.x() < PARAM::Field::PITCH_LENGTH / 2)
                newPoint = newPoint + adjustVec;
            if(newPoint.x() > PARAM::Field::PITCH_LENGTH / 2)
                newPoint.setX(PARAM::Field::PITCH_LENGTH / 2);
            if(fabs(newPoint.y()) > PARAM::Field::PENALTY_AREA_WIDTH / 2 ||
                    (fabs(newPoint.y()) < PARAM::Field::PENALTY_AREA_WIDTH / 2 && fabs(newPoint.x()) < PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH))
                return newPoint;
        }

        newPoint = p;
        if(newPoint.x() > PARAM::Field::PITCH_LENGTH / 2)
            newPoint.setX(200);
        //右半禁区点
        if (newPoint.y() > 0) {
            //距离禁区下边近，取下边投影
            if (newPoint.x() - PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH < PARAM::Field::PENALTY_AREA_WIDTH / 2 - newPoint.y())
                return CGeoPoint(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH - buffer, newPoint.y());
            //距离禁区右边近，取右边投影
            else return CGeoPoint(newPoint.x(), PARAM::Field::PENALTY_AREA_WIDTH / 2 + buffer);
        }
        //左半禁区点
        else {
            //距离禁区下边近，取下边投影
            if (newPoint.x() - PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH < PARAM::Field::PENALTY_AREA_WIDTH / 2 + newPoint.y())
                return CGeoPoint(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH - buffer, newPoint.y());
            //距离禁区左边近，取左边投影
            else return CGeoPoint(newPoint.x(), -PARAM::Field::PENALTY_AREA_WIDTH / 2 - buffer);
        }
	}

    CGeoPoint MakeOutOfCircle(const CGeoPoint& center, const double radius, const CGeoPoint& target, const double buffer, const bool isBack, const CGeoPoint& mePos, const CVector adjustVec) {
        CGeoPoint p(target);
        CVector adjustDir;
        if(isBack) {
            adjustDir = mePos - target;
        }
        else if(adjustVec.x() < 1e4)
            adjustDir = adjustVec;
        else {
            adjustDir = target - center;
            if(adjustDir.mod() < PARAM::Vehicle::V2::PLAYER_SIZE / 2.0)
                adjustDir = mePos - target;
        }

        adjustDir = adjustDir / adjustDir.mod();
        double adjustUnit = 0.5;
        while(p.dist(center) < radius + buffer)
            p = p + adjustDir * adjustUnit;
        return p;
    }

    CGeoPoint MakeOutOfLongCircle(const CGeoPoint& seg_start, const CGeoPoint& seg_end, const double radius, const CGeoPoint& target, const double buffer, const CVector adjustVec) {
        CGeoSegment segment(seg_start, seg_end);
        CGeoPoint p(target);
        CGeoPoint nearPoint = (seg_start.dist(target) < seg_end.dist(target) ? seg_start : seg_end);
        CVector adjustDir= target - nearPoint;
        if(adjustDir.x() < 1e4)
            adjustDir = adjustVec;
        adjustDir = adjustDir / adjustDir.mod();
        double adjustUnit = 0.5;
        while(segment.dist2Point(p) < radius + buffer)
            p = p + adjustDir * adjustUnit;
        return p;
    }

    //针对门柱
    CGeoPoint MakeOutOfRectangle(const CGeoPoint &recP1, const CGeoPoint &recP2, const CGeoPoint &target, const double buffer) {
        double leftBound = min(recP1.x(), recP2.x());
        double rightBound = max(recP1.x(), recP2.x());
        double upperBound = max(recP1.y(), recP2.y());
        double lowerBound = min(recP1.y(), recP2.y());
        double middleY = (upperBound + lowerBound) / 2.0;
        double middleX = (leftBound + rightBound) / 2.0;

        CGeoPoint targetNew = target;
        if(targetNew.y() < upperBound + buffer &&
                targetNew.y() > lowerBound - buffer &&
                targetNew.x() > leftBound - buffer &&
                targetNew.x() < rightBound + buffer) {
            if(fabs(middleX) < PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::GOAL_DEPTH * 2.0 / 3.0) { //边门柱
                double xInside = copysign(min(fabs(leftBound), fabs(rightBound)), leftBound);
                double yInside = copysign(min(fabs(upperBound), fabs(lowerBound)), lowerBound);
                double yOutside = copysign(max(fabs(upperBound), fabs(lowerBound)), lowerBound);
                if(fabs(targetNew.x()) < fabs(xInside)) {
                    targetNew.setX(xInside - copysign(buffer, xInside));
                } else if(fabs(targetNew.y()) < fabs(yInside)) {
                    targetNew.setY(yInside - copysign(buffer, yInside));
                } else if(fabs(targetNew.y()) > fabs(yOutside)) {
                    targetNew.setY(yInside + copysign(buffer, yOutside));
                } else if(fabs(targetNew.y()) < fabs(middleY)) { //后面两种只针对虚拟门柱和仿真，实际不会出现
                    targetNew.setY(yInside - copysign(buffer, yInside));
                } else {
                    targetNew.setY(yInside + copysign(buffer, yOutside));
                }
            }
            else { //后门柱
                double xInside = copysign(min(fabs(leftBound), fabs(rightBound)), leftBound);
                if(fabs(targetNew.x()) < fabs(xInside)) {
                    targetNew.setX(xInside - copysign(buffer, xInside));
                } else if(targetNew.y() < lowerBound) {
                    targetNew.setY(lowerBound - buffer);
                } else if(targetNew.y() > upperBound) {
                    targetNew.setY(upperBound + buffer);
                } else if(targetNew.y() < 0) { //后面两种只针对虚拟门柱和仿真，实际不会出现
                    targetNew.setY(lowerBound - buffer);
                } else {
                    targetNew.setY(upperBound + buffer);
                }
            }
        }


        return targetNew;
    }


	CGeoPoint MakeOutOfCircleAndInField(const CGeoPoint& center,const double radius,const CGeoPoint& p,const double buffer)
	{
		const CVector p2c = p - center;
		const double dist = p2c.mod();
		if( dist > radius + buffer || dist < 0.01 ){ // 不在圆内
			return p;
		}
		CGeoPoint newPos(center + p2c * (radius + buffer) / dist);
		CGeoRectangle fieldRect(FieldLeft() + buffer,FieldTop() + buffer,FieldRight() - buffer,FieldBottom() - buffer);
		if( !fieldRect.HasPoint(newPos) ){ // 在场外,选择距离最近且不在圆内的场内点
			CGeoCirlce avoidCircle(center,radius + buffer);
			std::vector< CGeoPoint > intPoints;
			for(int i=0; i<4; ++i){
				CGeoLine fieldLine(fieldRect._point[i % 4],fieldRect._point[(i+1) % 4]);
				CGeoLineCircleIntersection fieldLineCircleInt(fieldLine,avoidCircle);
				if( fieldLineCircleInt.intersectant() ){
					intPoints.push_back(fieldLineCircleInt.point1());
					intPoints.push_back(fieldLineCircleInt.point2());
				}
			}
			double minDist = 1000.0;
			CGeoPoint minPoint = newPos;
			for( unsigned int i=0; i<intPoints.size(); ++i ){
				double cDist = p.dist(intPoints[i]);
				if( cDist < minDist ){
					minDist = cDist;
					minPoint = intPoints[i];
				}
			}
			return minPoint;
		}

		return newPos; // 圆外距离p最近的点
	}


	bool PlayerNumValid(int num)
	{
		if (num>=0 && num<PARAM::Field::MAX_PLAYER){
			return true;
		}
		else{
			return false;
		}
	}
	// 给定一个球门线上的点, 一个方向(角度), 找出一个在禁区外防守该方向的
	// 离禁区线较近的点
	CGeoPoint GetOutSidePenaltyPos(double dir, double delta, const CGeoPoint targetPoint)
	{	
		//double delta = PARAM::Field::MAX_PLAYER_SIZE + 1.5;
		CGeoPoint pInter = GetInterPos(dir, targetPoint);
		CGeoPoint pDefend = pInter + Polar2Vector(delta, dir);
		return pDefend;
	}

	CGeoPoint GetOutTheirSidePenaltyPos(double dir, double delta, const CGeoPoint& targetPoint) {
        CGeoPoint pInter = GetTheirInterPos(dir, targetPoint);
		return (pInter + Polar2Vector(delta, dir));
	}

	CGeoPoint GetInterPos(double dir, const CGeoPoint targetPoint) {
		using namespace PARAM::Field;
        // rectangle penalty
        CGeoPoint p1(-PITCH_LENGTH / 2, -PENALTY_AREA_WIDTH / 2);//禁区左下
        CGeoPoint p2(-PITCH_LENGTH / 2 + PENALTY_AREA_DEPTH, -PENALTY_AREA_WIDTH / 2);//禁区左上
        CGeoPoint p3(-PITCH_LENGTH / 2 + PENALTY_AREA_DEPTH, PENALTY_AREA_WIDTH / 2);//禁区右上
        CGeoPoint p4(-PITCH_LENGTH / 2, PENALTY_AREA_WIDTH / 2);//禁区右下
        CGeoLine line1(p1, p2);//禁区左边线
        CGeoLine line2(p2, p3);//禁区前边线
        CGeoLine line3(p3, p4);//禁区右边线
        CGeoLine dirLine(targetPoint, dir);

        CGeoLineLineIntersection inter1(line1, dirLine);
        CGeoLineLineIntersection inter2(line2, dirLine);
        CGeoLineLineIntersection inter3(line3, dirLine);

        CGeoPoint inter_p1 = inter1.IntersectPoint();
        GDebugEngine::Instance()->gui_debug_x(inter_p1, 3);//黄
        CGeoPoint inter_p2 = inter2.IntersectPoint();
        GDebugEngine::Instance()->gui_debug_x(inter_p2, 4);//绿
        CGeoPoint inter_p3 = inter3.IntersectPoint();
        GDebugEngine::Instance()->gui_debug_x(inter_p3, 9);//黑
        CGeoPoint returnPoint = targetPoint;//返回值

        //if (targetPoint.x() >= -PITCH_LENGTH / 2 + PENALTY_AREA_DEPTH) {
        if (targetPoint.y() <= 0) {//case 1
            if (InOurPenaltyArea(inter_p1, 10)) returnPoint = inter_p1;
            else returnPoint = inter_p2;
        }
        else {//case 2
            if (InOurPenaltyArea(inter_p3, 10)) returnPoint = inter_p3;
            else returnPoint = inter_p2;//随便选的
        }
        GDebugEngine::Instance()->gui_debug_x(returnPoint, 0);
        CGeoPoint p0(-PITCH_LENGTH / 2, 0);
        GDebugEngine::Instance()->gui_debug_line(returnPoint, p0, 0);
        return returnPoint;
	}
    CGeoPoint GetTheirInterPos(double dir, const CGeoPoint& targetPoint) {
        using namespace PARAM::Field;
        // rectangle penalty
        CGeoPoint p1(PITCH_LENGTH / 2, -PENALTY_AREA_WIDTH / 2);//禁区左上
        CGeoPoint p2(PITCH_LENGTH / 2 - PENALTY_AREA_DEPTH, -PENALTY_AREA_WIDTH / 2);//禁区左下
        CGeoPoint p3(PITCH_LENGTH / 2 - PENALTY_AREA_DEPTH, PENALTY_AREA_WIDTH / 2);//禁区右下
        CGeoPoint p4(PITCH_LENGTH / 2, PENALTY_AREA_WIDTH / 2);//禁区右上
        CGeoLine line1(p1, p2);//禁区左边线
        CGeoLine line2(p2, p3);//禁区下边线
        CGeoLine line3(p3, p4);//禁区右边线
        CGeoLine dirLine(targetPoint, dir);

        CGeoLineLineIntersection inter1(line1, dirLine);
        CGeoLineLineIntersection inter2(line2, dirLine);
        CGeoLineLineIntersection inter3(line3, dirLine);

        CGeoPoint inter_p1 = inter1.IntersectPoint();
        CGeoPoint inter_p2 = inter2.IntersectPoint();
        CGeoPoint inter_p3 = inter3.IntersectPoint();
        CGeoPoint returnPoint = targetPoint;//返回值

        if (targetPoint.x() >= PITCH_LENGTH / 2 - PENALTY_AREA_DEPTH) {
            if (targetPoint.y() <= 0) {//case 1
                if (InOurPenaltyArea(inter_p1, 0)) return inter_p1;
                else return p2;//随便选的
            }
            else {//case 2
                if (InOurPenaltyArea(inter_p3, 0)) return inter_p3;
                else return p3;//随便选的
            }
        }
        else if (std::fabs(targetPoint.y()) <= PENALTY_AREA_WIDTH / 2) {//case 3
            if (InOurPenaltyArea(inter_p2, 0)) return inter_p2;
            else return p2;//随便选的
        }
        else {
            if (targetPoint.y() <= 0) {//case 4
                if (InOurPenaltyArea(inter_p1, 0)) return inter_p1;
                else if (InOurPenaltyArea(inter_p2, 0)) return inter_p2;
                else return p2;//随便选的
            }
            else {//case 5
                if (InOurPenaltyArea(inter_p2, 0)) return inter_p2;
                else if (InOurPenaltyArea(inter_p3, 0)) return inter_p3;
                else return p3;//随便选的
            }
        }
	}
	float SquareRootFloat(float number) {
		long i;
		float x, y;
		const float f = 1.5F;

		x = number * 0.5F;
		y  = number;
		i  = * ( long * ) &y;
		i  = 0x5f3759df - ( i >> 1 );
		y  = * ( float * ) &i;
		y  = y * ( f - ( x * y * y ) );
		y  = y * ( f - ( x * y * y ) );
		return number * y;
	}
	bool canGo(const CVisionModule* pVision, const int vecNumber, const CGeoPoint& target, const int flags, const double avoidBuffer)//判断是否可以直接到达目标点
	{
		static bool _canGo = true;
		const CGeoPoint& vecPos = pVision->ourPlayer(vecNumber).Pos();
		CGeoSegment moving_seg(vecPos, target);
		const double minBlockDist2 = (PARAM::Field::MAX_PLAYER_SIZE/2 + avoidBuffer) * (PARAM::Field::MAX_PLAYER_SIZE/2 + avoidBuffer);
		for( int i=0; i<PARAM::Field::MAX_PLAYER * 2; ++i ){ // 看路线上有没有人
			if( i == vecNumber || !pVision->allPlayer(i).Valid()){
				continue;
			}
			const CGeoPoint& obs_pos = pVision->allPlayer(i).Pos();
			if( (obs_pos - target).mod2() < minBlockDist2 ){
				_canGo = false;
				return _canGo;
			}
			CGeoPoint prj_point = moving_seg.projection(obs_pos);
			if( moving_seg.IsPointOnLineOnSegment(prj_point) ){
				const double blockedDist2 = (obs_pos - prj_point).mod2();
				if( blockedDist2 < minBlockDist2 && blockedDist2 < (obs_pos - vecPos).mod2()){
					_canGo = false;
					return _canGo;
				}
			}
		}
		if( _canGo && (flags & PlayerStatus::DODGE_BALL) ){ // 躲避球
			const CGeoPoint& obs_pos = pVision->ball().Pos();
			CGeoPoint prj_point = moving_seg.projection(obs_pos);
			if( obs_pos.dist(prj_point) < avoidBuffer + PARAM::Field::BALL_SIZE && moving_seg.IsPointOnLineOnSegment(prj_point) ){
				_canGo = false;
				return _canGo;
			}
		}
		if( _canGo && (flags & PlayerStatus::DODGE_OUR_DEFENSE_BOX) ){ // 避免进入本方禁区
			if( PARAM::Rule::Version == 2003 ){	// 2003年的规则禁区是矩形
				CGeoRectangle defenseBox(-PARAM::Field::PITCH_LENGTH/2, -PARAM::Field::PENALTY_AREA_WIDTH/2 - avoidBuffer, -PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::PENALTY_AREA_WIDTH + avoidBuffer, PARAM::Field::PENALTY_AREA_WIDTH/2 + avoidBuffer);
				CGeoLineRectangleIntersection intersection(moving_seg, defenseBox);
				if( intersection.intersectant() ){
					if( moving_seg.IsPointOnLineOnSegment(intersection.point1()) || moving_seg.IsPointOnLineOnSegment(intersection.point2())){
						_canGo = false; // 要经过禁区
						return _canGo;
					}
				}
			}else if (PARAM::Rule::Version == 2004) { // 2004年的规则禁区是半圆形
				CGeoCirlce defenseBox(CGeoPoint(-PARAM::Field::PITCH_LENGTH/2, 0), PARAM::Field::PENALTY_AREA_WIDTH/2 + avoidBuffer);
				CGeoLineCircleIntersection intersection(moving_seg, defenseBox);
				if( intersection.intersectant() ){
					if( moving_seg.IsPointOnLineOnSegment(intersection.point1()) || moving_seg.IsPointOnLineOnSegment(intersection.point2())){
						_canGo = false; // 要经过禁区
						return _canGo;
					}
				}
			}
            else {// 2018年的规则禁区是矩形
				CGeoRectangle defenseBox(-PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH + avoidBuffer, -PARAM::Field::PENALTY_AREA_WIDTH / 2 - avoidBuffer, -PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::PENALTY_AREA_WIDTH / 2 + avoidBuffer);
				CGeoLineRectangleIntersection intersection(moving_seg, defenseBox);
				if (intersection.intersectant()) {
					if (moving_seg.IsPointOnLineOnSegment(intersection.point1()) || moving_seg.IsPointOnLineOnSegment(intersection.point2())) {
						_canGo = false; // 要经过禁区
						return _canGo;
					}
				}
			}
		}
		return _canGo;
	}

    bool isValidFlatPass(const CVisionModule *pVision, CGeoPoint start, CGeoPoint end, bool isShoot, bool ignoreCloseEnemy, bool ignoreTheirGuard){
        // 判断能否传球的角度限制
        static const double CLOSE_ANGLE_LIMIT = 8 * PARAM::Math::PI / 180;
        static const double FAR_ANGLE_LIMIT = 12 * PARAM::Math::PI / 180;
        static const double CLOSE_THRESHOLD = 50;
        static const double THEIR_ROBOT_INTER_THREADHOLD = 30;
        static const double SAFE_DIST = 50;
        static const double CLOSE_ENEMY_DIST = 50;

        bool valid = true;
//        if(!isShoot){
//            // 使用扇形进行计算
//            CVector passLine = end - start;
//            double passDir = passLine.dir();
//            for (int i = 0; i < PARAM::Field::MAX_PLAYER; ++i) {
//                if(pVision->theirPlayer(i).Valid()){
//                    if(ignoreCloseEnemy && pVision->theirPlayer(i).Pos().dist(start) < CLOSE_ENEMY_DIST) continue;
//                    if(ignoreTheirGuard && Utils::InTheirPenaltyArea(pVision->theirPlayer(i).Pos(), 30)) continue;
//                    CGeoPoint enemyPos = pVision->theirPlayer(i).Pos();
//                    CVector enemyLine = enemyPos - start;
//                    double enemyDir = enemyLine.dir();
//                    // 计算敌方车与传球线路的差角
//                    double diffAngle = fabs(enemyDir - passDir);
//                    diffAngle = diffAngle > PARAM::Math::PI ? 2*PARAM::Math::PI - diffAngle : diffAngle;
//                    // 计算补偿角
//                    double compensateAngle = fabs(atan2(PARAM::Vehicle::V2::PLAYER_SIZE + PARAM::Field::BALL_SIZE, start.dist(enemyPos)));
//        //            qDebug() << "compensate angle: " << enemyPos.x() << enemyPos.y() << enemyDir << passDir << compensateAngle;
//                    double distanceToEnemy = start.dist(enemyPos);
//                    double limit_angle = distanceToEnemy > CLOSE_THRESHOLD ? FAR_ANGLE_LIMIT : CLOSE_ANGLE_LIMIT;
//        //            qDebug() << "limit_angle: " << i << diffAngle << distanceToEnemy << (distanceToEnemy > CLOSE_THRESHOLD) << limit_angle << (diffAngle < limit_angle);
//                    if(diffAngle - compensateAngle < limit_angle && enemyPos.dist(start) < end.dist(start) + SAFE_DIST){
//                        valid = false;
//                        break;
//                    }
//                }
//            }
//        }
        // 使用平行线进行计算，解决近距离扇形计算不准问题
        CGeoSegment BallLine(start, end);
            for(int i = 0; i < PARAM::Field::MAX_PLAYER; i++) {
                if(!pVision->theirPlayer(i ).Valid()) continue;
                if(ignoreCloseEnemy && pVision->theirPlayer(i).Pos().dist(start) < CLOSE_ENEMY_DIST) continue;
                if(ignoreTheirGuard && Utils::InTheirPenaltyArea(pVision->theirPlayer(i).Pos(), 30)) continue;
                CGeoPoint targetPos = pVision->theirPlayer(i ).Pos();
                double dist = BallLine.dist2Point(targetPos);
                if(dist < THEIR_ROBOT_INTER_THREADHOLD){
                    valid = false;
                    break;
                }
            }
        return valid;
    }

    bool isValidChipPass(const CVisionModule *pVision, CGeoPoint start, CGeoPoint end){
        // 判断能否传球的角度限制
        static const double ANGLE_LIMIT = 5 * PARAM::Math::PI / 180;
        static const double CLOSE_SAFE_DIST = 50;
        static const double FAR_SAFE_DIST = 50;
        static const double FRONT_SAFE_DIST = 30;

        bool valid = true;
        // 使用扇形进行计算
        CVector passLine = end - start;
        double passDir = passLine.dir();
        for (int i = 0; i < PARAM::Field::MAX_PLAYER; ++i) {
            if(pVision->theirPlayer(i).Valid()){
                CGeoPoint enemyPos = pVision->theirPlayer(i).Pos();
                CVector enemyLine = enemyPos - start;
                double enemyDir = enemyLine.dir();
                // 计算敌方车与传球线路的差角
                double diffAngle = fabs(enemyDir - passDir);
                diffAngle = diffAngle > PARAM::Math::PI ? 2*PARAM::Math::PI - diffAngle : diffAngle;
                // 计算补偿角
                double compensateAngle = fabs(atan2(PARAM::Vehicle::V2::PLAYER_SIZE + PARAM::Field::BALL_SIZE, start.dist(enemyPos)));
    //            qDebug() << "compensate angle: " << enemyPos.x() << enemyPos.y() << enemyDir << passDir << compensateAngle;
                if(diffAngle - compensateAngle < ANGLE_LIMIT && ((enemyPos.dist(start) < end.dist(start) + FAR_SAFE_DIST && enemyPos.dist(start) > end.dist(start) - CLOSE_SAFE_DIST) || enemyPos.dist(start) < FRONT_SAFE_DIST)){
                    valid = false;
                    break;
                }
            }
        }
        return valid;
    }
}

