//#include <PlayInterface.h>
//#include <TaskMediator.h>

//CPlayInterface::CPlayInterface()
//{
//    for(int i = 0;i<=PARAM::Field::MAX_PLAYER;i++){
//		_realIndex[i] = 0;
//		_visionIndex[i] = 0;
//	}
////	_visionIndex[PARAM::Field::MAX_PLAYER] = 0;
////	_visionIndex[PARAM::Field::MAX_PLAYER] = 0;
//}

//CPlayInterface::~CPlayInterface()
//{
//}

////////////////////////////////////////////////////////////////////////////
///// 真实车号与决策车号对应处理，待被转移
//void CPlayInterface::setRealIndex(int Num, int realNum)
//{
//	// error cout
//	if (Num < 1 || Num > PARAM::Field::MAX_PLAYER) {
//		STD_DEBUG_OUT("@@@@@@@@@@@@@@@@Strategy Num","input invalid");
//		return ;
//	}

//	// error cout
//    if (realNum < 1 || realNum > PARAM::Field::MAX_PLAYER) {
//		STD_DEBUG_OUT("@@@@@@@@@@@@@@@Vision Index","input invalid");
//		return ;
//	}

//	_realIndex[Num-1] = realNum;

//	// (1-10)-(1-5)
//	_visionIndex[realNum-1] = Num;

//	return ;
//}

////int CPlayInterface::getRealIndexByNum(int Num)
////{
////	if (Num < 1 || Num > PARAM::Field::MAX_PLAYER) {
////		STD_DEBUG_OUT("#############Get RealIndex Error","must between 1-5 , Please check!!!");
////		return 0;
////	} else {
////		return _realIndex[Num-1];
////	}
////}

////int CPlayInterface::getNumbByRealIndex(int realNum)
////{
////    if (realNum < 1 || realNum > PARAM::Field::MAX_PLAYER) {
////		STD_DEBUG_OUT("#############Get StrategyNum Error","must between 1-12, Please check!!!");
////		return 0;
////	} else {
////		//if (realNum == TaskMediator::Instance()->goalie()) {
////		//	static int goalieIndex = _visionIndex[realNum-1];
////		//	bool needReturnModify = false;
////		//	int indexReturnModify = 0;
////		//	if (0 == goalieIndex) {
////		//		STD_DEBUG_OUT("#############Get GoalieNum Error","invalid value");
////		//		goalieIndex = _visionIndex[realNum-1];

////		//		// TODO [9/11/2011 ZJUNlict]
////		//		if (0 == goalieIndex) {
////		//			for(int i = 0; i < PARAM::Field::MAX_PLAYER; i ++){
////		//				if (0 == getRealIndexByNum(i)) {
////		//					needReturnModify = true;
////		//					indexReturnModify = i;
////		//					break;
////		//				}
////		//			}

////		//			if (! needReturnModify) {
////		//				needReturnModify = true;
////		//				indexReturnModify = 1;
////		//			}
////		//		}
////		//	}

////		//	if (needReturnModify) {
////		//		return indexReturnModify;
////		//	} else {
////		//		return goalieIndex;
////		//	}
////		//}

////		return _visionIndex[realNum-1];
////	}
////}

//void CPlayInterface::clearRealIndex()
//{
//	// clear real vehicle index
//	for (int i = 0; i < PARAM::Field::MAX_PLAYER; i ++) {
//		_realIndex[i] = 0;
//	}

//	// clear vision vehicle index
//    for (int i = 0; i < PARAM::Field::MAX_PLAYER; i ++) {
//		_visionIndex[i] = 0;
//	}

//	return ;
//}

////void CPlayInterface::setRoleNameAndNum(string name, int num)
////{
////	_roleNum[name] = num;
////}

////int CPlayInterface::getNumByRoleName(string name)
////{
////	return _roleNum[name];
////}
