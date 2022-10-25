#include "CubicEquation.h"
#include <cmath>
#include <iostream>


using namespace std;

const double precision = 0.0000001;

root::root()
{
	x[0][0] = 0;
	x[1][0] = 0;
	x[2][0] = 0;
	x[0][1] = 1;
	x[1][1] = 1;
	x[2][1] = 1;
}
root::root(const double& xx1, const double& xx2, const double& xx3)
{
	x[0][0] = xx1;
	x[1][0] = xx2;
	x[2][0] = xx3;
	x[0][1] = 0;
	x[1][1] = 0;
	x[2][1] = 0;
}

void root::print(void)const
{
	cout<<"x1 = "<<x[0][0]<<endl;
	cout<<"x2 = "<<x[1][0]<<endl;
	cout<<"x3 = "<<x[2][0]<<endl;
}

void root::test(const double& a, const double& b, const double& c, const double& d, const double& xmin, const double& xmax)const
{
	double y1 = a*x[0][0]*x[0][0]*x[0][0]+b*x[0][0]*x[0][0]+c*x[0][0]+d;
	double y2 = a*x[1][0]*x[1][0]*x[1][0]+b*x[1][0]*x[1][0]+c*x[1][0]+d;
	double y3 = a*x[2][0]*x[2][0]*x[2][0]+b*x[2][0]*x[2][0]+c*x[2][0]+d;

	if(abs(y1) < precision)
		cout<<"x1 = "<<x[0][0]<<" is a root of the cubic equation!"<<endl;
	else
		cout<<"x1 = "<<x[0][0]<<" is NOT a root of the cubic equation!"<<endl;
	if(abs(y2) < precision)
		cout<<"x2 = "<<x[1][0]<<" is a root of the cubic equation!"<<endl;
	else
		cout<<"x2 = "<<x[1][0]<<" is NOT a root of the cubic equation!"<<endl;
	if(abs(y3) < precision)
		cout<<"x3 = "<<x[2][0]<<" is a root of the cubic equation!"<<endl;
	else
		cout<<"x3 = "<<x[2][0]<<" is NOT a root of the cubic equation!"<<endl;
}



CCubicEquation::CCubicEquation(double a1, double b1, double c1, double d1, double xmin1, double xmax1)
{
	a0 = a1;
	b0 = b1;
	c0 = c1;
	d0 = d1;
	xmin = xmin1;
	xmax = xmax1;

	normalize();
	
}

