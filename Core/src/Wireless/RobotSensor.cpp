#include "RobotSensor.h"
//#include <PlayInterface.h>
////#include <CommandSender.h>
//#include "BestPlayer.h"
//#include <WorldModel.h>
//#include <fstream>
//#include "Global.h"
//#include "parammanager.h"
//using namespace std;

//namespace {
//	// 仿真实物标签
//    bool IS_SIMULATION = false;
//	bool debug = true;
//}

//CRobotSensor::CRobotSensor()
//{
//	// 读入仿真实物标签
//    ZSS::ZParamManager::instance()->loadParam(IS_SIMULATION,"Alert/IsSimulation",false);
//    qDebug() << "RobotSensor : IS_SIMULATION : " << IS_SIMULATION;

//	// 数据进行初始化
//	memset(_isValid, false, sizeof(_isValid));
//	memset(_timeIndex, 0, sizeof(_timeIndex));
//	memset(_updateCycle, 0, sizeof(_updateCycle));
//	memset(_valid_count, 2, sizeof(_valid_count));
//	memset(_lastKickingChecked, false, sizeof(_lastKickingChecked));
//	memset(_lastCheckedKickingCycle, 0, sizeof(_lastCheckedKickingCycle));

//	for(int i = 0; i < PARAM::Field::MAX_PLAYER; i ++) {
//		robotInfoBuffer[i].kickId = -1;
//		robotInfoBuffer[i].bInfraredInfo = false;
//		robotInfoBuffer[i].bControledInfo = false;
//		robotInfoBuffer[i].nKickInfo = 0;
//		robotInfoBuffer[i].changeNum = 0;
//		robotInfoBuffer[i].kickCount = 0;

//		_lastInfraredInfo[i] = false;
//		_lastBallCtrlInfo[i] = false;
//	}

//	for (int i=0;i < PARAM::Field::MAX_PLAYER; i ++){
//		_robotMsg[i].robotNum = 0;
//		_robotMsg[i].capacity = 0;
//		_robotMsg[i].battery = 0;
//		_robotMsg[i].infare = false;
//	}

//}

//void CRobotSensor::Update(int cycle)
//{
//	// 仿真不处理，直接予以返回
//	// 遍历以获取各个小车的下位本体信息

//	//string _refereeMsg = "";

//	//if (! vision->gameState().canMove()) {
//	//	_refereeMsg = "GameHalt";
//	//} else if( vision->gameState().gameOver()/* || _pVision->gameState().isOurTimeout() */){
//	//	_refereeMsg = "GameOver";
//	//} else if( vision->gameState().isOurTimeout() ){
//	//	_refereeMsg = "OurTimeout";
//	//} else if(!vision->gameState().allowedNearBall()){
//	//	// 对方发球
//	//	if(vision->gameState().theirIndirectKick()){
//	//		_refereeMsg = "TheirIndirectKick";
//	//	} else if (vision->gameState().theirDirectKick()){
//	//		_refereeMsg = "TheirDirectKick";
//	//	} else if (vision->gameState().theirKickoff()){
//	//		_refereeMsg = "TheirKickOff";
//	//	} else if (vision->gameState().theirPenaltyKick()){
//	//		_refereeMsg = "TheirPenaltyKick";
//	//	} else{
//	//		_refereeMsg = "GameStop";
//	//	}
//	//}


////	if(vision->GetCurrentRefereeMsg()!="GameStop"){
////		for (int i = 0; i < PARAM::Field::MAX_PLAYER; i ++) {
////			// 下位本体信息获取
////			int tempIndex = CommandSender::Instance()->getRobotInfo(i, &rawDataBuffer);
			
////			if (tempIndex < 0) {
////				continue;
////			}

////			// 强制保持一段时间球被踢出
////			if (_lastKickingChecked[i] || robotInfoBuffer[i].nKickInfo > 0) {
////				if (cycle - _lastCheckedKickingCycle[i] > 5) {
////					_lastKickingChecked[i] = false;
////					robotInfoBuffer[i].nKickInfo = 0;
////				}
////			}

