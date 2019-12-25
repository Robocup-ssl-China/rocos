#include "QuarticEquation.h"
#include <cmath>
#include <iostream>

const double precision_dichotomy = 0.01; // this precision needn't to be too small

CCubicEquation CQuarticEquation::derivation(void)
{
	return CCubicEquation(4*a, 3*b, 2*c, d, tBegin, tEnd);
}

double CQuarticEquation::fx(double x)
{
	return( a*pow(x,4)+b*pow(x,3)+c*pow(x,2)+d*x+e );
}

const double CQuarticEquation::getRoot(void)
{
	double y1 = fx(extreme[0]);
	double y2 = fx(extreme[1]);
	double y3 = fx(extreme[2]);
	double y4 = fx(extreme[3]);
	double y5 = fx(extreme[4]);
	if(y1>0 && y2>0 && y3>0 && y4>0 && y5>0)
		return -1;
	else if(y1 > 0)
	{
		if(y2 < 0)
		{
			return dichotomy(extreme[0], extreme[1]);
		}
		else if(y3 < 0)
		{
			return dichotomy(extreme[1], extreme[2]);
		}
		else if(y4 < 0)
		{
			return dichotomy(extreme[2], extreme[3]);
		}
		else if(y5 < 0)
		{
			return dichotomy(extreme[3], extreme[4]);
		}
	}
	else
	{
		//if fx(lower_limit)<0, then return lower_limit directly. Actually, lower_limit is 0.
		return extreme[0];
	}

	return 0.0;
}

//dichotomy(double x1, double x2) is responsible for returning a root between x1 and x2, regardless of fx(x1) is positive or negative.
//assume that at the beginning fx(x1) and fx(x2) have opposite sign
double CQuarticEquation::dichotomy(double x1, double x2)
{
	double xm = 0.0;
	if((x1 <= x2) && (!isSameSign(fx(x1),fx(x2))))
	{
		while(true)
		{
			xm = (x1+x2)/2;
			if(fx(xm) < precision_dichotomy)
			{
				return xm;
			}
			if(!isSameSign(fx(x1),fx(xm)))
			{
				x2 = xm;
			}
			else if(!isSameSign(fx(x2),fx(xm)))
			{
				x1 = xm;
			}
		}
	}
	else
	{
		std::cout<<"Error! In ""double CQuarticEquation::dichotomy(double x1, double x2)"" "<<"x1 > x2 OR fx(x1) and fx(x2) have the same sign"<<std::endl;
		return -1;
	}
	return 0;
}

const double CQuarticEquation::isSameSign(double y1, double y2)const
{
	if((y1>=0 && y2>=0) || (y1<0 && y2<0))
		return 1;
	return 0;
}

const double CQuarticEquation::getIsCrash(void)const
{
	return isCrash;
}

const double CQuarticEquation::getExtreme(int i)
{
	return extreme[i];
}

