#include "RobotPredictor.h"
#include <utils.h>
#include "RobotsCollision.h"
#include <string.h>
using namespace std;
//namespace {
////const int MAX_ROBOT_LOST_TIME = int(0.5 * PARAM::Vision::FRAME_RATE + 0.5);

////// 由于图像的一次事故，帧率和系统延时改变了
////const double TOTAL_MOV_LATED_FRAME = 4.2f; //平移的延时(原来为5.4)
////const int NUM_MOV_LATENCY_FRAMES = static_cast<int>(TOTAL_MOV_LATED_FRAME);
////const float MOV_LATENCY_FRACTION  = TOTAL_MOV_LATED_FRAME - static_cast<float>(NUM_MOV_LATENCY_FRAMES);

////const double TOTAL_ROT_LATED_FRAME = 3.5f;//1.5f; // 转动的延时(原来为4.0)
////const int NUM_ROT_LATENCY_FRAMES = static_cast<int>(TOTAL_ROT_LATED_FRAME);
////const float ROT_LATENCY_FRACTION  = TOTAL_ROT_LATED_FRAME - static_cast<float>(NUM_ROT_LATENCY_FRAMES);

////const float MAX_SPEED = 500;
////const float ZERRO_SPEED = 8;

////double beta = 0.6;
//}
CRobotPredictor::CRobotPredictor() : _robotLostTime(0), _isHasRotation(true) {
//    _robotFilter.initialize(PARAM::File::RobotPosFilterDir + "slowMatrices.txt", PARAM::File::RobotPosFilterDir + "fastMatrices.txt");
//    _robotRotationFilter.initialize(PARAM::File::RobotRotFilterDir + "slowMatrices.txt", PARAM::File::RobotRotFilterDir + "fastMatrices.txt");
}

