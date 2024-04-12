#include <string>
#include <cmath>
#include "VisionModule.h"
#include "utils.h"
#include "playmode.h"
#include "parammanager.h"
#include "DribbleStatus.h"
#include "RobotCapability.h"
#include "TaskMediator.h"
#include "KickStatus.h"
#include "PlayInterface.h"
#include <fstream>
#include "RobotSensor.h"
#include "GDebugEngine.h"
#include "RobotsCollision.h"
#include "Semaphore.h"
#include "staticparams.h"
#include "staticparams.h"
#include "RefereeBoxIf.h"
using namespace std;
Semaphore vision_to_decision(0);
Semaphore visionEvent;
std::mutex decisionMutex;
std::mutex visionMutex;

#ifdef USE_CUDA_MODULE
Semaphore vision_to_cuda(0);
#endif

namespace {
//    bool VERBOSE_MODE = false; // 输出信息
    bool IS_SIMULATION = false; // 是否仿真
    bool isYellow = false;

    std::thread* visionThread = nullptr;
    //	XBoxController xboxCtrl(1);
}

CVisionModule::CVisionModule()
    : _timeCycle(0)
    , _lastTimeCycle(0)
    , _validNum(0)
    , _TheirValidNum(0)
    , _ballKicked(false)
    , _ourGoalie(0)
    , _theirGoalie(0)
    , _sensorBall(false)
{
    ZSS::ZParamManager::instance()->loadParam(IS_SIMULATION, "Alert/IsSimulation", false);

    for (int i = 0; i < PARAM::Field::MAX_PLAYER; i++) {
        //恢复初始化对方车预测为能识别角度信息
        _theirPlayerPredictor[i].setIsHasRotation(false);
    }

    _last_xbox_pressed = PMNone;

    _theirPenaltyNum = 0;
    _ballVelChangeCouter = true;
}

void CVisionModule::registerOption(const COptionModule* pOption) {
    _pOption = pOption;
    _gameState.init(pOption->MyColor());
}

CVisionModule::~CVisionModule(void) {
    delete visionSocket;
    visionSocket = nullptr;
    delete visionThread;
    visionThread = nullptr;
}

void CVisionModule::startReceiveThread() {
    ZSS::ZParamManager::instance()->loadParam(isYellow, "ZAlert/IsYellow", false);
    int port = ZSS::Athena::VISION_SEND[isYellow ? PARAM::YELLOW : PARAM::BLUE];
    visionSocket = new QUdpSocket();
    visionSocket->bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress);
    visionThread = new std::thread([=] {receiveVisionMsg();});
    visionThread->detach();
    RefereeBoxInterface::Instance()->start();
}

void CVisionModule::initVisionMsg(CServerInterface::VisualInfo& temp) {
    for (int color = 0; color < PARAM::TEAMS; color++) {
        for (int i = 0; i <  PARAM::Field::MAX_PLAYER; i++) {
            temp.player[color][i].dir = 0;
            temp.player[color][i].rawdir = 0;
            temp.player[color][i].pos.valid = false;
            temp.player[color][i].pos.x = -999999;
            temp.player[color][i].pos.y = -999999;
            temp.player[color][i].rawPos.x = -999999;
            temp.player[color][i].rawPos.y = -999999;
            temp.player[color][i].dirvel = 0;
            temp.player[color][i].vel = CVector(0, 0);
        }
    }
}

