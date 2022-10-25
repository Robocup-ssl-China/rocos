/*========================================================================
    Vector.h : Simple vector class for 2D and 3D vectors
  ------------------------------------------------------------------------
    Copyright (C) 1999-2002  James R. Bruce
    School of Computer Science, Carnegie Mellon University
  ------------------------------------------------------------------------
    This software is distributed under the GNU General Public License,
    version 2.  If you do not have a copy of this licence, visit
    www.gnu.org, or write: Free Software Foundation, 59 Temple Place,
    Suite 330 Boston, MA 02111-1307 USA.  This program is distributed
    in the hope that it will be useful, but WITHOUT ANY WARRANTY,
    including MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  ========================================================================*/

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <cmath>
#include "util.h"
using namespace std;
#define V2COMP(p) p.x,p.y

namespace Vector {

//=====================================================================//
//  Vector3D Class
//=====================================================================//

#define EPSILON (1.0E-10)

//=====================================================================//
//  Vector2D Class
//=====================================================================//

template <class num> class vector2d;

template <class num>
struct vector2d_struct {
  num x,y;

  void set(num nx, num ny) {x=nx; y=ny;}
  void set(vector2d<num> p) {x=p.x; y=p.y;}
};

template <class num>
class vector2d{
public:
  num x,y;

  vector2d()
    {}
  vector2d(num nx,num ny)
    {x=nx; y=ny;}

  void set(num nx,num ny)
    {x=nx; y=ny;}
  void set(vector2d<num> p)
    {x=p.x; y=p.y;}
  vector2d<num> &operator=(vector2d<num> p)
    {set(p); return(*this);}

  vector2d<num> &operator=(vector2d_struct<num> p)
    {set(p.x, p.y); return(*this);}

  num length() const;
  num sqlength() const;
  num angle() const
    {
    return(atan2(y,x));//角度值是（-180，180】
    }

  vector2d<num> norm() const;
  vector2d<num> norm(const num len) const;
  void normalize();
  vector2d<num> bound(const num max_length) const;

  num dot(const vector2d<num> p) const;
  num cross(const vector2d<num> p) const;

  vector2d<num> perp()
    {return(vector2d(-y, x));}

  vector2d<num> &operator+=(const vector2d<num> p);
  vector2d<num> &operator-=(const vector2d<num> p);
  vector2d<num> &operator*=(const vector2d<num> p);
  vector2d<num> &operator/=(const vector2d<num> p);

  vector2d<num> operator+(const vector2d<num> p) const;
  vector2d<num> operator-(const vector2d<num> p) const;
  vector2d<num> operator*(const vector2d<num> p) const;
  vector2d<num> operator/(const vector2d<num> p) const;

  vector2d<num> operator*(const num f) const;
  vector2d<num> operator/(const num f) const;
  vector2d<num> &operator*=(num f);
  vector2d<num> &operator/=(num f);

  vector2d<num> operator-() const;

  bool operator==(const vector2d<num> p) const;
  bool operator!=(const vector2d<num> p) const;
  bool operator< (const vector2d<num> p) const;
  bool operator> (const vector2d<num> p) const;
  bool operator<=(const vector2d<num> p) const;
  bool operator>=(const vector2d<num> p) const;

