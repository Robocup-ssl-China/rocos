//Solution to the Cubic Equation.
//Created by Yihua Tan.Nov.12th,2011.

#ifndef QUADRATIC_EQUATION_H
#define QUADRATIC_EQUATION_H

#include "QuarticEquation.h"

class CQuarticEquation;

class CPoint {
public:
	CPoint(const double& x1 = 0, const double& y1 = 0) : x(x1), y(y1) {}
	CPoint(CPoint& p1) {
		x = p1.x;
		y = p1.y;
	}
	double x;
	double y;
private:
};

class CCar {
public:
	CCar(const CPoint& p, const double& vx, const double& vy, const double& ax, const double& ay, const double& r);
	CPoint point;
	double velX;
	double velY;
	double accX;
	double accY;
	double radius;
private:
};

class CQuadraticEquation {
public:
	CQuadraticEquation();
	CQuadraticEquation(const CCar& car, const double& t1, const double& t2);
	void init(const CCar& car, const double& t1, const double& t2);
	double getradius(){return radius;}
	~CQuadraticEquation(){}
	friend CQuarticEquation operator-(CQuadraticEquation& q1, CQuadraticEquation& q2);
public:
	//Physical information
	double pX;
	double pY;
	double pVelX;
	double pVelY;
	double pAccX;
	double pAccY;
	//x
	double ax;
	double bx;
	double cx;
	//y
	double ay;
	double by;
	double cy;
	//time
	double tBegin;
	double tEnd;
	//radius
	double radius;
};

#endif //QUADRATIC_EQUATION_H