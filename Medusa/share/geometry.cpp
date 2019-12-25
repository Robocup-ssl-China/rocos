#include <geometry.h>
/************************************************************************/
/*                         CVector                                      */
/************************************************************************/
CVector CVector::rotate(double angle) const
{
	double nowdir = dir() + angle;
	double nowx = mod() * cos(nowdir);
	double nowy = mod() * sin(nowdir);
	return CVector(nowx,nowy);
}

/************************************************************************/
/*                         CGeoLine                                     */
/************************************************************************/
CGeoLineLineIntersection::CGeoLineLineIntersection(const CGeoLine& line_1,const CGeoLine& line_2)
{
	double d = line_1.a() * line_2.b() - line_1.b() * line_2.a();
	if(std::abs(d) < 0.0001){
		_intersectant = false;
	} else{
		double px = (line_1.b() * line_2.c() - line_1.c() * line_2.b()) / d;
		double py = (line_1.c() * line_2.a() - line_1.a() * line_2.c()) / d;
		_point = CGeoPoint(px, py);
		_intersectant = true;
	}
}

/************************************************************************/
/*                         CGeoRectangle                                */
/************************************************************************/
bool CGeoRectangle::HasPoint(const CGeoPoint& p ) const 
{
	int px = 0;
	int py = 0;
	for(int i = 0; i < 4; i++){ // 寻找一个肯定在多边形polygon内的点p：多边形顶点平均值
		px += _point[i].x();
		py += _point[i].y();
	}
	px /= 4;
	py /= 4;
	CGeoPoint inP = CGeoPoint(px, py);
	for(int i = 0; i < 4; i++) {
		CGeoSegment line_1(p, inP);
		CGeoSegment line_2(_point[i], _point[(i+1)%4]);
		CGeoLineLineIntersection inter(line_1, line_2);
		if(inter.Intersectant()
			&&line_1.IsPointOnLineOnSegment(inter.IntersectPoint())
			&&line_2.IsPointOnLineOnSegment(inter.IntersectPoint())){
			return false;
		}
	}
	return true;
}

/************************************************************************/
/*                         CGeoCircle                                   */
/************************************************************************/
bool CGeoCirlce::HasPoint(const CGeoPoint& p) const
{
	double d = (p - _center).mod();
	if(d < _radius){
		return true;
	}
	return false;
}

/************************************************************************/
/*                         CGeoEllipse                                   */
/************************************************************************/
bool CGeoEllipse::HasPoint(const CGeoPoint& p) const
{
	double x =p.x() - _center.x();
	double y = p.y() - _center.y();
	if ((x*x/(_xaxis*_xaxis) + y*y/(_yaxis*_yaxis)) < 1){
		return true;
	}
	return false;
}


/************************************************************************/
/*                      CGeoLineRectangleIntersection                   */
/************************************************************************/
CGeoLineRectangleIntersection::CGeoLineRectangleIntersection(const CGeoLine& line, const CGeoRectangle& rect)
{
	int num = 0;
	for(int i = 0; i < 4; i++){
		CGeoSegment rectbound(rect._point[i], rect._point[(i+1)%4]);
		CGeoLineLineIntersection inter(line, rectbound);
		if(inter.Intersectant() && rectbound.IsPointOnLineOnSegment(inter.IntersectPoint())){
			if(num >=2 ){
				printf("Error in CGeoLineRectangleIntersection, Num be: %d", num);
			}
			_point[num] = inter.IntersectPoint();
			num++;
		}
	}
	if(num > 0){
		_intersectant = true;
	} else{
		_intersectant = false;
	}
}
/************************************************************************/
/*                        CGeoLineCircleIntersection                    */
/************************************************************************/
CGeoLineCircleIntersection::CGeoLineCircleIntersection(const CGeoLine& line,const CGeoCirlce& circle)
{
	CGeoPoint projection = line.projection(circle.Center());
	CVector center_to_projection = projection - circle.Center();
	double projection_dist = center_to_projection.mod();
	if(  projection_dist < circle.Radius() ){ // 圆心到直线的距离小于半径
		_intersectant = true;
		if(projection_dist < 0.001){
			CVector p1top2 = line.point2() - line.point1();
			_point1 = circle.Center() + p1top2 * circle.Radius() / (p1top2.mod() + 0.0001);
			_point2 = circle.Center() + p1top2 * circle.Radius() / (p1top2.mod() + 0.0001) * -1;
		} else { 
			double angle = std::acos(projection_dist / circle.Radius());
			_point1 = circle.Center() + center_to_projection.rotate(angle) * circle.Radius() / projection_dist;
			_point2 = circle.Center() + center_to_projection.rotate(-angle) * circle.Radius() / projection_dist;
		}
	}else{
		_intersectant = false;
	}
}

