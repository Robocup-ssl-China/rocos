#include "visionmodule.h"
#include "globaldata.h"
#include "maintain.h"
#include "dealball.h"
#include "dealrobot.h"
#include "messages_robocup_ssl_wrapper.pb.h"
#include "field.h"
#include "parammanager.h"
#include "globalsettings.h"
#include "rec_recorder.h"
#include <QElapsedTimer>
#include "networkinterfaces.h"
#include <QtDebug>
#include <QTimer>
#include <thread>
#include "communicator.h"
#include "sim/sslworld.h"
#include "simmodule.h"
namespace {
auto zpm = ZSS::ZParamManager::instance();
auto vpm = ZSS::VParamManager::instance();
QTimer sim_timer;
}
/**
 * @brief VisionModule consturctor
 * @param parent
 */
VisionModule::VisionModule()
    : p_draw_signal("draw_signal")
    , p_sim_signal("sim_signal")
    , s_ssl_vision("ssl_vision", [this](const zos::Data& data){parse((void*) data.data(), data.size());})
    , _running(false){
    std::fill_n(GlobalData::instance()->cameraUpdate, PARAM::CAMERA, false);
    std::fill_n(GlobalData::instance()->cameraControl, PARAM::CAMERA, false);
    GlobalData::instance()->cameraControl[0] = true;

    SSLWorld::instance()->p_ssl_vision.link(&s_ssl_vision);
    p_sim_signal.link(&SSLWorld::instance()->s_sim_signal);
}
/**
 * @brief connect UDP for receive vision
 * @param real
 */
void VisionModule::udpSocketConnect(bool real) {
    if (real) {
        zpm->loadParam(vision_port, "AlertPorts/Vision4Real", 10005);
    }else{
        zpm->loadParam(vision_port, "AlertPorts/Vision4Sim", 10020);
    }
    if(real){
        qDebug() << "VisionPort : " << vision_port;
        udpReceiveSocket.bind(QHostAddress::AnyIPv4, vision_port, QUdpSocket::ShareAddress);
        udpReceiveSocket.joinMulticastGroup(QHostAddress(ZSS::SSL_ADDRESS),NetworkInterfaces::instance()->get("vision"));
        connect(&udpReceiveSocket, SIGNAL(readyRead()), this, SLOT(storeData()), Qt::DirectConnection);
    }
    else{
        int desired = 65;
        ZSS::SParamManager::instance()->loadParam(desired,"worldp_vars/DesiredFPS");
        if(desired > 500) desired = 500;
        connect(&sim_timer,SIGNAL(timeout()),this,SLOT(oneStepSimData()),Qt::DirectConnection);
        sim_timer.start(int(1000/desired));
        ZSS::SimModule::instance()->resetSimCommand();
    }
    ReceiveVisionMessage temp;
    for (int i = 0; i < PARAM::CAMERA; i++) {
        GlobalData::instance()->camera[i].push(temp);
    }
    _running = true;
}
/**
 * @brief disconnect UDP
 */
