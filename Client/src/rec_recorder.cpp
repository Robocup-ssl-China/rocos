#include "rec_recorder.h"
#include "globaldata.h"
#include "globalsettings.h"
#include "field.h"
#include <QDateTime>
#include <QDataStream>
#include <QFile>
#include <QFileInfo>
#include "parammanager.h"
#include <cmath>
using namespace ZSS::Protocol;
namespace  {
auto GS = GlobalSettings::instance();
//RecMsg recMsg;
bool isRun = true;
QFile recordFile;
QIODevice* recIO;
QString filename;

//QTime timer;
}
RecRecorder::RecRecorder() {
}
void RecRecorder::init() {
    isRun = true;
//        qDebug() << "I AM RUNNING";
    QDateTime datetime;
//        qDebug() << datetime.currentDateTime().toString("yyyy-MM-dd-HH-mm-ss");
    filename = QString("LOG/Rec").append(datetime.currentDateTime().toString("yyyy-MM-dd-HH-mm-ss")).append(".log");
//    recordFile = new QFile(filename);
    recordFile.setFileName(filename);
    recordFile.open(QIODevice::WriteOnly | QIODevice::Append);
//    recordFile->open(QIODevice::WriteOnly | QIODevice::Append);
}

void RecRecorder::store() {
    if (isRun) {
        recordFile.open(QIODevice::WriteOnly | QIODevice::Append);
        recIO = &recordFile;
        //    Field::repaintLock();
        //    qDebug() << "I AM FUCKING RUNNING";
        //    recMsg = recMsgs.add_recmsgs();
        RecMessage recMsg;
        TeamRobotMsg* processMsg;
        TeamRobotMsg* maintain;
        Robot4Rec* robot4Rec;
        Ball4Rec* ball4Rec;
        Debug_Msgs* debugMsgs;
        //ctrlc
        GlobalData::instance()->ctrlCMutex.lock();
        recMsg.set_ctrlc(GlobalData::instance()->ctrlC);
        GlobalData::instance()->ctrlCMutex.unlock();
        //selectedArea
        recMsg.mutable_selectedarea()->set_maxx(GS->maximumX);
        recMsg.mutable_selectedarea()->set_maxy(GS->maximumY);
        recMsg.mutable_selectedarea()->set_minx(GS->minimumX);
        recMsg.mutable_selectedarea()->set_miny(GS->minimumY);
        //maintainVision
        const OriginMessage &robot_vision = GlobalData::instance()->processRobot[0];
        recMsg.mutable_maintainvision()->set_lasttouch(GlobalData::instance()->lastTouch % PARAM::ROBOTMAXID);
        recMsg.mutable_maintainvision()->set_lasttouchteam(GlobalData::instance()->lastTouch < PARAM::ROBOTMAXID ? PARAM::BLUE : PARAM::YELLOW);
        for(int color = PARAM::BLUE; color <= PARAM::YELLOW; color++) {
            processMsg = recMsg.mutable_maintainvision()->add_processmsg();
            processMsg->set_size(robot_vision.robotSize[color]);
            for(int j = 0; j < robot_vision.robotSize[color]; j++) {
                robot4Rec = processMsg->add_robot();
                robot4Rec->set_id(robot_vision.robot[color][j].id);
                robot4Rec->set_posx(robot_vision.robot[color][j].pos.x());
                robot4Rec->set_posy(robot_vision.robot[color][j].pos.y());
                robot4Rec->set_angle(robot_vision.robot[color][j].angle);
            }
        }
        auto& maintainMsg = GlobalData::instance()->maintain[0];
        for(int color = PARAM::BLUE; color <= PARAM::YELLOW; color++) {
            maintain = recMsg.mutable_maintainvision()->add_maintain();
            maintain->set_size(maintainMsg.robotSize[color]);
            for(int j = 0; j < maintainMsg.robotSize[color]; j++) {
                robot4Rec = maintain->add_robot();
                robot4Rec->set_posx(maintainMsg.robot[color][j].pos.x());
                robot4Rec->set_posy(maintainMsg.robot[color][j].pos.y());
                robot4Rec->set_angle(maintainMsg.robot[color][j].angle);
            }
        }
        recMsg.mutable_maintainvision()->mutable_balls()->set_size(maintainMsg.ballSize);
        for(int j = 0; j < maintainMsg.ballSize; j++) {
            ball4Rec = recMsg.mutable_maintainvision()->mutable_balls()->add_ball();
            ball4Rec->set_posx(maintainMsg.ball[j].pos.x());
            ball4Rec->set_posy(maintainMsg.ball[j].pos.y());
            ball4Rec->set_valid(maintainMsg.ball[j].valid);
        }
        //debugMsgs
        GlobalData::instance()->debugMutex.lock();
        for (int team = PARAM::BLUE; team <= PARAM::YELLOW; team++) {
            debugMsgs = recMsg.add_debugmsgs();
            if (team == PARAM::BLUE) {
                debugMsgs->ParseFromArray(GlobalData::instance()->debugBlueMessages.data(), GlobalData::instance()->debugBlueMessages.size());
            } else {
                debugMsgs->ParseFromArray(GlobalData::instance()->debugYellowMessages.data(), GlobalData::instance()->debugYellowMessages.size());
            }
        }

        GlobalData::instance()->debugMutex.unlock();
        //    Field::repaintUnlock();

        int size = recMsg.ByteSizeLong();
        QByteArray buffer(size, 0);
        recMsg.SerializeToArray(buffer.data(), buffer.size());
        QDataStream stream(recIO);
        stream << buffer;
        //    recordFile->flush();
        //    recordFile->close();
        recordFile.close();
        recMsg.Clear();
        if (recordFile.size() > (qint64)1024 * 1024 * 1024 * 5) {
            stop();
        }
    }
}

void RecRecorder::stop() {
    isRun = false;
    recIO = nullptr;
    QFileInfo afterFile(filename);
    if (afterFile.size() < 1) {
//        QFile emptyFile(filename);
        recordFile.remove();
    }
//    if (recordFile && recordFile->isOpen()) recordFile->close();
//    delete recordFile;
//    recordFile = nullptr;
}