////			// 数据有效(tempIndex表示数据的序列号)
////            if (tempIndex != _timeIndex[i] && tempIndex>= 0){
////				_timeIndex[i] = tempIndex;
////				_isValid[i] = true;
////				_valid_count[i] = 2;
////				_updateCycle[i] = cycle;

////				UpdateBallDetected(i);

////				// 表明是踢球的包
////				//if (rawDataBuffer.nKickInfo) {
////					UpdateKickerInfo(i, cycle);
////				//}
				
////			} else {
////				_lastKickingChecked[i] = false;
////			}
////		}
////	}else{
////		//cout<<"in stop"<<endl;
////		static bool hasreceived[PARAM::Field::MAX_PLAYER]={false};
////		static bool timeout[PARAM::Field::MAX_PLAYER]={false};
////		static int count[PARAM::Field::MAX_PLAYER]={0};
////		static int i=1;
////		//i表示每次置位的决策车号
////		static bool noneedcheck=false;
////		static int cycle=0;
////		if ((vision->Cycle()-cycle)>6){
////			noneedcheck=false;
////			cycle=WorldModel::Instance()->vision()->Cycle()-cycle;
////			//for (int num=1;num<PARAM::Field::MAX_PLAYER;num++){
////			//	memset(outputmsg[num],0,sizeof(outputmsg[num]));
////			//}
////			//cout<<"firstin cycle:"<<cycle<<endl;
////		}
////		  cycle = WorldModel::Instance()->vision()->Cycle();
////		if (i>PARAM::Field::MAX_PLAYER){
////			i=1;
////			noneedcheck=true;
////			//cout<<"********************************"<<endl;
////			//printf("RealNum:  %d\t%d\t%d\t%d\t%d\t%d\n",_robotMsg[1].robotNum,_robotMsg[2].robotNum,_robotMsg[3].robotNum,_robotMsg[4].robotNum,
////			//	_robotMsg[5].robotNum,_robotMsg[6].robotNum);
////			//printf("Battery: ");
////			//for(int j = 1; j <= 6; j++ ){
////			//	printf("%.2f\t", (_robotMsg[j].battery + 3.951) / 13.51 );
////			//}
////			//printf("\n");
////			//printf("Capcity:  %d\t%d\t%d\t%d\t%d\t%d\n",_robotMsg[1].capacity,_robotMsg[2].capacity,_robotMsg[3].capacity,_robotMsg[4].capacity,
////			//	_robotMsg[5].capacity,_robotMsg[6].capacity);
////			//cout<<"Infared:  "<<_robotMsg[1].infare<<"\t"<<_robotMsg[2].infare<<"\t"<<_robotMsg[3].infare<<"\t"<<_robotMsg[4].infare<<"\t"<<_robotMsg[5].infare<<"\t"
////			//	<<_robotMsg[6].infare<<endl;
////			//cout<<"********************************"<<endl;
////			return;
////		}
////		int realnum= PlayInterface::Instance()->getRealIndexByNum(i);
////		if ( !noneedcheck){
////			if (realnum){
////				if ((!hasreceived[i] || !timeout[i])&&!noneedcheck){
////					CommandSender::Instance()->setstop(i,true);
////					int tempIndex = CommandSender::Instance()->getRobotInfo(i, &rawDataBuffer);
////				//int p=rawDataBuffer.battery;
////				//int f=rawDataBuffer.nRobotNum;
////				//cout<<cycle<<"  i= "<<i<<"  robotnum=  "<<f<<"  tempIndex= "<<tempIndex<<"  battery=  "<<p<<endl;
////					if (tempIndex != _timeIndex[i] && tempIndex>=0) {
////						_timeIndex[i] = tempIndex;
////						_isValid[i] = true;
////						_valid_count[i] = 2;
////						_updateCycle[i] = cycle;
////						hasreceived[i]=true;
////						int RobotNum=rawDataBuffer.nRobotNum;
////						int batterymsg=rawDataBuffer.battery;
////						int capacity=rawDataBuffer.capacity;
////						bool infaredmsg=rawDataBuffer.bInfraredInfo;
////						_robotMsg[i].robotNum = RobotNum;
////						_robotMsg[i].battery = batterymsg;
////						_robotMsg[i].capacity = capacity;
////						_robotMsg[i].infare = infaredmsg;
////						//memset(outputmsg[i],0,sizeof(outputmsg[i]));
////						//if (batterymsg>0 && infaredmsg==false){
////						//	sprintf(outputmsg[i],"num: %d bat: %d cap: %d good",RobotNum,batterymsg,capacity);
////						//}else if (batterymsg>0 && infaredmsg==true){
////						//	sprintf(outputmsg[i],"num: %d bat: %d cap: %d bad",RobotNum,batterymsg,capacity);
////						//}else{
////						//	;
////						//}
////					//stopmsgfile<<RobotNum<<'\t'<<batterymsg<<'\t'<<capacity<<'\t'<<infaredmsg<<endl;
////						hasreceived[i]=false;
////						count[i]=0;
////						RobotNum=0;
////						batterymsg=0;
////						capacity=0;
////						CommandSender::Instance()->setstop(i,false);
////						i++;
////						if (i<=PARAM::Field::MAX_PLAYER){
////							CommandSender::Instance()->setstop(i,true);
////						}
////					}else{
////						count[i]++;
////						if (count[i]==10) {
////						/*cout<<"Packet Number is :		"<<count[i]<<endl;*/
////							count[i] = 0;
////							timeout[i]=true;
////							CommandSender::Instance()->setstop(i,false);
////							timeout[i]=false;
////						//cout<<i<<" "<<"not matched"<<endl;
////							i++;
////							if (i<=PARAM::Field::MAX_PLAYER){
////								CommandSender::Instance()->setstop(i,true);
////							}
////						}
////					}
////				}
////			}else{
////			i++;
////			}
////		}
////}

