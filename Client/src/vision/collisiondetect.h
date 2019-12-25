#ifndef COLLISIONDETECT_H
#define COLLISIONDETECT_H
#include "singleton.hpp"
#include "globaldata.h"
#include "geometry.h"

#define DETECT_DIST 10 //detect whether the ball in 300mm of vechile
#define SPLIT_THRESHOLD 20  //split
#define NEAR_VECHILE_MIN_FRAME 5 //keep frame to detect ball SLIGHTLY touch vechile
#define LENGTH_THERESHOLD 5800
#define WIDTH_THERESHOLD 4400
#define MAX_FRAME_NUM 20    //split and merge frame num
#define SPLIT_THRESHOLD 20  //split
#define CHIP_DIS 10
class CCollisionDetect {
  public:
    CCollisionDetect();
    //virtual ~CCollisionDetect();
    void visionAlart();
    void analyzeData(ReceiveVisionMessage & result);
    bool ballCloseEnough2Analyze(int);
    bool ballIsOnEdge(CGeoPoint);
  private:
    int LinePoint[MAX_FRAME_NUM];
    int _RecordFlag = 0;
    int _RecordIndex = -1;
    int PointN;
    //int LastTouch=0;//0 for struggle; 1 for our ;2 for their; 3 for wall;

};
typedef Singleton<CCollisionDetect> CollisionDetect;

#endif // COLLISIONDETECT_H
