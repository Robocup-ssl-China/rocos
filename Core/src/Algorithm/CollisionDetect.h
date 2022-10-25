//#pragma once
//#include "VisionModule.h"
//#include "staticparams.h"
//#include "DataQueue.hpp"
//#include <MultiThread.h>

//#define MAX_FRAME_NUM 20  //最大存储帧数
//struct VisionInfo {
//	CGeoPoint BallPos;
//	CGeoPoint ourPlayer[PARAM::Field::MAX_PLAYER];
//	CGeoPoint theirPlayer[PARAM::Field::MAX_PLAYER];
//};

//class CCollisionDetect
//{
//public:
//	CCollisionDetect();
//	virtual ~CCollisionDetect();
//	void initialize(CVisionModule*);
//	void visionAlart();
//    static void ZSS_THREAD_FUNCTION doCollisionDetect();

//private:
//    void recordPos();
//	void analyzeData();
//	bool IsOnEdge(CGeoPoint);

//    std::thread *_collision_detect_thread = nullptr;
//	CVisionModule* _pVision;			///<图像指针
//	DataQueue<VisionInfo> _VisionQueue;
//	int  LinePoint[MAX_FRAME_NUM];
//	int _RecordFlag = 0;
//	int _RecordIndex = -1;
//	int PointN;
//	int LastTouch=0;//0 for NOT sure; 1 for our ;2 for their; 3 for wall;
//	int LastTouchNumber = 0;//car number; 0 for not sure or wall
//};
//typedef NormalSingleton< CCollisionDetect > CollisionDetect;