CRobotPredictor::CRobotPredictor(bool isHasRotation) : _robotLostTime(0), _isHasRotation(isHasRotation) {
    // _isHasRotation将在visionModule的构造函数里被设定, 并针对其值构造具有(或不具有)朝向的对手的CRobotPredictor对象
//    if (_isHasRotation) {
//        _robotFilter.initialize(PARAM::File::RobotPosFilterDir + "slowMatrices.txt", PARAM::File::RobotPosFilterDir + "fastMatrices.txt");
//        _robotRotationFilter.initialize(PARAM::File::RobotRotFilterDir + "slowMatrices.txt", PARAM::File::RobotRotFilterDir + "fastMatrices.txt");
//    } else {
//        _robotFilter.initialize(PARAM::File::RobotPosFilterDir + "slowMatrices.txt", PARAM::File::RobotPosFilterDir + "fastMatrices.txt");
//    }
}
void CRobotPredictor::updateVision(int cycle, const VehicleInfoT& player, const ObjectPoseT& ball, bool invert) {
    // !!!!这里好像有问题:
    // 如果是对手车也能识别朝向,就会调用updateOurVision, 但因为在该函数里取不到cmd(只有我方车才有cmd存下来), 结果就不对.
    // 这里需要一个辨别我方车与对方车的方式!!
    RobotVisionData& predictedVision = _visionLogger.getVision(cycle);
    const double x = invert ? -player.pos.x : player.pos.x;
    const double y = invert ? -player.pos.y : player.pos.y;
    const double raw_x = invert ? -player.rawPos.x : player.rawPos.x;
    const double raw_y = invert ? -player.rawPos.y : player.rawPos.y;
    const double vel_x = invert ? -player.vel.x() : player.vel.x();
    const double vel_y = invert ? -player.vel.y() : player.vel.y();
    const double raw_vel_x = invert ? -player.raw_vel.x() : player.raw_vel.x();
    const double raw_vel_y = invert ? -player.raw_vel.y() : player.raw_vel.y();
    const double acc_x = invert ? -player.accelerate.x() : player.accelerate.x();
    const double acc_y = invert ? -player.accelerate.y() : player.accelerate.y();
    //std::cout << "Id:" << realNum << "\tPos: " << x << "," << y << "\tVel:" << player.vel
    //         << "\tdir:" << player.dir << std::endl;
    double dir, rawdir, dirVel, rawDirVel;

    dir = invert ? Utils::Normalize(player.dir + PARAM::Math::PI) : Utils::Normalize(player.dir);
    rawdir = invert ? Utils::Normalize(player.rawdir + PARAM::Math::PI) : Utils::Normalize(player.rawdir);

    if(fabs(player.dirvel) > 6 || fabs(player.raw_dirVel) > 6) {
        dirVel = 0;
        rawDirVel = 0;
    } else {
        dirVel = player.dirvel;
        rawDirVel = player.raw_dirVel;
    }

    _collisionSimulator.reset(ball.RawPos(), ball.Vel()); // 初始化球的信息(对手没有朝向信息)

    predictedVision.cycle = cycle;
    predictedVision.SetRawPos(raw_x, raw_y);
    predictedVision.SetPos(x, y);
    predictedVision.SetDir(dir);
    predictedVision.SetRawDir(rawdir);
    predictedVision.SetValid(player.pos.valid);
    predictedVision.SetAcc(CVector(acc_x, acc_y));
    predictedVision.SetRawVel(CVector(raw_vel_x, raw_vel_y));
    predictedVision.SetRawRotVel(rawDirVel);

    if (_isHasRotation) {//our robot
        if(_commandLogger.commandValid(cycle - 1))
            cmd = _commandLogger.getCommand(cycle - 1);
        else {
            cmd.rot_speed = 0;
            cmd.vel = CVector(0, 0);
        }
        // 用前一帧的命令当做当前的速度(考虑图像实际测得的速度!!! 实验结果表明: 考虑图像速度后整体会慢下来,效果不好)
        CVector updateVel = cmd.vel.rotate(_visionLogger.getVision(cycle - 1).Dir()) * 1;
        predictedVision.SetVel(updateVel);
        // modified by zhyaic 2014.3.26, 此处不能用下发的旋转速度做为此时机器人的旋转速度，尤其是当机器人在y方向有较大分量时
        // 这个地方应引起注意!!!!!!
        predictedVision.SetRotVel(cmd.rot_speed);
    } else {
        predictedVision.SetVel(CVector(vel_x, vel_y));
        predictedVision.SetRotVel(dirVel);
    }
}

//void CRobotPredictor::updateOurVision(int cycle, const VehicleInfoT& player, const ObjectPoseT& ball, bool invert, int realNum) {
//    const double x = invert ? -player.pos.x : player.pos.x;
//    const double y = invert ? -player.pos.y : player.pos.y;
//    double dir = 0;
//    if(fabs(player.dir) > 6) {
//        dir = 0;
//    } else {
//        dir = invert ? Utils::Normalize(player.dir + PARAM::Math::PI) : player.dir;
//    }

//    const bool seen = player.pos.valid;
//    const CGeoPoint pos(x, y);

//    updateRawVision(cycle, x, y, dir, seen); // 处理原始视觉数据
//    _collisionSimulator.reset(ball.RawPos(), ball.Vel()); // 初始化球的信息
//    RobotVisionData& predictedVision = _visionLogger.getVision(cycle);	// 注意: 这里是引用,在predictedVision上的操作相当于在_visionLogger相应帧的图像数据上操作
//    predictedVision.cycle = cycle; // 初始化
//    predictedVision.realNum = realNum;


//    if( seen ) {
//        predictedVision.SetRawPos(x, y);
//    } else { // 没看到车，猜测一个原始位置
//        for( int i = 1; i < PARAM::Field::MAX_PLAYER; ++i ) {
//            if( _visionLogger.visionValid(cycle - i) ) {
//                for( int j = cycle - i; j < cycle; ++j ) {
//                    const RobotVisionData& previousVision = _visionLogger.getVision(j);
//                    RobotVisionData& nextVision = _visionLogger.getVision(j + 1);
//                    if( _visionLogger.getVision(j - NUM_MOV_LATENCY_FRAMES).Valid() ) { //j-5
//                        nextVision.SetRawPos(previousVision.RawPos() + _visionLogger.getVision(j - NUM_MOV_LATENCY_FRAMES).Vel() / PARAM::Vision::FRAME_RATE);
//                    } else {
//                        nextVision.SetRawPos(previousVision.RawPos());
//                    }
//                    nextVision.cycle = j + 1;
//                }
//                break;
//            }
//        }
//    }
//    if( seen && checkValid(cycle, pos) ) {
//        _robotLostTime = 0;

