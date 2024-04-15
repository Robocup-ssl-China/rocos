#ifndef _VISION_MODULE_H_
#define _VISION_MODULE_H_
#include "staticparams.h"
#include "WorldDefine.h"
#include "PlayerCommand.h"
#include "BallPredictor.h"
#include "RobotPredictor.h"
#include "game_state.h"
#include "ServerInterface.h"
#include "OptionModule.h"
#include "utils.h"
#include <vector>
#include "singleton.hpp"
#include <QUdpSocket>
#include "vision_detection.pb.h"
using namespace std;
/// @file   VisionModule.h
/// @author Yonghai Wu <liunian@zju.edu.cn>
/// @date   Tue Oct 13 14:26:36 2009
/// 
/// @brief  视觉预处理，包括滤波，预测等等。
///         
///     本文件主要是维护整个世界模型，最主要的是视觉信息，其他还有裁判
///  盒信息，和下位机通过双向通讯得到的机器人电机转速及其自身状态信息。
///  其中：
/// 
///   -  视觉的原始信息包括 10 个机器人和球的位置信息。视觉信息的处理主
/// 要包括:
///       -# 容错和滤波处理：由于原始数据包含测量噪声，有时还有信息不全
///    和错误的情况发生，所以需要先进行滤波和容错处理。这里采用的
///    kalman 滤波算法。
///       -# 预测：由于摄像机硬件和图像处理费时的原因，视觉信息存在 100
///    ms 左右的延时。所以必须进行预测才能得到机器人和球的真实位置。这
///    里总共尝试过基于神经网络的预测模型和线性预测模型。
///       -# 碰撞模型：在球被机器人挡住时，采用基于碰撞模型的算法对球的
///    位置进行估计。
/// 
///  - 对裁判盒信息的处理，这里主要是做了一层比较友好的包装，重新定义了
/// 读取的接口。
/// 
///  - 从下位机读取的信息具体有：
///      - 红外对管是否检测到球
///      - 射门指令是否被执行\n
///   红外的状态可以用来校正在球被机器人挡住时的碰撞模型，射门的执行情
///   况用来判断射门动作是否被真正执行，这对机器人状态的跳转非常重要。
/************************************************************************/
/*                       VisionModule                                   */
/************************************************************************/
class CVisionModule{
public:
	CVisionModule();
    ~CVisionModule(void);
    void startReceiveThread();
	void registerOption(const COptionModule* pOption);
    void setNewVision();

    int getValidNum() const {
        return _validNum;
    }
    int getTheirValidNum() const {
        return _TheirValidNum;
    }
    const PlayerVisionT& allPlayer(int num) const {
        return (num < PARAM::Field::MAX_PLAYER) ? ourPlayer(num) : theirPlayer(num - PARAM::Field::MAX_PLAYER);
    }

    const PlayerVisionT& ourPlayer(int num) const {
		if (Utils::PlayerNumValid(num)) {
            return _ourPlayerPredictor[num].getResult(_timeCycle);
		} else {
		//	std::cout<<"Player num:" << num << " [ ####### ] Get our player info Invalid !!!"<<std::endl;
			return _ourPlayerPredictor[0].getResult(_timeCycle);
		}
	}

    const PlayerVisionT& theirPlayer(int num) const {
		if (Utils::PlayerNumValid(num)) {
            return _theirPlayerPredictor[num].getResult(_timeCycle);
		} else {
		//	std::cout<<"Player num:" << num << " [ ####### ] Get their player info Invalid !!!"<<std::endl;
			return _theirPlayerPredictor[0].getResult(_timeCycle);
		}
	}

    const ObjectPoseT& ball() const {
        return _ballPredictor.getResult(_timeCycle);
    }

    const PlayerVisionT& ourPlayer(int cycle, int num) const {
        return _ourPlayerPredictor[num].getResult(cycle);
    }

    const PlayerVisionT& theirPlayer(int cycle, int num) const {
        return _theirPlayerPredictor[num].getResult(cycle);
    }

    const ObjectPoseT& ball(int cycle) const {
        return _ballPredictor.getResult(cycle);
    }

    const ObjectPoseT& rawBall() const {
        return _rawBallPos;
    }

    const RobotRawVisionData& ourRawPlayer(int num) const {
        return _ourPlayerPredictor[num].getRawData(_timeCycle);
    }

    const RobotRawVisionData& theirRawPlayer(int num) const {
        return _theirPlayerPredictor[num].getRawData(_timeCycle);
    }