  vector2d<num> rotate(const double a) const;
  vector2d<num> perp() const;
};

template <class num>
num vector2d<num>::length() const
{
  return(sqrt(x*x + y*y));
}

template <class num>
num vector2d<num>::sqlength() const
{
  return(x*x + y*y);
}

//规一化
template <class num>
vector2d<num> vector2d<num>::norm() const
{
  vector2d<num> p;
  num l;

  l = sqrt(x*x + y*y);
  p.x = x / l;
  p.y = y / l;

  return(p);
}

template <class num>
vector2d<num> vector2d<num>::norm(const num len) const
{
  vector2d<num> p;
  num f;

  f = len / sqrt(x*x + y*y);
  p.x = x * f;
  p.y = y * f;

  return(p);
}

template <class num>
void vector2d<num>::normalize()
{
  num l;

  l = sqrt(x*x + y*y);
  x /= l;
  y /= l;
}

template <class num>
vector2d<num> vector2d<num>::bound(const num max_length) const
{
  vector2d<num> p;
  num l,f;

  l = sqrt(x*x + y*y);

  if(max_length < l){
    p.set(x,y);
  }else{
    f = max_length / l;
    p.set(f*x,f*y);
  }

  return(p);
}

//点乘,相当于投影之后的乘积
template <class num>
num vector2d<num>::dot(const vector2d<num> p) const
{
  return(x*p.x + y*p.y);
}

template <class num>
num dot(const vector2d<num> a,const vector2d<num> b)
{
  return(a.x*b.x + a.y*b.y);
}

template <class num>
num cosine(const vector2d<num> a,const vector2d<num> b)
// equivalent to dot(a.norm(),b.norm())
{
  num l;

  l = sqrt(a.x*a.x + a.y*a.y) * sqrt(b.x*b.x + b.y*b.y);

  return((a.x*b.x + a.y*b.y) / l);
}

template <class num>
num vector2d<num>::cross(const vector2d<num> p) const
{
  // right handed
  return(x*p.y - p.x*y);
}

// returns point rotated by <a> radians
template <class num>
vector2d<num> vector2d<num>::rotate(const double a) const
{
  vector2d<num> q;
  double s,c;

  s = sin(a);
  c = cos(a);

  q.x = c*x + -s*y;
  q.y = s*x + c*y;

  return(q);
}

template <class num>
vector2d<num> vector2d<num>::perp() const
{
  return vector2d<num>(-y, x);
}


#define VECTOR2D_EQUAL_BINARY_OPERATOR(opr) \
  template <class num> \
  vector2d<num> &vector2d<num>::operator opr (const vector2d<num> p) \
  {                  \
    x = x opr p.x;   \
    y = y opr p.y;   \
    return(*this);   \
  }

VECTOR2D_EQUAL_BINARY_OPERATOR(+=)
VECTOR2D_EQUAL_BINARY_OPERATOR(-=)
VECTOR2D_EQUAL_BINARY_OPERATOR(*=)
VECTOR2D_EQUAL_BINARY_OPERATOR(/=)

#define VECTOR2D_BINARY_OPERATOR(opr) \
  template <class num> \
  vector2d<num> vector2d<num>::operator opr (const vector2d<num> p) const \
  {                  \
    vector2d<num> r; \
    r.x = x opr p.x; \
    r.y = y opr p.y; \
    return(r);       \
  }

VECTOR2D_BINARY_OPERATOR(+)
VECTOR2D_BINARY_OPERATOR(-)
VECTOR2D_BINARY_OPERATOR(*)
VECTOR2D_BINARY_OPERATOR(/)

#define VECTOR2D_SCALAR_OPERATOR(opr) \
  template <class num> \
  vector2d<num> vector2d<num>::operator opr (const num f) const \
  {                  \
    vector2d<num> r;  \
    r.x = x opr f;   \
    r.y = y opr f;   \
    return(r);       \
  }

VECTOR2D_SCALAR_OPERATOR(*)
VECTOR2D_SCALAR_OPERATOR(/)

#define VECTOR2D_EQUAL_SCALAR_OPERATOR(opr) \
  template <class num> \
  vector2d<num> &vector2d<num>::operator opr (num f) \
  {                \
    x = x opr f;   \
    y = y opr f;   \
    return(*this); \
  }

VECTOR2D_EQUAL_SCALAR_OPERATOR(*=)
VECTOR2D_EQUAL_SCALAR_OPERATOR(/=)

#define VECTOR2D_LOGIC_OPERATOR(opr,combine) \
  template <class num> \
  bool vector2d<num>::operator opr (const vector2d<num> p) const \
  {                            \
    return((x opr p.x) combine \
           (y opr p.y));       \
  }

VECTOR2D_LOGIC_OPERATOR(==,&&)
VECTOR2D_LOGIC_OPERATOR(!=,||)

VECTOR2D_LOGIC_OPERATOR(< ,&&)
VECTOR2D_LOGIC_OPERATOR(> ,&&)
VECTOR2D_LOGIC_OPERATOR(<=,&&)
VECTOR2D_LOGIC_OPERATOR(>=,&&)


template <class num>
vector2d<num> vector2d<num>::operator-() const
{
  vector2d<num> r;
  r.x = -x;
  r.y = -y;
  return(r);
}

template <class num>
inline vector2d<num> abs(vector2d<num> a)
{
  a.x = ::fabs(a.x);
  a.y = ::fabs(a.y);

  return(a);
}

/*template <class num>
inline vector2d<num> max(vector2d<num> a,vector2d<num> b)
{
  vector2d<num> v;

  v.x = ::max(a.x,b.x);
  v.y = ::max(a.y,b.y);

  return(v);
}*/

template <class num>
inline vector2d<num> bound(vector2d<num> v,num low,num high)
{
  v.x = ::bound(v.x,low,high);
  v.y = ::bound(v.y,low,high);

  return(v);
}

template <class num>
num distance(const vector2d<num> a,const vector2d<num> b)
{
  num dx,dy;

  dx = a.x - b.x;
  dy = a.y - b.y;

  return(sqrt(dx*dx + dy*dy));
}

// returns square of distance between two points
template <class num>
num sqdistance(const vector2d<num> a,const vector2d<num> b)
{
  num dx,dy;

  dx = a.x - b.x;
  dy = a.y - b.y;

  return(dx*dx + dy*dy);
}

// returns distance from point p to line x0-x1
template <class num>
num distance_to_line(const vector2d<num> x0,const vector2d<num> x1,const vector2d<num> p)
{
  vector2d<num> x;
  num t;

  t = ((p.x - x0.x) + (p.y - x0.y)) / (x1.x + x1.y);
  x = x0 + (x1 - x0) * t;

  // printf("dist:(%f,%f)-(%f,%f)\n",x.x,x.y,p.x,p.y);

  return(distance(x,p));
}

// returns perpendicular offset from line x0-x1 to point p
template <class num>
num offset_to_line(const vector2d<num> x0,const vector2d<num> x1,const vector2d<num> p)
{
  vector2d<num> n;

  // get normal to line
  n = (x1 - x0).perp().norm();

  return(n.dot(p - x0));
}

// returns perpendicular offset from line x0-x1 to point p
template <class num>
num offset_along_line(const vector2d<num> x0,const vector2d<num> x1,const vector2d<num> p)
{
  vector2d<num> n,v;

  // get normal to line
  n = x1 - x0;
  n.normalize();

  v = p - x0;

  return(n.dot(v));
}

// returns nearest point on segment a0-a1 to line b0-b1
template <class num>
vector2d<num> segment_near_line(const vector2d<num> a0,const vector2d<num> a1,
                const vector2d<num> b0,const vector2d<num> b1)
{
  vector2d<num> v,n,p;
  double dn,t;

  v = a1-a0;
  n = (b1-b0).norm();
  n.set(-n.y,n.x);

  dn = dot(v,n);
  if(fabs(dn) < EPSILON) return(a0);

  t = -dot(a0-b0,n) / dn;
  // printf("t=%f dn=%f\n",t,dn);
  if(t < 0) t = 0;
  if(t > 1) t = 1;
  p = a0 + v*t;

  return(p);
}

//
template <class num>
vector2d<num> intersection(const vector2d<num> a1, const vector2d<num> a2,
         const vector2d<num> b1, const vector2d<num> b2)
{
  vector2d<num> a = a2 - a1;

  vector2d<num> b1r = (b1 - a1).rotate(-a.angle());
  vector2d<num> b2r = (b2 - a1).rotate(-a.angle());
  vector2d<num> br = (b1r - b2r);

  return 
    vector2d<num>(b2r.x - b2r.y * (br.x / br.y), 0.0).rotate(a.angle()) + a1;
}

// gives counterclockwise angle from <a-b> to <c-b>
template <class num>
num vertex_angle(const vector2d<num> a,
                 const vector2d<num> b,
                 const vector2d<num> c)
{
  return(angle_mod((a-b).angle() - (c-b).angle()));
}


//==== Generic functions =============================================//
// (work on 2d or 3d vectors)

// returns nearest point on line segment x0-x1 to point p
template <class vector>
vector point_on_segment(const vector x0,const vector x1,const vector p,bool flag)
{//flag = true；考虑线段的性质，flag = flase；不考虑线段的性质
  vector sx,sp,r;
  double f,l;

  sx = x1 - x0;
  sp = p  - x0;

  f = dot(sx,sp);
  if(f <= 0.0 && flag) return(x0); // also handles x0=x1 case

  l = sx.sqlength();
  if(f >= l && flag) return(x1);

  r = x0 + sx * (f / l);

  return(r);
}

template <class vector>
double closest_point_time(const vector x1,const vector v1,
                          const vector x2,const vector v2)
// returns time of closest point of approach of two points
// moving along constant velocity vectors.
{
  vector v  = v1 - v2;
  double sl = v.sqlength();
  double t;

  if(sl < EPSILON) return(0.0); // parallel tracks, any time is ok.

  t = -v.dot(x1 - x2) / sl;
  if(t < 0.0) return(0.0); // nearest time was in the past, now
                           // is closest point from now on.

  return(t);
}

// Ported from: dist3D_Segment_to_Segment
//   from http://geometryalgorithms.com
// Copyright 2001, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose providing
// that this copyright notice is included with it.  SoftSurfer makes
// no warranty for this code, and cannot be held liable for any real
// or imagined damage resulting from its use.  Users of this code must
// verify correctness for their application.
template <class vector>
double distance_seg_to_seg(vector s1a,vector s1b,vector s2a,vector s2b, vector& p1, vector& p2)
// return distnace between segments s1a-s1b and s2a-s2b
{
  vector dp;
  vector u = s1b - s1a;
  vector v = s2b - s2a;
  vector w = s1a - s2a;
  float a = dot(u,u);        // always >= 0
  float b = dot(u,v);
  float c = dot(v,v);        // always >= 0
  float d = dot(u,w);
  float e = dot(v,w);
  float D = a*c - b*b;       // always >= 0
  float sc, sN, sD = D;      // sc = sN / sD, default sD = D >= 0
  float tc, tN, tD = D;      // tc = tN / tD, default tD = D >= 0

  if(false){
    printf("SegDist (%f,%f)-(%f,%f) to (%f,%f)-(%f,%f)\n",
           V2COMP(s1a),V2COMP(s1b),V2COMP(s2a),V2COMP(s2b));
  }

  // compute the line parameters of the two closest points
  if(D < EPSILON){    // the lines are almost parallel
    sN = 0.0;
    tN = e;
    tD = c;
  }else{                // get the closest points on the infinite lines
    sN = (b*e - c*d);
    tN = (a*e - b*d);
    if(sN < 0){         // sc < 0 => the s=0 edge is visible
      sN = 0.0;
      tN = e;
      tD = c;
    }else if(sN > sD){  // sc > 1 => the s=1 edge is visible
      sN = sD;
      tN = e + b;
      tD = c;
    }
  }

  if(tN < 0){           // tc < 0 => the t=0 edge is visible
    tN = 0.0;
    // recompute sc for this edge
    if(-d < 0){
      sN = 0.0;
    }else if(-d > a){
      sN = sD;
    }else{
      sN = -d;
      sD = a;
    }
  }else if(tN > tD){      // tc > 1 => the t=1 edge is visible
    tN = tD;
    // recompute sc for this edge
    if((-d + b) < 0){
      sN = 0;
    }else if((-d + b) > a){
      sN = sD;
    }else{
      sN = (-d + b);
      sD = a;
    }
  }
  // finally do the division to get sc and tc
  sc = sN / sD;
  tc = tN / tD;

  p1 = s1a + u*sc;
  p2 = s2a + v*tc;

  // get the difference of the two closest points
  dp = w + u*sc - v*tc; // = S1(sc) - S2(tc)

  return(dp.length()); // return the closest distance
}

} // namespace vector

