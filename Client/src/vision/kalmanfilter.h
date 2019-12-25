#ifndef KALMANFILTER_H
#define KALMANFILTER_H
#include "matrix2d.h"
#include "geometry.h"

/**
 * @brief The KalmanFilter class
 */
class  KalmanFilter {
  public:
    KalmanFilter();
    ~KalmanFilter();
    void init(double, double, double, double);
    const Matrix2d& update(double, double);
    const Matrix2d& update(CGeoPoint pos) {
        return update(pos.x(), pos.y());
    }
    const Matrix2d& follow(double, double);
    const Matrix2d& follow(CGeoPoint pos) {
        return follow(pos.x(), pos.y());
    }

  private:
    Matrix2d K;
    Matrix2d P;
    Matrix2d x;
    Matrix2d A;
    Matrix2d Q;
    Matrix2d H;
    Matrix2d R;
    Matrix2d x_;
    Matrix2d P_;
    Matrix2d z;
    bool ifInit;
};

#endif // KALMANFILTER_H