void CVisionModule::receiveVisionMsg() {
    QByteArray buffer;
    int robots_blue_n, robots_yellow_n;
    static unsigned int cycle = 0;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        while (visionSocket->state() == QUdpSocket::BoundState && visionSocket->hasPendingDatagrams()) {
            buffer.resize(visionSocket->pendingDatagramSize());
            visionSocket->readDatagram(buffer.data(), buffer.size());
            detectionFrame.ParseFromArray(buffer, buffer.size());

            robots_blue_n = std::min(detectionFrame.robots_blue_size(), PARAM::ROBOTNUM);
            robots_yellow_n = std::min(detectionFrame.robots_yellow_size(), PARAM::ROBOTNUM);
            visionMutex.lock();		// 图像加锁
            initVisionMsg(visionTemp);

            bool ballFound = detectionFrame.has_balls();
            if (ballFound) {
                auto & ball = detectionFrame.balls();
                if (ball.x() > 88888 && ball.y() > 88888) {
                    //理论上不应该进入这个if
                    visionTemp.ball.x = -32768;
                    visionTemp.ball.x = -32768;
                    visionTemp.ball.valid = false;
                } else {
                    visionTemp.ball.x = ball.x();
                    visionTemp.ball.y = ball.y();
                    visionTemp.rawBall.x = ball.raw_x();
                    visionTemp.rawBall.y = ball.raw_y();
                    visionTemp.chipPredict.x = ball.chip_predict_x();
                    visionTemp.chipPredict.y = ball.chip_predict_y();
                    visionTemp.ball.valid = ball.valid();
                    visionTemp.BallState = (ballState)ball.ball_state();
                    visionTemp.BallLastTouch = ball.last_touch();
                    visionTemp.BallVel.setVector(ball.vel_x(), ball.vel_y());
                }
            } else {
                std::cout << "Ball not Found!!!" << std::endl;
            }
            int index = 0;
            for (int color = 0; color < PARAM::TEAMS; color++) {
                bool ourRobot = (color == _pOption->MyColor());
                int robot_size = (color == PARAM::BLUE) ? detectionFrame.robots_blue_size() : detectionFrame.robots_yellow_size();
                for (int i = 0; i < PARAM::ROBOTNUM; i++) {
                    if(i >= robot_size) break;
                    auto& robot = (color==PARAM::BLUE) ? detectionFrame.robots_blue(i) : detectionFrame.robots_yellow(i);
                    int ourOrTheir = ourRobot ? 0 : 1;
                    if (!robot.valid()) continue;
                    index = robot.robot_id();
                    visionTemp.player[ourOrTheir][index].pos.x = robot.x();
                    visionTemp.player[ourOrTheir][index].pos.y = robot.y();
                    visionTemp.player[ourOrTheir][index].rawPos.x = robot.raw_x();
                    visionTemp.player[ourOrTheir][index].rawPos.y = robot.raw_y();
                    visionTemp.player[ourOrTheir][index].pos.valid = robot.valid();
                    visionTemp.player[ourOrTheir][index].dir = robot.orientation();
                    visionTemp.player[ourOrTheir][index].rawdir = robot.raw_orientation();
                    visionTemp.player[ourOrTheir][index].vel.setVector(robot.vel_x(), robot.vel_y());
                    visionTemp.player[ourOrTheir][index].raw_vel.setVector(robot.raw_vel_x(), robot.raw_vel_y());
                    visionTemp.player[ourOrTheir][index].dirvel = robot.rotate_vel();
                    visionTemp.player[ourOrTheir][index].raw_dirVel = robot.raw_rotate_vel();
                    visionTemp.player[ourOrTheir][index].accelerate.setVector(robot.accelerate_x(), robot.accelerate_y());
                }
            }
//            decisionMutex.lock();		// 决策加锁
            auto refBox = RefereeBoxInterface::Instance();
            //(VisionReceiver::_visionMessage)->message2VisionInfo(temp);	// 图像接受线程转换图像数据
            visionTemp.mode = refBox->getPlayMode();		// 裁判盒命令接受线程得到裁判盒指令,并设置相应mode
            visionTemp.next_command = refBox->getNextCommand();
            visionTemp.ballPlacePosition.x = refBox->getBallPlacementPosX();
            visionTemp.ballPlacePosition.y = refBox->getBallPlacementPosY();
            visionTemp.ballPlacePosition.setValid(true);
            refMsgTemp.blueGoal = refBox->getBlueGoalNum();
            refMsgTemp.yellowGoal = refBox->getYellowGoalNum();
            refMsgTemp.timeRemain = refBox->getRemainTime();
            refMsgTemp.blueGoalie = refBox->getBlueGoalie();
            refMsgTemp.yellowGoalie = refBox->getYellowGoalie();

            visionTemp.cycle = ++cycle;  // 没有用到图像里面的cycle,而是策略自己记录cycle,收到一个图像便加1;

            visionMutex.unlock();		// 图像解锁
            visionEvent.Signal();
//            decisionMutex.unlock();	// 决策解锁
        }
    }
}