//==== Vector types ====//

typedef Vector::vector2d<double> vector2d;
typedef vector2d point2d;
typedef Vector::vector2d_struct<double> vector2d_struct;

typedef Vector::vector2d<float> vector2f;
typedef vector2f point2f;

//==== Some conversion functions ====//


inline vector2f vstructtof(vector2d_struct &a)
{
  vector2f r((float) a.x, (float) a.y);
  return (r);
}

inline vector2d vstructtod(vector2d_struct &a)
{
  vector2d r(a.x, a.y);
  return (r);
}

inline void vftostruct(vector2f a, vector2d_struct &b)
{
  b.x = a.x;
  b.y = a.y;
}

inline void vdtostruct(vector2d a, vector2d_struct &b)
{
  b.x = a.x;
  b.y = a.y;
}

inline vector2f vdtof(vector2d &a)
{
  vector2f r(a.x,a.y);
  return(r);
}

inline vector2d vftod(vector2f &a)
{
  vector2d r(a.x,a.y);
  return(r);
}

inline void vdtodarray(vector2d &a, double *array)
{
  array[0] = a.x;
  array[1] = a.y;
}

inline void vftodarray(vector2f &a, double *array)
{
  array[0] = (double) a.x;
  array[1] = (double) a.y;
}

inline void vdtofarray(vector2d &a, float *array)
{
  array[0] = (float) a.x;
  array[1] = (float) a.y;
}

inline void vftofarray(vector2f &a, float *array)
{
  array[0] = a.x;
  array[1] = a.y;
}
#endif
// __VECTOR_H__