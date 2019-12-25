#ifndef KALMANFILTER1D_H
#define KALMANFILTER1D_H

/*
 * Kalman Filter For Robot Direction
 * Author: Wayne
 * Date:2018/5/27
 */

class KalmanFilterDir
{
public:
    KalmanFilterDir();
    void init(double,double,double,double);
    double update(double);
    double normalize(double );
private:
    double x,A,p,q,gain,H,r;
};

#endif // KALMANFILTER1D_H