void CVisionModule::setNewVision() {
    //////////////////////////////////////////////////////////////////////////
    //  重要说明，请注意
    //	定义我方小车位置在左侧，对方车位置在右侧，笛卡尔坐标定义<x y theta>
    //	比赛时不管是左边还是右边，对于策略处理而言统一定义为我方在左半场
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    // 默认我方在球场的左边，切记
    visionEvent.Wait();
    visionMutex.lock();
    _info = visionTemp;
    _refMsg = refMsgTemp;
    visionMutex.unlock();
    const bool invert = !(_pOption->MySide() == PARAM::Field::POS_SIDE_LEFT);

    if (_pOption->MyColor() == PARAM::BLUE) {
        _ourGoal = _refMsg.blueGoal;
        _theirGoal = _refMsg.yellowGoal;
        _ourGoalie = _refMsg.blueGoalie;
        _theirGoalie = _refMsg.yellowGoalie;
    } else {
        _ourGoal = _refMsg.yellowGoal;
        _theirGoal = _refMsg.blueGoal;
        _ourGoalie = _refMsg.yellowGoalie;
        _theirGoalie = _refMsg.blueGoalie;
    }
    /////////////////////////////////////////////////////////////////////////////
    /// @brief Step 0. 比赛开始时，记录调试信息，辅助调试
    /////////////////////////////////////////////////////////////////////////////
    // 更新当前的时间周期
    setCycle(_info.cycle);

    // 更新新图像数据中的原始球信息
    _lastRawBallPos = _rawBallPos;
    _rawBallPos.SetValid(_info.ball.valid);
    // add by zhyaic 2013.6.3 原始的位置应该反向
    const int invertFactor = invert ? -1 : 1;
    _rawBallPos.SetPos(_info.ball.x * invertFactor, _info.ball.y * invertFactor);

    /////////////////////////////////////////////////////////////////////////////
    /// @brief Step 1. 进行球预测,也就是输入当前球的观测进行滤波
    /////////////////////////////////////////////////////////////////////////////
    _ballPredictor.updateVision(_info, invert);

    //printf("%d\n", RobotCapFactory::Instance()->getRobotCap(0,2)->maxSpeed(0));
    /////////////////////////////////////////////////////////////////////////////
    /// @brief Step 2: 进行我方和对方机器人位置预测，关注滤波器
    /////////////////////////////////////////////////////////////////////////////
    //【#TODO】机器人碰撞检测器清空，
    const ObjectPoseT& thisBall = _ballPredictor.getData(_timeCycle);
    // 确定球员的信息是否需要反向，确保正确更新比赛双方球员的信息
    for (int i = 0; i < PARAM::Field::MAX_PLAYER; ++ i) {
        const VehicleInfoT& ourPlayer = _info.player[0][i];
        const VehicleInfoT& theirPlayer = _info.player[1][i];
        _ourPlayerPredictor[i].updateVision(_info.cycle, ourPlayer, thisBall, invert);
        _theirPlayerPredictor[i].updateVision(_info.cycle, theirPlayer, thisBall, invert);
    }

    //更新双方当前在场上的球员数量，我方排除门将，对方全部
    checkBothSidePlayerNum();

    /////////////////////////////////////////////////////////////////////////////
    /// @brief Step 3: 更新双向通讯的数据，仅针对实物模式
    /////////////////////////////////////////////////////////////////////////////
    RobotSensor::Instance()->Update(this);

    /////////////////////////////////////////////////////////////////////////////
    /// @brief Step 4: 碰撞检测
    /////////////////////////////////////////////////////////////////////////////
    // TODO
    /////////////////////////////////////////////////////////////////////////////
    /// @brief Step 6: 更新裁判盒信息 及处理球数据相关的特殊情况
    /////////////////////////////////////////////////////////////////////////////
    checkKickoffStatus(_info);
    int ref_mode = _info.mode;
    _last_xbox_pressed = ref_mode;

    // 更新裁判盒信息，一般当且仅当比赛模式为停球状态时，判断球是否被踢出
    if (ref_mode >= PMStop && ref_mode < PMNone) {
        _gameState.transition(playModePair[ref_mode].ch, _ballKicked,_info.next_command);
    }

    //更新裁判盒信息
    updateRefereeMsg();

    dealSpecialBall();

    //for lua
    _ballPlacementPosition.setX(_info.ballPlacePosition.x);
    _ballPlacementPosition.setY(_info.ballPlacePosition.y);

    /////////////////////////////////////////////////////////////////////////////
    /// @brief Step 7: 向调试面板中显示一些必要的信息
    /////////////////////////////////////////////////////////////////////////////
    debugOnField();

    vision_to_decision.Signal();
#ifdef USE_CUDA_MODULE
    vision_to_cuda.Signal();
#endif
}

