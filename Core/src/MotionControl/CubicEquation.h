//Solution to the Cubic Equation.
//Created by Yihua Tan.Nov.12th,2011.

#ifndef CUBIC_EQUATION_H
#define CUBIC_EQUATION_H



class CCubicEquation;

class root
{
public:
	root();
	root(const double& xx1, const double& xx2, const double& xx3);
	void print(void)const;
	void test(const double& a, const double& b, const double& c, const double& d, const double& xmin, const double& xmax)const;
	friend CCubicEquation;
	const double getX(const int& i, const int& j)const
	{
		if((i<3 && i>=0) &&(j<2 && j>=0))
			return x[i][j];
		else
			return 0;
	}
private:
	double x[3][2];

};

class CCubicEquation
{
public:
	CCubicEquation(double a1=1, double b1=0, double c1=0, double d1=0, double xmin1=-1000, double xmax1=1000);
	void solveCubicEquation(void);
	void checkRoot(void)const;
	const root& getRoot(void);

private:
	void normalize(void);
	double findMax(void)const;
	void sort(void);
	double a0,b0,c0,d0,xmin,xmax;//Actually,xmin and xmax have not been used.
	double a,b,c,d;
	root root_CubicEquation;
};

#endif //CUBIC_EQUATION_H
