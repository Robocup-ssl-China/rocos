//#define WIN32_LEAN_AND_MEAN
//#include "VisionReceiver.h"
//#include <CommandInterface.h>
//#include <OptionModule.h>
//#include <RefereeBoxIf.h>
//#include <playmode.h>
//#include "parammanager.h"
//#include "staticparams.h"
//#include <thread>
//#include <mutex>
//#include "Semaphore.h"
//#include "game_state.h"

//Semaphore visionEvent;
//Semaphore vision_receive_semaphore(0);
//VisionReceiver* VisionReceiver::_instance = 0;
//std::mutex* decisionMutex = new std::mutex(); ///<用来线程互斥，决策线程和图像获取后唤醒，同上 visionEvent 配合使用
//extern Semaphore vision_semaphore;
//Semaphore vision_finish_semaphore(0);

//namespace {
//std::thread *_thread = nullptr;
//std::thread *_thread2 = nullptr;
//bool DEBUG_MODE = false;
//COptionModule *_pOption = 0;
//CRefereeBoxInterface *_referee = 0; //裁判盒
//std::mutex* visionMutex = 0; ///<用来线程互斥，保证数据完整
//const int VISION_PORT = 12345;
//const int _RecentLogLength =  30; //seconds
//bool isYellow = false;
//}

//VisionReceiver::VisionReceiver(COptionModule *pOption): _threadAlive(false) {
//    _pOption = pOption;
//    _info.cycle = 0;


//    ///> 启动裁判盒
//    _referee = RefereeBoxInterface::Instance();
//    _referee->start();

//    ///> 初始化互斥锁
//    visionMutex = new std::mutex();

//    ///> 启动视觉接收线程
//    _thread = new std::thread([ = ] {getMessage();});
//    _thread->detach();
//    _threadAlive = true;

//    //配置vision接收端口
//    ZSS::ZParamManager::instance()->loadParam(isYellow, "ZAlert/IsYellow", false);
//    int port = ZSS::Athena::VISION_SEND[isYellow ? PARAM::YELLOW : PARAM::BLUE];
//    visionSocket = new QUdpSocket();
//    visionSocket->bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress);

//    _thread2 = new std::thread([ = ] {ReceiveVision();});

//}

//VisionReceiver::~VisionReceiver(void) {
//    if(visionMutex)
//        delete visionMutex;
//    if(_thread)
//        delete _thread;
//    if(_thread2)
//        delete _thread2;
//}

//VisionReceiver* VisionReceiver::instance(COptionModule *pOption) {
//    if(_instance == nullptr)
//        _instance = new VisionReceiver(pOption);
//    return _instance;
//}

//void VisionReceiver::destruct() {
//    if(_instance)
//        delete _instance;
//}

//void VisionReceiver::InitInfo(CServerInterface::VisualInfo &temp) {
//    for (int i = 0; i <  PARAM::Field::MAX_PLAYER; i++) {
//        temp.ourRobotIndex[i] = 0;
//        temp.player[i].dir = 0;
//        temp.player[i].rawdir = 0;
//        temp.player[i].pos.valid = false;
//        temp.player[i].pos.x = -999999;
//        temp.player[i].pos.y = -999999;
//        temp.player[i].rawPos.x = -999999;
//        temp.player[i].rawPos.y = -999999;
//        temp.player[i].dirvel = 0;
//        temp.player[i].vel = CVector(0, 0);

//        temp.theirRobotIndex[i] = 0;
//        temp.player[i + PARAM::Field::MAX_PLAYER].dir = 0;
//        temp.player[i + PARAM::Field::MAX_PLAYER].rawdir = 0;
//        temp.player[i + PARAM::Field::MAX_PLAYER].pos.valid = false;
//        temp.player[i + PARAM::Field::MAX_PLAYER].pos.x = -999999;
//        temp.player[i + PARAM::Field::MAX_PLAYER].pos.y = -999999;
//        temp.player[i + PARAM::Field::MAX_PLAYER].rawPos.x = -999999;
//        temp.player[i + PARAM::Field::MAX_PLAYER].rawPos.y = -999999;
//        temp.player[i + PARAM::Field::MAX_PLAYER].dirvel = 0;
//        temp.player[i + PARAM::Field::MAX_PLAYER].vel =  CVector(0, 0);
//    }
//}