void CVisionModule::checkKickoffStatus(const CServerInterface::VisualInfo& info) {
    if (_gameState.canEitherKickBall()) {	// 若允许去踢球
        if (! _ballKicked ) {	// 球没有被判断为踢出
            if (gameState().ourRestart()) {
                const double OUR_BALL_KICKEDBUFFER = 50 + 30 + 10;
                const CVector ballMoved = ball().Pos() - _ballPosSinceNotKicked;
                if( ballMoved.mod2() > OUR_BALL_KICKEDBUFFER * OUR_BALL_KICKEDBUFFER ) {
                    _ballKicked = true;
                }
            } else {
//                CBestPlayer::PlayerList theirList =  BestPlayer::Instance()->theirFastestPlayerToBallList();
//                if (theirList.empty()) {
//                    _ballKicked = false;
//                } else {
                    const double THEIR_BALL_KICKED_BUFFER = 50 + 50;
                    const CVector ballMoved = ball().Pos() - _ballPosSinceNotKicked;
                    if( ballMoved.mod2() > THEIR_BALL_KICKED_BUFFER * THEIR_BALL_KICKED_BUFFER ) {
                        _ballKicked = true;
                    }
//                }
            }

        }
    } else {					// 球已经被判断为踢出
        _ballKicked = false;
        _ballPosSinceNotKicked = ball().Pos();
    }

    return ;
}

void CVisionModule::checkBothSidePlayerNum() {
    // 统计我方实际在场上的小车个数 except goalie
    _validNum = 0;
    int tempGoalieNum = TaskMediator::Instance()->goalie();
    for (int i = 0; i < PARAM::Field::MAX_PLAYER; i++) {
        if (ourPlayer(i).Valid() && i != tempGoalieNum) {
            _validNum++;
        }
    }
    _validNum = _validNum > (PARAM::Field::MAX_PLAYER - 1) ? (PARAM::Field::MAX_PLAYER - 1) : _validNum;

    // 统计对方实际在场上的小车个数
    _TheirValidNum = 0;
    for (int i = 0; i < PARAM::Field::MAX_PLAYER; i++) {
        if (theirPlayer(i).Valid())	{
            _TheirValidNum ++;
        }
    }
    _TheirValidNum = _TheirValidNum > PARAM::Field::MAX_PLAYER ? PARAM::Field::MAX_PLAYER : _TheirValidNum;

    return;
}

void CVisionModule::setPlayerCommand(int num, const CPlayerCommand* pCmd) {
    _ourPlayerPredictor[num].updateCommand(_timeCycle, pCmd);
    CDribbleStatus* dribbleStatus = DribbleStatus::Instance();
    if( pCmd->dribble() ) {
        dribbleStatus->setDribbleOn(pCmd->number(), _timeCycle, ball().Pos());
    } else {
        dribbleStatus->setDribbleOff(pCmd->number());
    }

    return ;
}