/************************************************************************/
/*                        CGeoLineEllipseIntersection                    */
/************************************************************************/
double ourSqrt(double x) {
	if (x < 1e-10) x = 1e-10;
	return sqrt(x);
}

CGeoLineEllipseIntersection::CGeoLineEllipseIntersection(const CGeoLine& line,const CGeoEllipse& ellipse)
{
	const CGeoPoint Center =ellipse.Center();
	double x0=Center.x();
	double y0=Center.y();
	double m = ellipse.Xaxis();
	double n =ellipse.Yaxis();
	double a = line.a();
	double b = line.b();
	double c = line.c()+a*x0+ b*y0;
	double x1 = 0;
	double y1 = 0;
	double x2 = 0;
	double y2 = 0;
	//先求出与标准方程的交点再进行平移
	if ( b!=0){
		double p = (n*n*b*b + m*m*a*a);
		double q = (2*m*m*a*c);
		double r = (-m*m*n*n*b*b + (m*m*c*c));
		double tmp = q*q - 4 * p*r; // 解决不在sqrt定义域问题
		if (tmp < 1e-10) tmp = 1e-10;
		if ((tmp)>0 ){
			_intersectant = true;
			x1= ((-1)*q + ourSqrt(tmp))/(2*p);
			y1 = -(a*x1 + c)/b;
			x2 = ((-1)*q - ourSqrt(tmp))/(2*p);
			y2 = -(a*x2 + c)/b;
			CVector transVector1 = CVector(x1,y1);
			CVector transVector2 = CVector(x2,y2); 
			_point1 = Center + transVector1;
			_point2 = Center + transVector2; 
		}else if ((tmp)==0 ){
			_intersectant = true;
			x1 = (-q)/(2*p);
			y1 = -(a*x1 + c)/b;
			CVector transVector = CVector(x1,y1);
			_point1 = Center + transVector;
			_point2 = Center + transVector; 
		}else{
			_intersectant = false;
		}
	}else if (b== 0 ){
		double t = m*m*n*n - c*c*n*n/(a*a);
		if (t > 0){
			_intersectant = true;
			x1 = (-c)/a;
			y1 = ourSqrt(t)/m;
			x2 = (-c)/a;
			y2 = -ourSqrt(t)/m;
			CVector transVector1 = CVector(x1,y1);
			CVector transVector2 = CVector(x2,y2); 
			_point1 = Center + transVector1;
			_point2 = Center + transVector2; 
		}else if (t==0){
			_intersectant = true;
			x1 = (-c)/a;
			y1 = 0;
			CVector transVector = CVector(x1,y1);
			_point1 = Center + transVector;
			_point2 = Center + transVector; 
		}else{
			_intersectant = false;
		}
	}
}

/*********************************************************************/
/*                     CGeoSegmentCircleIntersection                 */
/********************************************************************/
CGeoSegmentCircleIntersection::CGeoSegmentCircleIntersection(const CGeoSegment& line,const CGeoCirlce& circle)
{
	CGeoPoint projection = line.projection(circle.Center());
	CVector center_to_projection = projection - circle.Center();
	double projection_dist = center_to_projection.mod();
	if(  projection_dist < circle.Radius() ){ // 圆心到直线的距离小于半径
		_intersectant = true;intersection_size=2;
		if(projection_dist < 0.001){
			CVector p1top2 = line.point2() - line.point1();
			_point1 = circle.Center() + p1top2 * circle.Radius() / (p1top2.mod() + 0.0001);
			_point2 = circle.Center() + p1top2 * circle.Radius() / (p1top2.mod() + 0.0001) * -1;
		} else { 
			double angle = std::acos(projection_dist / circle.Radius());
			_point1 = circle.Center() + center_to_projection.rotate(angle) * circle.Radius() / projection_dist;
			_point2 = circle.Center() + center_to_projection.rotate(-angle) * circle.Radius() / projection_dist;
		}
		if(!line.IsPointOnLineOnSegment(_point2)&&(!line.IsPointOnLineOnSegment(_point1))) {_intersectant = false;intersection_size=0;}
		else {  if (!line.IsPointOnLineOnSegment(_point1)) {_point1=_point2;intersection_size=1;}
		if (!line.IsPointOnLineOnSegment(_point2)) {intersection_size=1;}
		}
	}else{
		_intersectant = false;intersection_size=0;
	}
}