//        // 用简单线性模型预测
//        // 先设置初始状态
//        const RobotRawVisionData& fliteredVision = _filterData.getVision(cycle);
//        const bool ballClose = (pos - ball.RawPos()).mod2() < 2500; // 距离近的时候才考虑和球的碰撞
//        RobotCommandEffect cmd;
//        predictedVision.SetPos(CGeoPoint(fliteredVision.x, fliteredVision.y));
//        predictedVision.SetDir(fliteredVision.dir);

//        if(_commandLogger.commandValid(cycle - 1))
//            cmd = _commandLogger.getCommand(cycle - 1);
//        else {
//            //std::cout<<"1.prediction vel equal zero"<<endl;
//            cmd.rot_speed = 0;
//            cmd.vel = CVector(0, 0);
//        }

//        // 用前一帧的命令当做当前的速度(是否考虑图像实际测得的速度??? 实验结果表明: 考虑图像速度后整体会慢下来,效果不好)
//        double rate = 0;
//        CVector updateVel = CVector(fliteredVision.xVel, fliteredVision.yVel) * rate + cmd.vel.rotate(_visionLogger.getVision(cycle - 1).Dir()) * (1 - rate);
//        predictedVision.SetVel(updateVel);

//        // modified by zhyaic 2014.3.26, 此处不能用下发的旋转速度做为此时机器人的旋转速度，尤其是当机器人在y方向有较大分量时
//        // 这个地方应引起注意
//        double updateRotVel = cmd.rot_speed;
//        predictedVision.SetRotVel(updateRotVel);

//        // 位置预测
//        // 延迟帧数的小数部分
//        bool collision_with_opp = false;
//        if (MOV_LATENCY_FRACTION > 0) {
//            RobotCommandEffect cmd;
//            if(_commandLogger.commandValid(cycle - 1 - NUM_MOV_LATENCY_FRAMES))
//                cmd = _commandLogger.getCommand(cycle - 1 - NUM_MOV_LATENCY_FRAMES) ;
//            else {
//                //std::cout<<"2.prediction vel equal zero"<<endl;
//                cmd.rot_speed = 0;
//                cmd.vel = CVector(0, 0);
//            }
//            if( ballClose) {
//                _collisionSimulator.simulate(predictedVision, 1.0 / PARAM::Vision::FRAME_RATE);
//            }
//            /*if(print)
//                std::cout << "cmd " << cmd.vel.rotate(_visionLogger.getVision(cycle-1-NUM_LATENCY_FRAMES).Dir()) << "\t" << predictedVision.Pos() << std::endl;*/
//            double dir = _visionLogger.getVision(cycle - 1 - NUM_MOV_LATENCY_FRAMES).Dir();
//            double timeLength = MOV_LATENCY_FRACTION / PARAM::Vision::FRAME_RATE;
//            //cout << timeLength<< endl;
//            predictedVision.SetPos(predictedVision.Pos() + cmd.vel.rotate(dir)*timeLength);
//            if ( RobotsCollisionDetector::Instance()->checkCollision(predictedVision, timeLength) )
//                collision_with_opp = true;
//        }

//        RobotCommandEffect last_cmd;
//        if ( _commandLogger.commandValid(cycle - NUM_MOV_LATENCY_FRAMES - 1) )
//            last_cmd = _commandLogger.getCommand(cycle - NUM_MOV_LATENCY_FRAMES - 1);
//        else {
//            last_cmd.rot_speed = 0.0;
//            last_cmd.vel = CVector(0, 0);
//        }

