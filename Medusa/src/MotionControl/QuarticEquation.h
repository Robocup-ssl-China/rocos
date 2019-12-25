//Solution to the Cubic Equation.
//Created by Yihua Tan.Nov.12th,2011.

#ifndef QUARTIC_EQUATION_H
#define QUARTIC_EQUATION_H

#include "CubicEquation.h"
#include "QuadraticEquation.h"

class CQuadraticEquation;

class CQuarticEquation
{
public:
	//t begin at 0
	CQuarticEquation(double a1, double b1, double c1, double d1, double e1,double tb, double te):a(a1), b(b1), c(c1), d(d1), e(e1), tBegin(0), tEnd(te-tb)
	{
		isCrash = -1;
		for(int i=0; i<5; i++)
		{
			extreme[i] = 0;
		}
	}
	CCubicEquation derivation(void);
	double fx(double x);
	const double getRoot(void);
	const double getExtreme(int i);
	const double getIsCrash(void)const;
	friend CQuarticEquation operator-(CQuadraticEquation& q1, CQuadraticEquation& q2);//ugly! how to improve it?!

private:
	const double isSameSign(double y1, double y2)const;
	double dichotomy(double x1, double x2);
	double isCrash;
	double a;
	double b;
	double c;
	double d;
	double e;
	//time
 	double tBegin;
 	double tEnd;
	double extreme[5];
};


#endif //QUARTIC_EQUATION_H