////	return ;
//}

//bool CRobotSensor::IsInfraredOn(int num)
//{
//    return robotInfoBuffer[num].bInfraredInfo;
//}

//void CRobotSensor::UpdateBallDetected(int num)
//{
//	// 丢掉除了自己本身的数据
//	int realnum = PlayInterface::Instance()->getRealIndexByNum(num);
//	if (realnum != rawDataBuffer.nRobotNum) {
//		return ;
//	}

//	// 现在红外信号时改变才上传 [8/7/2011 cliffyin]
//	// 红外
//	bool currentInfraredInfo = rawDataBuffer.bInfraredInfo;
//	bool InfraredInfoReset = false;
//	if (_lastInfraredInfo[num] != currentInfraredInfo) {
//		InfraredInfoReset = true;
//		_lastInfraredInfo[num] = currentInfraredInfo;
//	}

//	if (InfraredInfoReset) {
//		robotInfoBuffer[num].bInfraredInfo = currentInfraredInfo;
//		//cout<<"Num : "<<num<<" Real : "<<rawDataBuffer.nRobotNum<<endl;
//		//cout<<"Change CurrentInfraredInfo : "<<currentInfraredInfo<<endl;
//	}

//	// 控球
//	bool currentBallCtrl = rawDataBuffer.bControledInfo;
//	bool BallCtrlReset = false;
//	if (_lastBallCtrlInfo[num] != currentBallCtrl) {
//		BallCtrlReset = true;
//		_lastBallCtrlInfo[num] = currentBallCtrl;
//	}

//	if (BallCtrlReset) {
//		robotInfoBuffer[num].bControledInfo = currentBallCtrl;
//	/*	cout<<"Num : "<<num<<" Real : "<<rawDataBuffer.nRobotNum<<endl;
//		cout<<"Change CurrentBallCtrl : "<<currentBallCtrl<<endl;*/
//	}

//	return ;
//}

//void CRobotSensor::UpdateKickerInfo(int num, int cycle)
//{
//	// 前提是已经返回包有球被踢出的信号
//	//std::cout << "Kick~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" <<rawDataBuffer.nKickInfo << endl;
//	robotInfoBuffer[num] = rawDataBuffer;
//	//if (num == 3 || num == 2) cout << num << " infra : " << rawDataBuffer.bInfraredInfo << endl;
//	_lastKickingChecked[num] = true;
//	_lastCheckedKickingCycle[num] = cycle;
	
//	return ;
//}
