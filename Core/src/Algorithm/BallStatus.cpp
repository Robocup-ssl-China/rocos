#include "BallStatus.h"
#include <RobotSensor.h>
#include "Global.h"
#include "utils.h"
#include "BallSpeedModel.h"
#include "parammanager.h"
#include "staticparams.h"
namespace{
	bool isNearPlayer = false;
	bool isEnemyTouch = false;
    bool isOurTouch = false;

	int standOffCouter=0;
	int ourBallCouter=0;


	int lastTheirBestPlayer=0;
	int lastOurBestPlayer=0;
	const int StateMaxNum=5;
	int stateCouter[StateMaxNum]={0,0,0,0,0};

    bool IS_SIMULATION = false;

    const int KICK_OUT_MIN_VELOCITY = 750;
}

CBallStatus::CBallStatus(void):_chipkickstate(false)
{
	_ballMovingVel = CVector(0,0);
	_isKickedOut = false;
	_ballToucher=0;
	_ballState=None;
	_ballStateCouter=0;

	standOffCouter=0;
	ourBallCouter=0;
    ZSS::ZParamManager::instance()->loadParam(IS_SIMULATION,"Alert/IsSimulation",false);

	initializeCmdStored();
}

void CBallStatus::UpdateBallStatus(const CVisionModule* pVision)
{
	UpdateBallMoving(pVision);
	CheckKickOutBall(pVision);
	_contactChecker.refereeJudge(pVision);
	_lastBallToucher=_ballToucher;
	_ballToucher=_contactChecker.getContactNum();
	if (_isKickedOut||_isChipKickOut){
		_ballToucher=_kickerNum;
	}

}


void CBallStatus::UpdateBallMoving(const CVisionModule* pVision)
{
	const ObjectPoseT& ball = pVision->ball(); // 球	
	isNearPlayer = false;
	for (int i=0; i<PARAM::Field::MAX_PLAYER*2; i++){
		if (pVision->allPlayer(i).Valid() && pVision->allPlayer(i).Pos().dist(ball.Pos())< PARAM::Field::MAX_PLAYER_SIZE/2+5){
			isNearPlayer = true;
			break;
		}
	}
    if (false == isNearPlayer && ball.Vel().mod()>20){
		// 只有当球不在车附近时,才使用预测后的球速来更新; 以免球在车附近时,将球速设为0;
		_ballMovingVel = ball.Vel();
	}
	else{
		// 保留球速，方向使用稳定可靠的方向
		double ballspeed = max(1.0, ball.Vel().mod());
		_ballMovingVel = Utils::Polar2Vector(ballspeed, _ballMovingVel.dir());
		//std::cout<<"BallStatus: Ball Near Player"<<endl;
	}
}

// 从PlayInterface中移出的球状态部分
void CBallStatus::initializeCmdStored()
{
	for (int i=0; i<PARAM::Field::MAX_PLAYER; i++){
		for (int j=0; j<MAX_CMD_STORED; j++){
			_kickCmd[i][j].setKickCmd(i,0,0,0);
		}
	}
}

void CBallStatus::setCommand(CSendCmd kickCmd, int cycle)
{
	_kickCmd[kickCmd.num()][cycle % MAX_CMD_STORED] = kickCmd;
}
void CBallStatus::setCommand(int num, int normalKick, int chipKick, unsigned char dribble, int cycle)
{
	_kickCmd[num][cycle % MAX_CMD_STORED].setKickCmd(num, normalKick, chipKick, dribble);
}

void CBallStatus::clearKickCmd() 
{
	for (int i=0; i<PARAM::Field::MAX_PLAYER; i++){
		for (int j=0; j<MAX_CMD_STORED; j++){
			_kickCmd[i][j].clear();
		}
	}
}

void CBallStatus::CheckKickOutBall(const CVisionModule* pVision)
{
	_isKickedOut = false;
	_isChipKickOut = false;
	for (int num=0; num < PARAM::Field::MAX_PLAYER; num++){
        //利用通讯信息
//            bool sensorValid = RobotSensor::Instance()->IsInfoValid(num);
//            bool isBallInFoot = RobotSensor::Instance()->IsInfraredOn(num);
        int isKickDeviceOn = RobotSensor::Instance()->IsKickerOn(num);
//            bool isBallControlled = RobotSensor::Instance()->isBallControled(num);
        /*if(sensorValid){
            cout<<pVision->Cycle()<<"  "<<sensorValid<<"  "<<isBallInFoot<<"  "<<isKickDeviceOn<<" "<<isBallControlled<<endl;
        }*/
        // 用于双向通迅出现问题时，且此时红外正常
        if (isKickDeviceOn > 0) {
            _kickerNum = num;
            _isKickedOut = true;
            if (isKickDeviceOn == IS_CHIP_KICK_ON) {
                _isChipKickOut = true;
            }
            break;
        }
	}
    if ((pVision->allPlayer(_ballToucher).RawPos() - pVision->rawBall().Pos()).mod()<98) {
		_ballChipLine = CGeoLine(pVision->allPlayer(_ballToucher).RawPos(), pVision->allPlayer(_ballToucher).Dir());
	}
}


void CBallStatus::clearBallStateCouter(){
	memset(stateCouter,0,StateMaxNum*sizeof(int));
	_ballState=None;
}

void subStateJudge(int i,int* stateCouter,bool* enterCond,int* keepThreshold,int& _ballState){
	if (!enterCond[i]){
		stateCouter[i]--;
		if (stateCouter[i]==0){
			_ballState=None;
			memset(stateCouter,0,StateMaxNum*sizeof(int));
		}
	}else{
		stateCouter[i]++;
		stateCouter[i]=min(stateCouter[i],keepThreshold[i]);
	}
}
