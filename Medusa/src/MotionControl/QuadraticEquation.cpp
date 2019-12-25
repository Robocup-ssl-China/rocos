//Created by Yihua Tan.Nov.12th,2011.
//Modified by Yihua Tan.Nov.25th.2011.
//Modified by YYS 2014/03/07

#include "QuadraticEquation.h"
#include "CubicEquation.h"
#include "QuarticEquation.h"
#include <cmath>
#include <iostream>

const double radius_player = 0.3;//radius of robots.The value should be modified according to practical test.

CQuadraticEquation::CQuadraticEquation() {
	pX = 0.0f;
	pY = 0.0f;
	pVelX = 0.0f;
	pVelY = 0.0f;
	pAccX = 0.0f;
	pAccY = 0.0f;

	ax = 0.0f;
	bx = 0.0f;
	cx = 0.0f;

	ay = 0.0f;
	by = 0.0f;
	cy = 0.0f;

	tBegin = 0.0f;
	tEnd = 0.0f;

	radius = 0.0f;
}


CQuadraticEquation::CQuadraticEquation(const CCar& car, const double& t1, const double& t2) {
	pX = car.point.x;
	pY = car.point.y;
	pVelX = car.velX;
	pVelY = car.velY;
	pAccX = car.accX;
	pAccY = car.accY;

	ax = 0.5 * car.accX;
	bx = car.velX;
	cx = car.point.x;

	ay = 0.5 * car.accY;
	by = car.velY;
	cy = car.point.y;

	tBegin = t1;
	tEnd = t2;

	radius = car.radius;
}

void CQuadraticEquation::init(const CCar& car, const double& t1, const double& t2) {
	pX = car.point.x;
	pY = car.point.y;
	pVelX = car.velX;
	pVelY = car.velY;
	pAccX = car.accX;
	pAccY = car.accY;

	ax = 0.5 * car.accX;
	bx = car.velX;
	cx = car.point.x;

	ay = 0.5 * car.accY;
	by = car.velY;
	cy = car.point.y;

	tBegin = t1;
	tEnd = t2;

	radius = car.radius;
}

CQuarticEquation operator-(CQuadraticEquation& q1, CQuadraticEquation& q2) {
	double a = 0.25 * (pow((q1.pAccX - q2.pAccX), 2) + pow((q1.pAccY - q2.pAccY), 2));
	double b = (q1.pVelX - q2.pVelX) * (q1.pAccX - q2.pAccX) + (q1.pVelY - q2.pVelY) * (q1.pAccY - q2.pAccY);
	double c = (q1.pX - q2.pX) * (q1.pAccX - q2.pAccX) + (q1.pY - q2.pY) * (q1.pAccY - q2.pAccY) + pow((q1.pVelX - q2.pVelX), 2) + pow((q1.pVelY - q2.pVelY), 2);
	double d = 2 * ((q1.pX - q2.pX) * (q1.pVelX - q2.pVelX) + (q1.pY - q2.pY) * (q1.pVelY - q2.pVelY));
	double e = pow((q1.pX - q2.pX), 2) + pow((q1.pY - q2.pY), 2) - pow(q1.getradius() + q2.getradius(), 2);
	
	//calculate the intersection of the two ranges
	double tBegin, tEnd;

	if(q1.tBegin < q2.tBegin)
		tBegin = q2.tBegin;
	else
		tBegin = q1.tBegin;
	if(q1.tEnd > q2.tEnd)
		tEnd = q2.tEnd;
	else
		tEnd = q1.tEnd;
	//Because in the formula " a*t^3+b*t^2+c*t+d=0 ",the 't' defaults to starting at 0!
	tEnd = tEnd - tBegin;
	tBegin = 0;

	CQuarticEquation quartic(a, b, c, d, e, tBegin, tEnd);
	
	if(tEnd > 0) {//The two ranges has intersection.
		CCubicEquation cubic = quartic.derivation();
		
		cubic.solveCubicEquation();

		//calculate the extreme values
		//if the root is not valid, set the corresponding extreme value to 0
		quartic.extreme[0] = tBegin;
		quartic.extreme[4] = tEnd;

		double extremey[5] = {0, 0, 0, 0, 0};
		extremey[0] = quartic.fx(tBegin);
		extremey[4] = quartic.fx(tEnd);


		if((cubic.getRoot().getX(0, 1) == 0.0) && cubic.getRoot().getX(0, 0) >= tBegin && cubic.getRoot().getX(0, 0) <= tEnd) {
			extremey[1] = quartic.fx(cubic.getRoot().getX(0, 0));
			quartic.extreme[1] = cubic.getRoot().getX(0, 0);
		}
		if((cubic.getRoot().getX(1, 1) == 0.0) && cubic.getRoot().getX(1, 0) >= tBegin && cubic.getRoot().getX(1, 0) <= tEnd) {
			extremey[2] = quartic.fx(cubic.getRoot().getX(1, 0));
			quartic.extreme[2] = cubic.getRoot().getX(1, 0);
		}
		if((cubic.getRoot().getX(2, 1) == 0.0) && cubic.getRoot().getX(2, 0) >= tBegin && cubic.getRoot().getX(2, 0) <= tEnd) {
			extremey[3] = quartic.fx(cubic.getRoot().getX(2, 0));
			quartic.extreme[3] = cubic.getRoot().getX(2, 0);
		}

		double isCrash = 0;//NOT crashed: return 0; crashed: return 1
		for(int i = 0; i < 5;i++) {
			if(extremey[i] < 0) {
				isCrash = 1;
				break;
			}
		}
		quartic.isCrash = isCrash;
	}
	else { // tEnd <= 0  //The two ranges has NO intersection!
		quartic.isCrash = 0;
	}
	return quartic;
}

CCar::CCar(const CPoint& p, const double& vx, const double& vy, const double& ax, const double& ay, const double& r) {
	point = p;
	velX = vx;
	velY = vy;
	accX = ax;
	accY = ay;
	radius = r;
}