//        for(int i = cycle - NUM_MOV_LATENCY_FRAMES; i < cycle ; ++i ) {
//            if ( collision_with_opp )
//                break;

//            // 延迟帧数的整数部分: 对延迟的每一帧做线性预测
//            RobotCommandEffect cmd;
//            if(_commandLogger.commandValid(i))
//                cmd = _commandLogger.getCommand(i);
//            else {
//                //std::cout<<"3.prediction vel equal zero"<<endl;
//                cmd.rot_speed = 0;
//                cmd.vel = CVector(0, 0);
//            }
//            //const RobotCommandEffect&cmd = _commandLogger.getCommand(i);
//            if( ballClose) {
//                _collisionSimulator.simulate(predictedVision, 1.0 / PARAM::Vision::FRAME_RATE);
//            }
//            /*if(print)
//                std::cout << "cmd " << cmd.vel.rotate(_visionLogger.getVision(i).Dir()) << "\t" << predictedVision.Pos() << std::endl;*/
//            CVector ave_vel = cmd.vel.rotate(_visionLogger.getVision(i).Dir()) * beta + last_cmd.vel.rotate(_visionLogger.getVision(i - 1).Dir()) * (1 - beta);
//            predictedVision.SetPos(predictedVision.Pos() + ave_vel / PARAM::Vision::FRAME_RATE);

//            last_cmd = cmd;
//            if ( RobotsCollisionDetector::Instance()->checkCollision(predictedVision, 1.0 / PARAM::Vision::FRAME_RATE) )
//                collision_with_opp = true;
//        }

//        // 朝向预测
//        if (ROT_LATENCY_FRACTION > 0) {
//            // 延迟帧数的小数部分
//            RobotCommandEffect cmd;
//            if(_commandLogger.commandValid(cycle - 1 - NUM_ROT_LATENCY_FRAMES))
//                cmd = _commandLogger.getCommand(cycle - 1 - NUM_ROT_LATENCY_FRAMES) ;
//            else {
//                //std::cout<<"4.prediction vel equal zero"<<endl;
//                cmd.rot_speed = 0;
//                cmd.vel = CVector(0, 0);
//            }
//            predictedVision.SetDir(Utils::Normalize(predictedVision.Dir() + cmd.rot_speed  / PARAM::Vision::FRAME_RATE * ROT_LATENCY_FRACTION) );
//        }

//        if ( _commandLogger.commandValid(cycle - NUM_MOV_LATENCY_FRAMES - 1) )
//            last_cmd = _commandLogger.getCommand(cycle - NUM_MOV_LATENCY_FRAMES - 1);
//        else {
//            last_cmd.rot_speed = 0.0;
//            last_cmd.vel = CVector(0, 0);
//        }

//        for(int i = cycle - NUM_ROT_LATENCY_FRAMES; i < cycle ; ++i ) {
//            // 延迟帧数的整数部分
//            RobotCommandEffect cmd;
//            if(_commandLogger.commandValid(i))
//                cmd = _commandLogger.getCommand(i);
//            else {
//                //std::cout<<"5.prediction vel equal zero"<<endl;
//                cmd.rot_speed = 0;
//                cmd.vel = CVector(0, 0);
//            }
//            double ave_rot = last_cmd.rot_speed * (1 - beta) + cmd.rot_speed * beta;
//            predictedVision.SetDir(Utils::Normalize(predictedVision.Dir() + ave_rot  / PARAM::Vision::FRAME_RATE) );
//            last_cmd = cmd;
//        }

//        predictedVision.SetValid(true);
//    } else {
//        if( ++_robotLostTime > 150) { //>31
//            predictedVision.SetValid(false);
////            _robotFilter.reset();
////            _robotRotationFilter.reset();
//            // need  reset!!!!!!!!!!!!!!!!!!!!!!!!
//            // player.pos.valid = false;
//            // player.pos.setValid(false);
//            // player.pos.setValid(0);
//            // VehicleInfoT* changePlayer = (VehicleInfoT*)(&player);
//            // changePlayer->pos.setValid(false);
//        } else {
//            predictLost(cycle, ball);
//        }
//    }