void CVisionModule::updateRefereeMsg() {
    updateNextRefereeMsg(_gameState._next_command);
    if (_lastRefereeMsg != _refereeMsg && _refereeMsg == "TheirPenaltyKick") { // 记录当前是对方第几个点球
        _theirPenaltyNum++;
    }

    _lastRefereeMsg = _refereeMsg;
    //std::cout << "check BallPlaceMent : " << (_gameState.ballPlacement() ? "true" : "false") << std::endl;

    if (! _gameState.canMove()) {
        _refereeMsg = "GameHalt";
    } else if( _gameState.gameOver()) {
        _refereeMsg = "GameOver";
    } else if( _gameState.isOurTimeout() || _gameState.isTheirTimeout()) {
        _refereeMsg = "OurTimeout";
    } else if(!_gameState.allowedNearBall()) {
        // 对方发球
        if(_gameState.theirIndirectKick()) {
            _refereeMsg = "TheirIndirectKick";
        } else if (_gameState.theirDirectKick()) {
            _refereeMsg = "TheirIndirectKick";
        } else if (_gameState.theirKickoff()) {
            _refereeMsg = "TheirKickOff";
        } else if (_gameState.theirPenaltyKick()) {
            _refereeMsg = "TheirPenaltyKick";
        } else if (_gameState.theirBallPlacement()) {
            if(_nextRefereeMsg == ""){
                _refereeMsg = "TheirBallPlacement";
            }else{
                _refereeMsg = _nextRefereeMsg;
            }
        } else {
            _refereeMsg = "GameStop";
        }
    } else if( _gameState.ourRestart()) {
        if( _gameState.ourKickoff() ) {
            _refereeMsg = "OurKickOff";
        } else if(_gameState.penaltyKick()) {
            _refereeMsg = "OurPenaltyKick";
        } else if(_gameState.ourIndirectKick()) {
            _refereeMsg = "OurIndirectKick";
        } else if(_gameState.ourDirectKick()) {
            _refereeMsg = "OurIndirectKick";
        }
    } else if (_gameState.ourBallPlacement()) {
        if(_nextRefereeMsg == ""){
            _refereeMsg = "OurBallPlacement";
        }else{
            _refereeMsg = _nextRefereeMsg;
        }
    } else {
        _refereeMsg = "";
    }
    _realRefereeMsg = _refereeMsg;
    if(_gameState.ourBallPlacement()){
        _realRefereeMsg = "OurBallPlacement";
    }else if(_gameState.theirBallPlacement()){
        _realRefereeMsg = "TheirBallPlacement";
    }
}
void CVisionModule::updateNextRefereeMsg(const unsigned int command){
    const static string Command2Message[] = {
        "",//"GameHalt",
        "",//"GameStop",
        "",//"KickOff",
        "",//"PenaltyKick",
        "IndirectKick",
        "IndirectKick",
        "",//"Timeout",
        "",
        "",//"BallPlacement"
    };
    if(command < 2){
        _nextRefereeMsg = Command2Message[command];
    }else if(command < 4){
        _nextRefereeMsg = "";
    }else if(command < 18){
        if(Command2Message[int(command/2)]==""){
            _nextRefereeMsg = "";
        }else {
            _nextRefereeMsg = (((_pOption->MyColor() == TEAM_YELLOW?0:1)==command%2)?"Our":"Their")+Command2Message[int(command/2)];
        }
    }else{
        _nextRefereeMsg = "";
    }
}
const string& CVisionModule::getNextRefereeMsg() const {
    return _nextRefereeMsg;
}

const string& CVisionModule::getCurrentRefereeMsg() const {
    return _realRefereeMsg;
}

const string& CVisionModule::getLastRefereeMsg() const {
    return _lastRefereeMsg;
}
const string& CVisionModule::getLuaRefereeMsg() const {
    return _refereeMsg;
}