    void setPlayerCommand(int num, const CPlayerCommand* pCmd);
    //比赛状态
    const GameState& gameState() const {
        return _gameState;
    }

    int playerLostTime(const int number) const {
        return _ourPlayerPredictor[number].lostTime();
    }

    int getCycle() const {
        return _timeCycle;
    }

    int getLastCycle() const {
        return _lastTimeCycle;
    }

    int getSide() const {
        return _pOption->MySide();
    }

    const std::string& getCurrentRefereeMsg()const;// include ball place
    const std::string& getLastRefereeMsg()const;
    const std::string& getNextRefereeMsg()const;
    const std::string& getLuaRefereeMsg()const; // if ball place, get next command

    int getOurGoal(){
        return _ourGoal;
    }
    int getTheirGoal(){
        return _theirGoal;
    }
    int  getTimeRemain(){
        return _refMsg.timeRemain;
    }

    const CVector getOurRawPlayerSpeed(int num) const {
        return _ourPlayerPredictor[num].getRawSpeed(_timeCycle);
    }
    const CVector getTheirRawPlayerSpeed(int num) const {
        return _theirPlayerPredictor[num].getRawSpeed(_timeCycle);
    }

    int getTheirPenaltyNum() {
        return _theirPenaltyNum;
    }
    void resetTheirPenaltyNum() {
        _theirPenaltyNum = 0;
    }
//	int GetTheirGoalieStrategyNum() {return _theirGoalieStrategyNum; }

    bool getBallVelStable(){
        return !_ballVelDirChanged;
    }
    const CGeoPoint& getBallPlacementPosition() const{
        return _ballPlacementPosition;
    }

	bool ballVelValid();
protected:
    void setCycle(int cycle){
        _lastTimeCycle = _timeCycle; _timeCycle = cycle;
    }
    void checkBothSidePlayerNum();
    void checkKickoffStatus(const CServerInterface::VisualInfo& info); // 检查开球状态
    void updateRefereeMsg();
    void updateNextRefereeMsg(const unsigned int);
	void judgeBallVelStable();
private:
    const COptionModule* _pOption;   			//比赛相关参数，是左半场还是右半场.
    static const int MAX_SAVE_CYCLES = 64;		//保存的最多周期数.
    GameState _gameState;						//比赛状态，裁判盒信息.

    //比赛相关的周期（时间）
    int _timeCycle;
    int _lastTimeCycle;
	
    //数据滤波器
    CBallPredictor _ballPredictor;										//球预测.
    CRobotPredictor _ourPlayerPredictor[PARAM::Field::MAX_PLAYER];    //我方机器人预测.
    CRobotPredictor _theirPlayerPredictor[PARAM::Field::MAX_PLAYER];	//对方机器人预测.

    //双方场上队员统计
    int _validNum;							//得到除守门员外我方的球员数
	int _TheirValidNum;						//得到对方的球员数

	bool _ballKicked;						//判断球是否被踢,用来决定比赛状态.
	CGeoPoint _ballPosSinceNotKicked;		//球在没有被踢之前的位置.
	ObjectPoseT _rawBallPos;				//这个周期球的原始数据，用来draw比较.
	
	ObjectPoseT _newBallPos; // the most useless varible I have ever seen.
	CGeoPoint _ballPos[10];  // and this more than the last line.

	ObjectPoseT _lastRawBallPos;			//上帧球的的原始数据
	// 保存数据
	bool _hasCollision;						//发生过碰撞,此时的处理特殊些(给下一个周期用).
    std::string _nextRefereeMsg;
    std::string _refereeMsg;
    std::string _lastRefereeMsg;
    std::string _realRefereeMsg;
	int _lastContactNum;

	int _ourGoal;
	int _theirGoal;
	int _ourGoalie;
	int _theirGoalie;
	int _last_xbox_pressed;
	int _theirPenaltyNum; // 对方第几个点球
    CGeoPoint _ballPlacementPosition;

	int _ballVelChangeCouter;
    bool _ballVelDirChanged;
    bool _sensorBall;

    CServerInterface::VisualInfo _info, visionTemp;
    RefRecvMsg _refMsg, refMsgTemp;
    QUdpSocket* visionSocket;
    Vision_DetectionFrame detectionFrame;

    void initVisionMsg(CServerInterface::VisualInfo &visionTemp);
    void receiveVisionMsg();
    void dealSpecialBall();
    void debugOnField();
};

typedef Singleton<CVisionModule> VisionModule;

#endif // _VISION_MODULE_H_