//    predictedVision.SetType(player.type);
//}

//void CRobotPredictor::updateTheirVision(int cycle, const VehicleInfoT & player, const ObjectPoseT & ball, bool invert, int realNum) {
//    const double x = invert ? -player.pos.x : player.pos.x;
//    const double y = invert ? -player.pos.y : player.pos.y;
//    double dir = 0;
//    if(fabs(player.dir) > 6) {
//        dir = 0;
//    } else {
//        dir = invert ? Utils::Normalize(player.dir + PARAM::Math::PI) : player.dir;
//    }

//    const bool seen = player.pos.valid;
//    const CGeoPoint pos(x, y);

//    updateRawVision(cycle, x, y, dir, seen); // 处理原始视觉数据
//    _collisionSimulator.reset(ball.RawPos(), ball.Vel()); // 初始化球的信息
//    RobotVisionData& predictedVision = _visionLogger.getVision(cycle);	// 注意: 这里是引用,在predictedVision上的操作相当于在_visionLogger相应帧的图像数据上操作
//    predictedVision.cycle = cycle; // 初始化
//    predictedVision.realNum = realNum;


//    if( seen ) {
//        //predictedVision.cycle = cycle; // 初始化
//        predictedVision.SetRawPos(x, y);
//    } else { // 没看到车，猜测一个原始位置
//        for( int i = 1; i < 8; ++i ) {
//            if( _visionLogger.visionValid(cycle - i) ) {
//                for( int j = cycle - i; j < cycle; ++j ) {
//                    const RobotVisionData& previousVision = _visionLogger.getVision(j);
//                    RobotVisionData& nextVision = _visionLogger.getVision(j + 1);
//                    if( _visionLogger.getVision(j - PARAM::Latency::TOTAL_LATED_FRAME).Valid() ) {
//                        nextVision.SetRawPos(previousVision.RawPos() + _visionLogger.getVision(j - PARAM::Latency::TOTAL_LATED_FRAME).Vel() / PARAM::Vision::FRAME_RATE);
//                    } else {
//                        nextVision.SetRawPos(previousVision.RawPos());
//                    }
//                    nextVision.cycle = j + 1;
//                }
//                break;
//            }
//        }
//    }


//    updateRawVision(cycle, predictedVision.RawPos().x(), predictedVision.RawPos().y(), dir, seen); // 处理原始视觉数据，进行滤波
//    if( seen && checkValid(cycle, pos) ) {
//        _robotLostTime = 0;
//        // 用简单线性模型预测
//        // 先设置初始状态
//        const RobotRawVisionData& rawVision = _rawVisionLogger.getVision(cycle);	// 取出kalman滤波后得到的位置和速度信息
//        const bool ballClose = (pos - ball.RawPos()).mod2() < 2500; // 距离近的时候才考虑和球的碰撞
//        RobotCommandEffect cmd;
//        predictedVision.SetPos(CGeoPoint(rawVision.x, rawVision.y));
//        predictedVision.SetDir(rawVision.dir);

//        // 用图像当做当前的速度
//        predictedVision.SetVel(CVector(rawVision.xVel, rawVision.yVel));
//        predictedVision.SetRotVel(rawVision.rotVel);
//        //std::cout<<"opp speed: "<<predictedVision.Vel().mod()<<endl;
//        // 根据动作进行推算
//        bool collision_with_us = false;
//        if (MOV_LATENCY_FRACTION > 0) {
//            predictedVision.SetPos(predictedVision.Pos() + predictedVision.Vel() / PARAM::Vision::FRAME_RATE * MOV_LATENCY_FRACTION);
//            // 我们只在朝向有效时才对朝向进行(-PI,PI)的转换处理,否则返回无效朝向值(图像传过来是1000度,即弧度17).
//            // 这样上层可以利用无效值做是否选用对手朝向的选择 by WLJiang.
//            if(_isHasRotation)
//                predictedVision.SetDir(Utils::Normalize(predictedVision.Dir() + predictedVision.RotVel()  / PARAM::Vision::FRAME_RATE * MOV_LATENCY_FRACTION) );