void VisionModule::udpSocketDisconnect() {
    // MARKTODO store camera edge
    // if (IF_EDGE_TEST) {
    //     for (int i = 0; i < PARAM::CAMERA; i++) {
    //         SingleCamera& currentCamera = GlobalData::instance()->cameraMatrix[i];
    //         vpm->changeParam("Camera" + QString::number(i) + "Leftmin", currentCamera.leftedge.min);
    //         vpm->changeParam("Camera" + QString::number(i) + "Leftmax", currentCamera.leftedge.max);
    //         vpm->changeParam("Camera" + QString::number(i) + "Rightmin", currentCamera.rightedge.min);
    //         vpm->changeParam("Camera" + QString::number(i) + "Rightmax", currentCamera.rightedge.max);
    //         vpm->changeParam("Camera" + QString::number(i) + "Upmin", currentCamera.upedge.min);
    //         vpm->changeParam("Camera" + QString::number(i) + "Upmax", currentCamera.upedge.max);
    //         vpm->changeParam("Camera" + QString::number(i) + "Downmin", currentCamera.downedge.min);
    //         vpm->changeParam("Camera" + QString::number(i) + "Downmax", currentCamera.downedge.max);
    //     }
    // }
    _running = false;
    disconnect(&udpReceiveSocket);
    udpReceiveSocket.abort();
    sim_timer.stop();
    disconnect(&sim_timer, SIGNAL(timeout()), 0, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    resetVision();
    ZSS::SimModule::instance()->resetSimCommand();
}
void VisionModule::resetVision() {
    qDebug() << "reset vision";
    std::scoped_lock<std::shared_mutex> lock(_visionmutex);
    _detectionFrame.Clear();
    QByteArray data;
    auto detectionBall = _detectionFrame.mutable_balls();
    detectionBall->set_valid(false);
    detectionBall->set_x(-32767);
    detectionBall->set_y(-32767);
    detectionBall->set_raw_x(-32767);
    detectionBall->set_raw_y(-32767);
    int size = _detectionFrame.ByteSizeLong();
    data.resize(size);
    _detectionFrame.SerializeToArray(data.data(),size);
    p_draw_signal.publish(data.data(),size);
}
/**
 * @brief VisionModule::storeData
 */
void VisionModule::storeData() {
    static QByteArray datagram;
    while (udpReceiveSocket.hasPendingDatagrams()) {
        datagram.resize(udpReceiveSocket.pendingDatagramSize());
        udpReceiveSocket.readDatagram(datagram.data(), datagram.size());
        parse((void*)datagram.data(), datagram.size());
    }
}
void VisionModule::oneStepSimData(){
    p_sim_signal.publish("sim_signal");
}

/**
 * @brief process data
 * @return
 */
bool VisionModule::dealWithData() {
    counter++;
    updateEdge();
    DealBall::instance()->run();
    DealRobot::instance()->run();
    Maintain::instance()->run();
    return true;
}
/**
 * @brief parse camera vision message
 * @param ptr
 * @param size
 */
void VisionModule::parse(void * ptr, int size) {
    static SSL_WrapperPacket packet;
    ReceiveVisionMessage message;
    packet.ParseFromArray(ptr, size);
    if (packet.has_detection()) {
        const SSL_DetectionFrame& detection = packet.detection();
        message.camID = detection.camera_id();
        if (message.camID >= PARAM::CAMERA || message.camID < 0) {
//            qDebug() << "get invalid camera id : " << message.camID;
            message.camID = 0;
        }
        int ballSize = detection.balls_size();
        int blueSize = detection.robots_blue_size();
        int yellowSize = detection.robots_yellow_size();
        for (int i = 0; i < ballSize; i++) {
            const SSL_DetectionBall& ball = detection.balls(i);
            if (GlobalSettings::instance()->inChosenArea((CGeoPoint(ball.x(), ball.y())))) {
                message.addBall((CGeoPoint(ball.x(), ball.y())));
            }
        }
        for (int i = 0; i < blueSize; i++) {
            const SSL_DetectionRobot& robot = detection.robots_blue(i);
            if (GlobalSettings::instance()->inChosenArea((CGeoPoint(robot.x(), robot.y())))
                    && robot.robot_id() < PARAM::ROBOTMAXID) {
                message.addRobot(PARAM::BLUE, robot.robot_id(), (CGeoPoint(robot.x(), robot.y())), (robot.orientation()));
            }
        }
        for (int i = 0; i < yellowSize; i++) {
            const SSL_DetectionRobot& robot = detection.robots_yellow(i);
            if (GlobalSettings::instance()->inChosenArea((CGeoPoint(robot.x(), robot.y())))
                    && robot.robot_id() < PARAM::ROBOTMAXID) {
                message.addRobot(PARAM::YELLOW, robot.robot_id(), (CGeoPoint(robot.x(), robot.y())), (robot.orientation()));
            }
        }
        GlobalData::instance()->camera[message.camID].push(message);
        GlobalData::instance()->cameraUpdate[message.camID] = true;
    }
    if (collectNewVision()) {
        checkCommand();
        dealWithData();
        udpSend();
        ZRecRecorder::instance()->store();
        std::fill_n(GlobalData::instance()->cameraUpdate, PARAM::CAMERA, false);
    }
}

void VisionModule::checkCommand() {
    for (int team = 0; team < PARAM::TEAMS; team++) {
        auto commands = ZCommunicator::instance()->getCommands(team);
        if (commands.valid) {
            GlobalData::instance()->robotCommand[team].push(commands);
            GlobalData::instance()->commandMissingFrame[team] = 0;
        } else {
            commands = RobotCommands();
            GlobalData::instance()->robotCommand[team].push(commands);
            GlobalData::instance()->commandMissingFrame[team] >= 20 ?
            GlobalData::instance()->commandMissingFrame[team] = 20 :
                    GlobalData::instance()->commandMissingFrame[team]++;
        }
    }
    ZCommunicator::instance()->clearCommands();
}

void  VisionModule::udpSend() {
    if (!_running) return;
    //udp start
    std::scoped_lock<std::shared_mutex> lock(_visionmutex);
    _detectionFrame.Clear();
    auto detectionBall = _detectionFrame.mutable_balls();
    ReceiveVisionMessage result = GlobalData::instance()->maintain[0];
    if (result.ballSize > 0) {
        detectionBall->set_x(result.ball[0].pos.x());
        if (result.ball[0].pos.y() == 0) detectionBall->set_y(float(0.1));
        else detectionBall->set_y(result.ball[0].pos.y());//to fix a role match bug 2018.6.15
        CVector TransferVel(result.ball[0].velocity.x(), result.ball[0].velocity.y());
        detectionBall->set_vel_x(TransferVel.x());
        detectionBall->set_vel_y(TransferVel.y());
        detectionBall->set_valid(DealBall::instance()->getValid());
        detectionBall->set_last_touch(GlobalData::instance()->lastTouch);
        detectionBall->set_ball_state(result.ball[0].ball_state_machine.ballState);
        detectionBall->set_raw_x(GlobalData::instance()->processBall[0].ball[0].pos.x());
        detectionBall->set_raw_y(GlobalData::instance()->processBall[0].ball[0].pos.y());
        detectionBall->set_chip_predict_x(GlobalData::instance()->maintain[0].ball[0].predict_pos.x());
        detectionBall->set_chip_predict_y(GlobalData::instance()->maintain[0].ball[0].predict_pos.y());
    } else {
        detectionBall->set_valid(false);
        detectionBall->set_x(-32767);
        detectionBall->set_y(-32767);
    }
    for (int team = 0; team < PARAM::TEAMS; team++) {
        for (int i = 0; i < result.robotSize[team]; i++) {
            Vision_DetectionRobot* robot;
            if (team == 0 )  robot = _detectionFrame.add_robots_blue();
            else robot = _detectionFrame.add_robots_yellow();
            robot->set_x(result.robot[team][i].pos.x());
            robot->set_y(result.robot[team][i].pos.y());
            robot->set_orientation(result.robot[team][i].angle);
            robot->set_robot_id(result.robot[team][i].id);
            CVector TransferVel(result.robot[team][i].velocity.x(),
                                result.robot[team][i].velocity.y());
            robot->set_vel_x(TransferVel.x());
            robot->set_vel_y(TransferVel.y());
            robot->set_rotate_vel(result.robot[team][i].rotateVel);
            robot->set_accelerate_x(result.robot[team][i].accelerate.x());
            robot->set_accelerate_y(result.robot[team][i].accelerate.y());
            robot->set_raw_x(GlobalData::instance()->processRobot[0].robot[team][i].pos.x());
            robot->set_raw_y(GlobalData::instance()->processRobot[0].robot[team][i].pos.y());
            robot->set_raw_orientation(GlobalData::instance()->processRobot[0].robot[team][i].angle);
            robot->set_valid(true);
            robot->set_raw_vel_x(result.robot[team][i].raw_vel.x());
            robot->set_raw_vel_y(result.robot[team][i].raw_vel.y());
            robot->set_raw_rotate_vel(result.robot[team][i].rawRotateVel);
        }
    }
    int size = _detectionFrame.ByteSizeLong();
    QByteArray buffer(size, 0);
    _detectionFrame.SerializeToArray(buffer.data(), buffer.size());
    p_draw_signal.publish(buffer.data(),size);
    GlobalData::instance()->ctrlCMutex.lock();
    bool sw = GlobalData::instance()->ctrlC;
    GlobalData::instance()->ctrlCMutex.unlock();
    if(!sw) { //需要发两个端口以供对打
        udpSender.sendData(buffer.data(), buffer.size(), ZSS::Athena::VISION_SEND[0]);
        udpSender.sendData(buffer.data(), buffer.size(), ZSS::Athena::VISION_SEND[1]);
        udpSender.sendData(buffer.data(), buffer.size(), ZSS::Athena::SEND_TO_PYTHON);
    }
}

/**
 * @brief judge if all needed camera vision is collected
 * @return
 */
bool VisionModule::collectNewVision() {
    for (int i = 0; i < PARAM::CAMERA; i++) {
        if (GlobalData::instance()->cameraControl[i] && !GlobalData::instance()->cameraUpdate[i])
            return false;
    }
    return true;
}
/**
 * @brief filed edgeTest
 */
void VisionModule::updateEdge() {
    for (int camID = 0; camID < PARAM::CAMERA; camID++) {
        if (GlobalData::instance()->cameraUpdate[camID]) {
            for (int i = 0; i < GlobalData::instance()->camera[camID][0].ballSize; i++) {
                GlobalData::instance()->cameraAnchor[camID].update(GlobalData::instance()->camera[camID][0].ball[i].pos, 1);
            }
            for (int color = 0; color < PARAM::TEAMS; color++){
                for (int i = 0; i < GlobalData::instance()->camera[camID][0].robotSize[color]; i++) {
                    GlobalData::instance()->cameraAnchor[camID].update(GlobalData::instance()->camera[camID][0].robot[color][i].pos, 0);
                }
            }
        }
    }
}
/**
 * @brief getFPS for front
 * @return
 */
quint16 VisionModule::getFPS() {
    static QElapsedTimer timer;
    static bool ifStart = false;
    static quint64 lastCount;
    static quint16 result;
    if (!ifStart) {
        ifStart = true;
        timer.start();
        lastCount = counter;
        return 0;
    }
    result = (counter - lastCount) * 1000.0 / timer.restart();
    lastCount = counter;
    return result;
}

VisionModule::CheckRobotResult VisionModule::checkRobot(double x,double y){
    std::shared_lock lock(_visionmutex);
    int blue_size = _detectionFrame.robots_blue_size();
    int yellow_size = _detectionFrame.robots_yellow_size();
    for(int i=0;i<blue_size;i++){
        auto&& robot = _detectionFrame.robots_blue(i);
        if(auto dist = std::hypot(robot.x()-x,robot.y()-y);dist < PARAM::Vehicle::V2::PLAYER_SIZE){
            return {true,PARAM::BLUE,robot.robot_id(),robot.x(),robot.y(),robot.orientation()};
        }
    }
    for(int i=0;i<yellow_size;i++){
        auto&& robot = _detectionFrame.robots_yellow(i);
        if(auto dist = std::hypot(robot.x()-x,robot.y()-y);dist < PARAM::Vehicle::V2::PLAYER_SIZE){
            return {true,PARAM::YELLOW,robot.robot_id(),robot.x(),robot.y(),robot.orientation()};
        }
    }
    return {false,0,0,0,0,0};
}