//void VisionReceiver::ReceiveVision() {
//    int i;
//    QByteArray buffer;
//    int robots_blue_n, robots_yellow_n;
//    while (true) {
//        std::this_thread::sleep_for(std::chrono::milliseconds(2));
//        while (visionSocket->state() == QUdpSocket::BoundState && visionSocket->hasPendingDatagrams()) {
//            buffer.resize(visionSocket->pendingDatagramSize());
//            visionSocket->readDatagram(buffer.data(), buffer.size());
//            detectionFrame.ParseFromArray(buffer, buffer.size());

//            robots_blue_n = std::min(detectionFrame.robots_blue_size(), PARAM::SENDROBOTNUM);
//            robots_yellow_n = std::min(detectionFrame.robots_yellow_size(), PARAM::SENDROBOTNUM);
//            InitInfo(_info);

//            bool ballFound = detectionFrame.has_balls();
//            if (ballFound) {
//                auto & ball = detectionFrame.balls();
//                if (ball.x() < -30000 && ball.y() < -30000) {
//                    //理论上不应该进入这个if
//                    _info.ball.x = -32768;
//                    _info.ball.x = -32768;
//                    _info.ball.valid = false;
//                } else {
//                    _info.ball.x = ball.x() / 10;
//                    _info.ball.y = -ball.y() / 10;
//                    _info.rawBall.x = ball.raw_x() / 10;
//                    _info.rawBall.y = - ball.raw_y() / 10;
//                    _info.chipPredict.x =  ball.chip_predict_x() / 10;
//                    _info.chipPredict.y = -ball.chip_predict_y() / 10;
//                    _info.ball.valid = ball.valid();
//                    _info.BallState = (ballState)ball.ball_state();
//                    _info.BallLastTouch = ball.last_touch();
//                    _info.BallVel.setVector(ball.vel_x() / 10, -ball.vel_y() / 10);
//                }
//            } else {
//                std::cout << "Ball not Found!!!" << std::endl;
//            }
//            int index;
//            _pOption->MyColor() == TEAM_BLUE ? index = 0 : index = PARAM::Field::MAX_PLAYER;
//            //Blue car
//            for (i = 0; i < robots_blue_n; i++) {
//                auto& robot = detectionFrame.robots_blue(i);
//                auto id = robot.robot_id();
//                if (id < PARAM::Field::MAX_PLAYER) {
//                    index = _pOption->MyColor() == TEAM_BLUE ? id : id + PARAM::Field::MAX_PLAYER;
//                    _pOption->MyColor() == TEAM_BLUE ? _info.ourRobotIndex[index] = id + 1 :
//                            _info.theirRobotIndex[index - PARAM::Field::MAX_PLAYER] = id + 1;
//                    _info.player[index].pos.x = robot.x() / 10;
//                    _info.player[index].pos.y = -robot.y() / 10;
//                    _info.player[index].rawPos.x = robot.raw_x() / 10;
//                    _info.player[index].rawPos.y = -robot.raw_y() / 10;
//                    _info.player[index].pos.valid = robot.valid();
//                    _info.player[index].dir = -robot.orientation();
//                    _info.player[index].rawdir = - robot.raw_orientation();
//                    _info.player[index].vel.setVector(robot.vel_x() / 10, -robot.vel_y() / 10);
//                    _info.player[index].raw_vel.setVector(robot.raw_vel_x() / 10, -robot.raw_vel_y() / 10);
//                    _info.player[index].dirvel = -robot.rotate_vel();
//                    _info.player[index].raw_dirVel = -robot.raw_rotate_vel();
//                    _info.player[index].accelerate.setVector(robot.accelerate_x() / 10, -robot.accelerate_y() / 10);
//                }
//            }
//            //Yellow Car
//            _pOption->MyColor() == TEAM_BLUE ? index = PARAM::Field::MAX_PLAYER : index = 0;
//            for (i = 0; i < robots_yellow_n; i++) {
//                auto& robot = detectionFrame.robots_yellow(i);
//                auto id = robot.robot_id();
//                if (id >= 0 && id < PARAM::Field::MAX_PLAYER) {
//                    index = _pOption->MyColor() == TEAM_YELLOW ? id : id + PARAM::Field::MAX_PLAYER;
//                    _pOption->MyColor() == TEAM_YELLOW ? _info.ourRobotIndex[index] = id + 1 :
//                            _info.theirRobotIndex[index - PARAM::Field::MAX_PLAYER] = id + 1;
//                    _info.player[index].pos.x = robot.x() / 10;
//                    _info.player[index].pos.y = -robot.y() / 10;
//                    _info.player[index].rawPos.x = robot.raw_x() / 10;
//                    _info.player[index].rawPos.y = -robot.raw_y() / 10;
//                    _info.player[index].pos.valid = robot.valid();
//                    _info.player[index].dir = -robot.orientation();
//                    _info.player[index].rawdir = - robot.raw_orientation();
//                    _info.player[index].vel.setVector(robot.vel_x() / 10, -robot.vel_y() / 10);
//                    _info.player[index].raw_vel.setVector(robot.raw_vel_x() / 10, -robot.raw_vel_y() / 10);
//                    _info.player[index].dirvel = -robot.rotate_vel();
//                    _info.player[index].raw_dirVel = -robot.raw_rotate_vel();
//                    _info.player[index].accelerate.setVector(robot.accelerate_x() / 10, -robot.accelerate_y() / 10);
//                }
//            }
//            vision_receive_semaphore.Signal();
//        }
//    }
//}