//            if ( RobotsCollisionDetector::Instance()->checkCollision(predictedVision, MOV_LATENCY_FRACTION / PARAM::Vision::FRAME_RATE))
//                collision_with_us = true;
//        }
//        for(int i = cycle - NUM_MOV_LATENCY_FRAMES; i < cycle ; ++i ) {
//            if ( collision_with_us )
//                break;

//            //const RobotCommandEffect&cmd = _commandLogger.getCommand(i);
//            if( ballClose) {
//                _collisionSimulator.simulate(predictedVision, 1.0 / PARAM::Vision::FRAME_RATE);
//            }
//            predictedVision.SetPos(predictedVision.Pos() + predictedVision.Vel() / PARAM::Vision::FRAME_RATE);
//            // 我们只在朝向有效时才对朝向进行(-PI,PI)的转换处理,否则返回无效朝向值(图像传过来是1000度,即弧度17).
//            // 这样上层可以利用无效值做是否选用对手朝向的选择 by WLJiang.
//            if(_isHasRotation)
//                predictedVision.SetDir(Utils::Normalize(predictedVision.Dir() + predictedVision.RotVel() / PARAM::Vision::FRAME_RATE) );

//            if ( RobotsCollisionDetector::Instance()->checkCollision(predictedVision, MOV_LATENCY_FRACTION / PARAM::Vision::FRAME_RATE))
//                collision_with_us = true;
//        }
//        predictedVision.SetValid(true);

//    } else {
//        if( ++_robotLostTime > 31) { //MAX_ROBOT_LOST_TIME
//            predictedVision.SetValid(false);
//        } else {
//            predictLost(cycle, ball);
//        }
//    }

//}

//void CRobotPredictor::updateRawVision(int cycle, double x, double y, double dir, bool seen) {
//    if( seen ) {
//        double playerPos[2] = { x, y };
//        double playerPosVel[2][2];
//        double playerRotation[2] = { std::cos(dir), std::sin(dir)};
//        double playerRotVel[2][2];
//        double rotVel;
//        double filterDir;

//        _robotFilter.updatePosition(playerPos, playerPosVel);	// kalman滤波得到预测的位置,速度
//        const CVector PlayVel(playerPosVel[0][1], playerPosVel[1][1]); // 速度
//        if (PlayVel.mod() > MAX_SPEED) {
//            playerPosVel[0][1] = 0;
//            playerPosVel[1][1] = 0;
//            _robotFilter.reset();
//        }


