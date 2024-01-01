#include "rec_player.h"
#include "field.h"
#include "zss_rec_old.pb.h"
#include "globaldata.h"
#include "globalsettings.h"
#include <QIODevice>
#include <QDataStream>
#include <QTime>
using namespace ZSS::Protocol;
namespace  {
auto GS = GlobalSettings::instance();
}
RecPlayer::RecPlayer() {

}

bool RecPlayer::loadRec(QString &filename, int &maxframe) {
    replayFile = new QFile(filename);
//    qDeleteAll(packets);
    packets.clear();
    if (replayFile->open(QIODevice::ReadOnly)) {
        QIODevice* recIO;
        recIO = replayFile;
        QDataStream stream(recIO);
        while (!stream.atEnd()) {
            QByteArray packet;
            stream >> packet;
            packets.append(packet);
        }
        maxframe = packets.size() - 1;
        delete replayFile;
        replayFile = nullptr;
        return true;
    }
    qDebug() << "open filed";
    return false;
}

bool RecPlayer::start(int position) {
    if (position > packets.size() - 1) {
        return false;
    }

    _currentFrame = position;

    _run = true;
    QThread::start();

    return true;
}

void RecPlayer::slowly(int position) {
    if (position > packets.size() - 1) {
        return;
    }

    _currentFrame = position;

    QThread::start();
}

void RecPlayer::stop() {
    _run = false;
    QThread::wait();
}

bool RecPlayer::good() {
    return packets.size() > 0;
}

void RecPlayer::sendMessage(const QByteArray& packet) {
//    qDebug() << "sendmessage is called" ;
    ZSS::Protocol::RecMessage recMsg;
    recMsg.ParseFromArray(packet.data(), packet.size());
    //ctrlc
    GlobalData::instance()->ctrlCMutex.lock();
    GlobalData::instance()->ctrlC = recMsg.ctrlc();
    GlobalData::instance()->ctrlCMutex.unlock();
    //selectedArea
    GS->maximumX = recMsg.selectedarea().maxx();
    GS->minimumX = recMsg.selectedarea().minx();
    GS->maximumY = recMsg.selectedarea().maxy();
    GS->minimumY = recMsg.selectedarea().miny();
    //maintainVision
    GlobalData::instance()->lastTouch = recMsg.maintainvision().lasttouchteam() == PARAM::BLUE ? recMsg.maintainvision().lasttouch() : recMsg.maintainvision().lasttouch() + PARAM::ROBOTMAXID;
    ReceiveVisionMessage result;
    for(int color = PARAM::BLUE; color <= PARAM::YELLOW; color++) {
//        processMsg = recMsg->mutable_maintainvision()->add_processmsg();
//        processMsg->set_size(robot_vision.robotSize[color]);
        for(int j = 0; j < recMsg.maintainvision().processmsg(color).size(); j++) {
            result.addRobot(color,
                            recMsg.maintainvision().processmsg(color).robot(j).id(),
                            recMsg.maintainvision().processmsg(color).robot(j).posx(),
                            recMsg.maintainvision().processmsg(color).robot(j).posy(),
                            recMsg.maintainvision().processmsg(color).robot(j).angle());
        }
    }
    GlobalData::instance()->processRobot.push(result);
//    qDebug() << "FUCKING MSG" << GlobalData::instance()->processRobot[0].robotSize[0];
//    auto& maintainMsg = GlobalData::instance()->maintain[0];
    ReceiveVisionMessage maintainResult;
    for(int color = PARAM::BLUE; color <= PARAM::YELLOW; color++) {
//        maintain = recMsg->mutable_maintainvision()->add_maintain();
//        maintain->set_size(maintainMsg.robotSize[color]);
        for(int j = 0; j < recMsg.maintainvision().maintain(color).size(); j++) {
            maintainResult.addRobot(color,
                                    recMsg.maintainvision().maintain(color).robot(j).id(),
                                    recMsg.maintainvision().maintain(color).robot(j).posx(),
                                    recMsg.maintainvision().maintain(color).robot(j).posy(),
                                    recMsg.maintainvision().maintain(color).robot(j).angle());
        }
    }
//    recMsg->mutable_maintainvision()->mutable_balls()->set_size(maintainMsg.ballSize);
    for(int j = 0; j < recMsg.maintainvision().balls().size(); j++) {
        maintainResult.addBall(recMsg.maintainvision().balls().ball(j).posx(),
                               recMsg.maintainvision().balls().ball(j).posy());
        maintainResult.ball[j].valid = recMsg.maintainvision().balls().ball(j).valid();
    }
    GlobalData::instance()->maintain.push(maintainResult);

    //debugMsgs
    GlobalData::instance()->debugMutex.lock();
    Debug_Msgs debugMsgs;
    for (int team = PARAM::BLUE; team <= PARAM::YELLOW; team++) {
        debugMsgs = recMsg.debugmsgs(team);
        int size = debugMsgs.ByteSizeLong();
        if (team == 0) {
            GlobalData::instance()->debugBlueMessages.resize(size);
            debugMsgs.SerializeToArray(GlobalData::instance()->debugBlueMessages.data(), size);
        } else {
            GlobalData::instance()->debugYellowMessages.resize(size);
            debugMsgs.SerializeToArray(GlobalData::instance()->debugYellowMessages.data(), size);
        }
//        qDebug() << "FUCK DEBUG MESSAGE SIZE" <<  debugMsgs.ByteSizeLong() << GlobalData::instance()->debugBlueMessages.size();
    }
    GlobalData::instance()->debugMutex.unlock();
}

void RecPlayer::run() {
    QTime timer;
    timer.start();
    sendMessage(packets.at(_currentFrame));
    emit positionChanged(_currentFrame);
    GlobalSettings::instance()->needRepaint();
    while (_run && ++_currentFrame < packets.size() - 1 && this->isRunning()) {
        sendMessage(packets.at(_currentFrame));
        emit positionChanged(_currentFrame);
        GlobalSettings::instance()->needRepaint();
        QThread::currentThread()->msleep(12);
    }

}

//void RecPlayer::terminate() {

//    delete replayFile;
//    replayFile = nullptr;
////    qDeleteAll(packets);
//    packets.clear();
//    QThread::quit();
//    QThread::wait();
//}