void CVisionModule::judgeBallVelStable() {
    if (ballVelValid()) {
        if (fabs(Utils::Normalize(this->ball().Vel().dir() - this->ball(_lastTimeCycle - 2).Vel().dir())) > PARAM::Math::PI * 10 / 180 && this->ball().Vel().mod() > 20) {
            _ballVelChangeCouter++;
            _ballVelChangeCouter = min(_ballVelChangeCouter, 4);
        } else {
            _ballVelChangeCouter--;
            _ballVelChangeCouter = max(_ballVelChangeCouter, 0);
        }
        if (_ballVelChangeCouter >= 3) {
            _ballVelDirChanged = true;
        }
        if (_ballVelChangeCouter == 0) {
            _ballVelDirChanged = false;
        }
    }

}

bool CVisionModule::ballVelValid() {
    if (!_rawBallPos.Valid() || fabs(_rawBallPos.X() - _lastRawBallPos.X()) < 0.0000000001 ) {
        return false;
    } else {
        return true;
    }
}

void CVisionModule::dealSpecialBall() {
    // 特殊情况一
    // 一些特殊比赛状态下，对于球的特殊处理，与场地的尺寸参数相关
    // 一般要求球看不到才予以处理
    if (!IS_SIMULATION) {
        if (_gameState.kickoff()) {				// 开球时
            if (!ball().Valid()/* || Ball().Pos().dist(CGeoPoint(0,0)) > 20*/) {
                _ballPredictor.setPos(_timeCycle, CGeoPoint(0, 0));
                _ballPredictor.setVel(_timeCycle, CVector(0, 0));
            }
        }
        double penaltyX = PARAM::Field::PENALTY_MARK_X;

        if (_gameState.ourPenaltyKick()) {		// 我方点球时
            if (!ball().Valid()/* || Ball().Pos().dist(CGeoPoint(penaltyX,0)) > 20*/) {
                _ballPredictor.setPos(_timeCycle, CGeoPoint(penaltyX, 0));
                _ballPredictor.setVel(_timeCycle, CVector(0, 0));
            }
        }

        if (_gameState.theirPenaltyKick()) {	// 对方点球时
            if (!ball().Valid()/* || Ball().Pos().dist(CGeoPoint(-penaltyX,0)) > 20*/) {
                _ballPredictor.setPos(CGeoPoint(-penaltyX, 0));
                _ballPredictor.setVel(_timeCycle, CVector(0, 0));
            }
        }
    }

    // 特殊情况二：
    // 红外有信息，若球没看到，则予以位置修正
    _sensorBall = false;
    for (int i = 0; i < PARAM::Field::MAX_PLAYER; i ++) {
        if (RobotSensor::Instance()->IsInfraredOn(i)) {
            _sensorBall = true;
            if (ball().Valid()) {	// 球看到，作红外信号假象检查，因为通讯可能会丢
            } else {				// 球看不到，根据红外信号纠正球的位置
                _ballPredictor.setPos(ourPlayer(i).Pos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER, ourPlayer(i).Dir()));//before 8.5
                _ballPredictor.setVel(_timeCycle, CVector(0, 0));
            }
            break;
        }
    }
}

void CVisionModule::debugOnField() {
//    // 输出当前球的预测速度 并 输出数值直观显示
//    GDebugEngine::Instance()->gui_debug_line(this->ball().Pos(), this->ball().Pos() + this->ball().Vel(), COLOR_ORANGE);
//    const double outballspeed = ball().Vel().mod();
//    QString velbuf = QString::number(outballspeed);
//    auto&& field_width = PARAM::Field::PITCH_WIDTH;
//    auto&& field_length = PARAM::Field::PITCH_LENGTH;
//    if (outballspeed <= 650) {
//        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-field_length / 2.3, field_width / 2.3), velbuf.toLatin1(), COLOR_BLACK);
//    } else {
//        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-field_length / 2.3, field_width / 2.3), velbuf.toLatin1(), COLOR_RED);
//    }

    // 输出我方小车的红外信号
    if (_sensorBall) {
        GDebugEngine::Instance()->gui_debug_arc(ball().Pos(), 4 * PARAM::Field::BALL_SIZE, 0, 360, COLOR_PURPLE);
        GDebugEngine::Instance()->gui_debug_arc(ball().Pos(), 2 * PARAM::Field::BALL_SIZE, 0, 360, COLOR_PURPLE);
    }
}