//        if (_isHasRotation) {
//            //如果有朝向，则计算转速
//            _robotRotationFilter.updatePosition(playerRotation, playerRotVel);
//            filterDir = std::atan2(playerRotVel[1][0], playerRotVel[0][0]);
//            rotVel = playerRotVel[0][1] * std::cos(90 * PARAM::Math::PI / 180 + filterDir) + playerRotVel[1][1] * std::sin(90 * PARAM::Math::PI / 180 + filterDir);
//        } else {
//            //std::cout<<"opp filter vel: "<<PlayVel<<endl;
//            filterDir = dir;
//            rotVel = 0;
//        }
//        //// 计算旋转速度，用两个周期的身体朝向
//        _rawVisionLogger.setVision(cycle, x, y, dir, playerPosVel[0][1], playerPosVel[1][1], rotVel);	// 存入当前帧的位置速度信息
//        _filterData.setVision(cycle, x, y, filterDir, playerPosVel[0][1], playerPosVel[1][1], rotVel);
//        if (_isHasRotation) {
//            _predictError.update(cycle, _rawVisionLogger, _visionLogger);
//        }
//    } else {
//        _robotFilter.reset();
//        _robotRotationFilter.reset();
//    }
//}
bool CRobotPredictor::checkValid(int cycle, const CGeoPoint & pos) {
    // 通过和上一帧的位置来比较,来确定当前以为的小车位置pos是否有效
    const double PLAYER_OUT_BUFFER = PARAM::Rule::Version == 2003 ? -20 : -50;
    if( !Utils::IsInField(pos, PLAYER_OUT_BUFFER) ) {
        return false; // 队员在边界以外很多
    }
    if( _visionLogger.visionValid(cycle - 1) ) {
        const RobotVisionData& lastRobot = _visionLogger.getVision(cycle - 1);
        if( lastRobot.Valid() ) {
            const double MAX_PLAYER_MOVE_DIST_PER_CYCLE = 10;
            const double MAX_PLAYER_ROTATE_ANGLE_PER_CYCLE = PARAM::Math::PI / 2;
            const CVector playerMoved = pos - lastRobot.RawPos();
            if( playerMoved.mod() > MAX_PLAYER_MOVE_DIST_PER_CYCLE ) {
                return false; // 走得太多，不可能，一定是看错了
            }
        }
    }
    return true;
}
void CRobotPredictor::predictLost(int cycle, const ObjectPoseT & ball) {
    //cout<<cycle<<" ";
    // 图像当前帧丢车后的处理办法
    if( _visionLogger.visionValid(cycle - 1)) {
        //cout<<"in*****************************************************"<<endl;
        const RobotVisionData& lastCycle = _visionLogger.getVision(cycle - 1);
        RobotVisionData& thisCycle = _visionLogger.getVision(cycle);
        if( !lastCycle.Valid() ) {
            thisCycle.SetValid(false);
            return;
        }
        thisCycle.SetPos(lastCycle.Pos());
        //cout<<"x: "<<lastCycle.Pos().x()<<"     "<<"y: "<<lastCycle.Pos().y()<<endl;
        thisCycle.SetDir(lastCycle.Dir());
        //cout<<"dir: "<<lastCycle.Dir()<<endl;
        // 我方用命令代替速度
        if (_isHasRotation) {
            if( _commandLogger.commandValid(cycle - 1) ) {
                // 若上一帧发出过指令,用指令代替速度;
                const RobotCommandEffect& cmd = _commandLogger.getCommand(cycle - 1);
                CVector vel = CVector(cmd.vel.rotate(lastCycle.Dir()).x(), cmd.vel.rotate(lastCycle.Dir()).y());
                thisCycle.SetVel(vel);
                //cout<<"Vx: "<<cmd.vel.rotate(lastCycle.Dir()).x()<<"     "<<"Vy: "<<cmd.vel.rotate(lastCycle.Dir()).y()<<endl;
                thisCycle.SetRotVel(cmd.rot_speed);
                //cout<<"speedW: "<<cmd.rot_speed<<endl;
            } else {
                // 上一帧没有发出过指令,继续使用上一帧速度信息
                thisCycle.SetVel(lastCycle.Vel());
                thisCycle.SetRotVel(lastCycle.RotVel());
            }
        } else {
            // 对方用前一周期的速度代替当前速度(没有朝向信息)
            thisCycle.SetVel(lastCycle.Vel());
            thisCycle.SetRotVel(lastCycle.RotVel());
        }

        if( (lastCycle.RawPos() - ball.RawPos()).mod2() < 2500) {
            _collisionSimulator.reset(ball.Pos(), ball.Vel());
            _collisionSimulator.simulate(thisCycle, 1.0 / PARAM::Vision::FRAME_RATE);
        }
        thisCycle.SetPos(lastCycle.Pos() + thisCycle.Vel() / PARAM::Vision::FRAME_RATE);
        thisCycle.SetDir(Utils::Normalize(lastCycle.Dir() + thisCycle.RotVel() / PARAM::Vision::FRAME_RATE));
        thisCycle.SetValid(true);
    }
}
void CRobotPredictor::updateCommand(int cycle, const CPlayerCommand * cmd) {
    _commandLogger.setCommand(cycle, cmd->getAffectedVel(), cmd->getAffectedRotateSpeed());
}