void CCubicEquation::solveCubicEquation(void)
{
	if(a == 0)
	{
		//a==0,turn to be a quadratic equation!
		if(b == 0)
		{
			if(c == 0)
			{
				root_CubicEquation.x[0][0]=0;//no real root
				root_CubicEquation.x[1][0]=0;//no real root
				root_CubicEquation.x[2][0]=0;//no real root
				root_CubicEquation.x[0][1]=1;//FLAG: not a real root
				root_CubicEquation.x[1][1]=1;//FLAG: not a real root
				root_CubicEquation.x[2][1]=1;//FLAG: not a real root
			}
			else
			{
				//b==0,c!=0,turn to be a linear equation!
				root_CubicEquation.x[0][0] = -d0/c0;//a real root
				root_CubicEquation.x[1][0]=0;//not a real root
				root_CubicEquation.x[2][0]=0;//not a real root
				root_CubicEquation.x[0][1]=0;//FLAG: is a real root
				root_CubicEquation.x[1][1]=1;//FLAG: not a real root
				root_CubicEquation.x[2][1]=1;//FLAG: not a real root
			}
		}
		else
		{
			double delta(c*c-4*b*d);
			root_CubicEquation.x[0][0] = (-c+sqrt(delta))/(2*b);//a real root
			root_CubicEquation.x[1][0] = (-c-sqrt(delta))/(2*b);//a real root
			root_CubicEquation.x[2][0] = 0;//not a real root
			root_CubicEquation.x[0][1] = 0;//FLAG: is a real root
			root_CubicEquation.x[1][1] = 0;//FLAG: is a real root
			root_CubicEquation.x[2][1] = 1;//FLAG: not a real root
		}
	}
	else
	{
		//a cubic equation!
		double disA = b*b-3*a*c;
		double disB = b*c-9*a*d;
		double disC = c*c-3*b*d;
		double disT = disB*disB-4*disA*disC;
        double x1(0)/*, x2(0), x3(0)*/;

		if(disA == 0 && disB == 0)
		{
			root_CubicEquation.x[0][0] = -b/(3*a);//a real root
			root_CubicEquation.x[1][0] = x1;//a real root
			root_CubicEquation.x[2][0] = x1;//a real root
			root_CubicEquation.x[0][1] = 0;//FLAG: is a real root
			root_CubicEquation.x[1][1] = 0;//FLAG: is a real root
			root_CubicEquation.x[2][1] = 0;//FLAG: is a real root
		}

		else if(disT > 0)
		{
			double y1 = disA*b + 3.0*a*(-disB+sqrt(disT))/2.0;
			double y2 = disA*b + 3.0*a*(-disB-sqrt(disT))/2.0;
			double y1_cub,y2_cub;
			if(y1<0)
				y1_cub=-pow(-y1,1.0/3.0);
			else
				y1_cub=pow(y1,1.0/3.0);
			if(y2<0)
				y2_cub=-pow(-y2,1.0/3.0);
			else
				y2_cub=pow(y2,1.0/3.0);

			root_CubicEquation.x[0][0] = (-b-y1_cub-y2_cub)/(3*a);//a real root
			root_CubicEquation.x[1][0] = 0;//not a real root
			root_CubicEquation.x[2][0] = 0;//not a real root
			root_CubicEquation.x[0][1] = 0;//FLAG: is a real root
			root_CubicEquation.x[1][1] = 1;//FLAG: not a real root
			root_CubicEquation.x[2][1] = 1;//FLAG: not a real root
		}

		else if(disT == 0)
		{
			double k = disB/disA;
			root_CubicEquation.x[0][0] = -b/a+k;//a real root
			root_CubicEquation.x[1][0] = -k/2;//a real root
			root_CubicEquation.x[2][0] = -k/2;//a real root
			root_CubicEquation.x[0][1] = 0;//FLAG: is a real root
			root_CubicEquation.x[1][1] = 0;//FLAG: is a real root
			root_CubicEquation.x[2][1] = 0;//FLAG: is a real root
		}

		else if(disT < 0)
		{
			double t = (2*disA*b-3*a*disB)/(2*pow(disA,1.5));
			double theta = acos(t);
			root_CubicEquation.x[0][0] = (-b-2*sqrt(disA)*cos(theta/3))/(3.0*a);//a real root
			root_CubicEquation.x[1][0] = (-b+sqrt(disA)*(cos(theta/3)+sqrt(3.0)*sin(theta/3)))/(3*a);//a real root
			root_CubicEquation.x[2][0] = (-b+sqrt(disA)*(cos(theta/3)-sqrt(3.0)*sin(theta/3)))/(3*a);//a real root
			root_CubicEquation.x[0][1] = 0;//FLAG: is a real root
			root_CubicEquation.x[1][1] = 0;//FLAG: is a real root
			root_CubicEquation.x[2][1] = 0;//FLAG: is a real root
		}
	}

	//sort the root from small to large,for the sake of convenience when looking for the root of quartic equation with Dichotomy
	sort();
}

void CCubicEquation::sort(void)
{
	//sort from small to large
	for(int i=0; i<3; i++)
	{
		for(int j=i+1; j<3; j++)
		{
			if(root_CubicEquation.x[i][0] > root_CubicEquation.x[j][0])
			{
				double temp1 = root_CubicEquation.x[i][0];
				double temp2 = root_CubicEquation.x[i][1];
				root_CubicEquation.x[i][0] = root_CubicEquation.x[j][0];
				root_CubicEquation.x[j][0] = temp1;
				root_CubicEquation.x[i][1] = root_CubicEquation.x[j][1];
				root_CubicEquation.x[j][1] = temp2;
			}
		}
	}
}

double CCubicEquation::findMax(void)const
{
	double a0_abs=abs(a0), b0_abs=abs(b0), c0_abs=abs(c0), d0_abs=abs(d0);
	double max = a0_abs;
	if(max < b0_abs)
		max = b0_abs;
	if(max < c0_abs)
		max = c0_abs;
	if(max < d0_abs)
		max = d0_abs;
	return max;
}


void CCubicEquation::normalize(void)
{
	double base = findMax();
	a = a0/base;
	b = b0/base;
	c = c0/base;
	d = d0/base;
	//If a is too small, the roots of cubic equation will probably overfloat.
	//So when a is smaller than precision, set it to 0.
	//That is to say, treat the cubic equation as a quadratic equation
	//It'll introduce deviation, but generally it meets the requirements
	if(a < precision)
		a = 0;
}
void CCubicEquation::checkRoot(void)const
{
	root_CubicEquation.test(a,b,c,d,xmin,xmax);
}

const root& CCubicEquation::getRoot(void)
{
	return root_CubicEquation;
}


