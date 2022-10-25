//#include "RobotsCollision.h"
//#include "VisionModule.h"
//#include "RobotPredictData.h"
//#include <iostream>

//RobotsCollisionDetector::RobotsCollisionDetector()
//{
//	for (int i=0; i<PARAM::Field::MAX_PLAYER; i++){
//		our_collision[i] = 0;
//		opp_collision[i] = 0;
//	}
//}
//RobotsCollisionDetector* RobotsCollisionDetector::Instance()
//{
//	static RobotsCollisionDetector* _instance = NULL;
//	if ( _instance == NULL )
//		_instance = new RobotsCollisionDetector;

//	return _instance;
//}
//void RobotsCollisionDetector::clearLastResult()
//{
//	for (int i=0; i<PARAM::Field::MAX_PLAYER; i++){
//		our_collision[i] = 0;
//		opp_collision[i] = 0;
//	}
//}
//void RobotsCollisionDetector::setVision(CVisionModule* pVision)
//{
//	_pVision = pVision;
//}
//void RobotsCollisionDetector::setCheckNum(int side, int robotID)
//{
//	current_check_side = side;
//	current_check_num = robotID;
//}
//bool RobotsCollisionDetector::checkCollision(const RobotVisionData& visionPos, double calTime)
//{
//	using namespace std;

//	double stepTime = calTime/3;
//	if ( current_check_side == 0 ){
//		// 检查我方的车
//		for (int i=0; i<PARAM::Field::MAX_PLAYER; i++){
//			if (_pVision->theirPlayer(i).Valid()){
//				if ( opp_collision[i-1] != 0 && opp_collision[i-1] != current_check_num )
//					continue; // 不是和我撞

//				if ( opp_collision[i-1] != 0 && opp_collision[i-1] == current_check_num){
//					our_collision[current_check_num-1] = i;
//					break; // 检测过了，是和我撞
//				}

//				// 还没检测过
//				const CGeoPoint& oppPos = _pVision->theirPlayer(i).Pos();
//				if ( oppPos.dist(visionPos.Pos()) < PARAM::Field::MAX_PLAYER_SIZE ){
//					our_collision[current_check_num-1] = i;
//					opp_collision[i-1] = current_check_num;
//					break;
//				}
//			}
//		}

//		/*if ( our_collision[current_check_num-1] > 0 )
//			cout<<"our:"<<current_check_num<<" collide with their "<<our_collision[current_check_num-1]<<endl;*/

//		return our_collision[current_check_num-1] > 0;
//	}
//	else{
//		// 检查对方车
//		//for (int i=0; i<PARAM::Field::MAX_PLAYER; i++){
//		//	if (_pVision->ourPlayer(i).Valid()){
//		//		if ( our_collision[i-1] != 0 && our_collision[i-1] != current_check_num )
//		//			continue; // 不是和我撞

//		//		if ( our_collision[i-1] != 0 && our_collision[i-1] == current_check_num){
//		//			opp_collision[current_check_num-1] = i;
//		//			break; // 检测过了，是和我撞
//		//		}

//		//		// 还没检测过
//		//		const CGeoPoint& teammatePos = _pVision->ourPlayer(i).Pos();
//		//		if ( teammatePos.dist(visionPos.Pos()) < PARAM::Field::MAX_PLAYER_SIZE ){
//		//			opp_collision[current_check_num-1] = i;
//		//			our_collision[i-1] = current_check_num;
//		//			break;
//		//		}
//		//	}
//		//}

//		return opp_collision[current_check_num-1] > 0;
//	}
//}