//ZSS_THREAD_FUNCTION void VisionReceiver::getMessage() {
//    ///> 接受图像周期
//    static unsigned int cycle = 0;

//    ///> 图像信息接受，需要锁的配合，并进入循环
//    while (true) {
//        //cout << "1 : receiver " << endl;
//        //CServerInterface::VisualInfo temp = {};
//        RefRecvMsg _recvMsg;
//        vision_receive_semaphore.Wait();

//        decisionMutex->lock();		// 决策加锁
//        visionMutex->lock();		// 图像加锁
//        //(VisionReceiver::_visionMessage)->message2VisionInfo(temp);	// 图像接受线程转换图像数据
//        _info.mode = _referee->getPlayMode();		// 裁判盒命令接受线程得到裁判盒指令,并设置相应mode
//        _info.next_command = _referee->getNextCommand();
//        _info.ballPlacePosition.x = _referee->getBallPlacementPosX();
//        _info.ballPlacePosition.y = _referee->getBallPlacementPosY();
//        _info.ballPlacePosition.setValid(true);
//        _recvMsg.blueGoal = _referee->getBlueGoalNum();
//        _recvMsg.yellowGoal = _referee->getYellowGoalNum();
//        _recvMsg.timeRemain = _referee->getRemainTime();
//        _recvMsg.blueGoalie = _referee->getBlueGoalie();
//        _recvMsg.yellowGoalie = _referee->getYellowGoalie();

//        VisionReceiver::instance()->_info.cycle = ++cycle;  // 暂时这么代替，fix me!!! 没有用到图像里面的cycle,而是策略自己记录cycle,收到一个图像便加1;

//        visionMutex->unlock();		// 图像解锁
//        vision_finish_semaphore.Signal();
//        visionEvent.Signal();
//        decisionMutex->unlock();	// 决策解锁
//    }

////    ///> 退出
////    VisionReceiver::instance()->_threadAlive = false;
////    visionEvent.Signal();

////    return;
//}

//bool VisionReceiver::getVisionInfo(CServerInterface::VisualInfo & info, RefRecvMsg & msg) const {
//    if(_threadAlive) {
//        visionMutex->lock();		// 图像加锁
//        info = this->_info;			// 将得到的图像数据输出给info
//        msg  = this->_recvMsg;
//        visionMutex->unlock();		// 图像解锁
//    }

//    return _threadAlive;
//}
