#include "logeventlabel.h"
#include "messages_robocup_ssl_detection.pb.h"
#include "globaldata.h"
#include <QtEndian>
#include <QDebug>

LogEventLabel::LogEventLabel(QObject *parent) : QObject(parent) {

}
LogEventLabel::~LogEventLabel() {
//    qDebug() << in.is_open();
    in.close();
    if (in.is_open()) {
        qDebug() << "WARNING LABEL FILE IS OPEN" << in.is_open();
    }

}


void LogEventLabel::parseDetection(const SSL_DetectionFrame &detection, frame &_frame) {
    for (int i = 0 ; i < detection.balls_size(); i++) {
        ::ball _ball;
        _ball.camera_id = detection.camera_id();
        _ball.pos.setX(detection.balls(i).x());
        _ball.pos.setY(detection.balls(i).y());
        _frame.ballList.append(_ball);
    }
    for (int i = 0; i < detection.robots_blue_size(); i++) {
        ::robot _robot;
        _robot.camera_id = detection.camera_id();
        _robot.pos.setX(detection.robots_blue(i).x());
        _robot.pos.setY(detection.robots_blue(i).y());
        _robot.dir = CVector(std::cos(detection.robots_blue(i).orientation()),
                             std::sin(detection.robots_blue(i).orientation()));
        _frame.botList[PARAM::BLUE][detection.robots_blue(i).robot_id()].append(_robot);
//        qDebug() << detection.robots_blue(i).robot_id() << _robot.dir.x() << _robot.dir.y() << _robot.dir.mod();
    }
    for (int i = 0; i < detection.robots_yellow_size(); i++) {
        ::robot _robot;
        _robot.camera_id = detection.camera_id();
        _robot.pos.setX(detection.robots_yellow(i).x());
        _robot.pos.setY(detection.robots_yellow(i).y());
        _robot.dir = CVector(std::cos(detection.robots_yellow(i).orientation()),
                             std::sin(detection.robots_yellow(i).orientation()));
        _frame.botList[PARAM::YELLOW][detection.robots_yellow(i).robot_id()].append(_robot);
    }
    return;
}
void  LogEventLabel::processSingelReferee(int startFrame, int endFrame) {
    if (startFrame == -1 || endFrame == -1) return;

    int _frameDuration = endFrame - startFrame;
    qDebug() << "current Group from" << startFrame << "to" << endFrame << "with" << _frameDuration;
    frames.clear();
    LabelerFrameGroup _labelerFrameGroup;
    LabelerFrame _labelerFrame;

    for (int frameGroupIndex = startFrame; frameGroupIndex < endFrame ; frameGroupIndex++) {
        //每帧读入
        ::frame _frame;
        _labelerFrameGroup = readFrame(frameGroupIndex);
        for(int frameIndex = 0; frameIndex < _labelerFrameGroup.frames_size(); frameIndex++) {
            _labelerFrame = _labelerFrameGroup.frames(frameIndex);
            if (_labelerFrame.has_vision_frame() && _labelerFrame.vision_frame().has_detection()) {
                parseDetection( _labelerFrame.vision_frame().detection(), _frame);
                _frame.timestamp = _labelerFrame.timestamp();
            }
        }
        //处理real
        double tempX, tempY;
        if(_frame.ballList.size() > 0) {
            tempX = tempY = 0;
            for (int i = 0; i < _frame.ballList.size(); i++) {
                tempX += _frame.ballList.at(i).pos.x();
                tempY += _frame.ballList.at(i).pos.y();
            }
            _frame.realBall.pos.setX((double)tempX / _frame.ballList.size());
            _frame.realBall.pos.setY((double)tempY / _frame.ballList.size());
            _frame.realBall.valid = true;
        }
        for (int id = 0; id < PARAM::ROBOTMAXID; id++) {
            if (_frame.botList[PARAM::BLUE][id].size() > 0) {
                tempX = tempY = 0;
                for (int i = 0; i < _frame.botList[PARAM::BLUE][id].size(); i++) {
                    tempX += _frame.botList[PARAM::BLUE][id].at(i).pos.x();
                    tempY += _frame.botList[PARAM::BLUE][id].at(i).pos.y();
                }
                _frame.bot[PARAM::BLUE][id].pos.setX((double)tempX / _frame.botList[PARAM::BLUE][id].size());
                _frame.bot[PARAM::BLUE][id].pos.setY((double)tempY / _frame.botList[PARAM::BLUE][id].size());
                _frame.bot[PARAM::BLUE][id].dir = _frame.botList[PARAM::BLUE][id].at(0).dir;
                _frame.bot[PARAM::BLUE][id].valid = true;
            }
            if (_frame.botList[PARAM::YELLOW][id].size() > 0) {
                tempX = tempY = 0;
                for (int i = 0; i < _frame.botList[PARAM::YELLOW][id].size(); i++) {
                    tempX += _frame.botList[PARAM::YELLOW][id].at(i).pos.x();
                    tempY += _frame.botList[PARAM::YELLOW][id].at(i).pos.y();
                }
                _frame.bot[PARAM::YELLOW][id].pos.setX((double)tempX / _frame.botList[PARAM::YELLOW][id].size());
                _frame.bot[PARAM::YELLOW][id].pos.setY((double)tempY / _frame.botList[PARAM::YELLOW][id].size());
                _frame.bot[PARAM::YELLOW][id].dir = _frame.botList[PARAM::YELLOW][id].at(0).dir;
                _frame.bot[PARAM::YELLOW][id].valid = true;
            }
        }
        frames.push_back(_frame);
    }
    //compute Vel
    int deltaD, deltaI;
    for (int frameIndex = 0; frameIndex < _frameDuration ; frameIndex++)
        if (frames.at(frameIndex).realBall.valid) {
            for (deltaD = 0; deltaD >= -3; deltaD--)
                if (frameIndex + deltaD < 0 || !frames.at(frameIndex + deltaD).realBall.valid)
                    break;
            deltaD++;
            for (deltaI = 0; deltaI <= 3; deltaI++)
                if (frameIndex + deltaI >= _frameDuration || !frames.at(frameIndex + deltaI).realBall.valid)
                    break;
            deltaI--;
            CVector _vel = (frames.at(frameIndex + deltaD).realBall.pos - frames.at(frameIndex + deltaI).realBall.pos)
                           / (deltaI - deltaD);
            frames[frameIndex].realBall.vel = _vel;
//            qDebug() << "Current frame" << frameIndex + startFrame << _vel.x() << _vel.y() << "within" << deltaI - deltaD << "frames";
        } else {
//            qDebug() << "Current frame" << frameIndex + startFrame << "has no BAll";
        }
    //BallPossession
    int ballinBlue = -1, ballinYellow = -1, inPossession = -1;
    for (int frameIndex = 0; frameIndex < _frameDuration ; frameIndex++) {
        BallPossessionLabel* _ballPossesionLabel = resultLabels.add_ball_possession_labels();
        _ballPossesionLabel->set_state(BallPossessionLabel_State_NONE);
        ballinBlue = ballInMouth(PARAM::BLUE, frameIndex);
        ballinYellow =  ballInMouth(PARAM::YELLOW, frameIndex);
        if (ballinBlue >= 0 && ballinYellow >= 0) {
            //争球，球被看到：极少情况/几乎没有
            _ballPossesionLabel->set_state(BallPossessionLabel_State_NONE);
//            qDebug() << "frame" << frameIndex + startFrame << "has conflict";
            inPossession = -1;
            frames[frameIndex].state |= BALL_POSSESSION_UNKNOWN;
        } else if (frames[frameIndex].realBall.valid) {
            //看得到球
            if(ballinBlue >= 0) {
//                qDebug() << "BALL POSSESSION" << frameIndex + startFrame << "BLUE IN " << ballinBlue;
                _ballPossesionLabel->set_state(BallPossessionLabel_State_BLUE_POSSES);
                _ballPossesionLabel->set_robot_id(ballinBlue);
                inPossession = ballinBlue;
                frames[frameIndex].state |= BALL_POSSESSION_BLUE;
            } else if (ballinYellow >= 0) {
//                qDebug() << "BALL POSSESSION" << frameIndex + startFrame << "YELLOW IN " << ballinYellow;
                _ballPossesionLabel->set_state(BallPossessionLabel_State_YELLOW_POSSES);
                _ballPossesionLabel->set_robot_id(ballinYellow);
                inPossession = PARAM::ROBOTMAXID + ballinYellow;
                frames[frameIndex].state |= BALL_POSSESSION_YELLOW;
            } else {
                _ballPossesionLabel->set_state(BallPossessionLabel_State_NONE);
                inPossession = -1;
                frames[frameIndex].state |= BALL_POSSESSION_UNKNOWN;
            }
        } else if (!frames[frameIndex].realBall.valid) {
            //看不见球
            if (inPossession >= 0) {
                if (inPossession < PARAM::ROBOTMAXID) {
//                    qDebug() << "BALL POSSESSION" << frameIndex + startFrame << "BLUE IN " << inPossession;
                    _ballPossesionLabel->set_state(BallPossessionLabel_State_BLUE_POSSES);
                    _ballPossesionLabel->set_robot_id(inPossession);
                    frames[frameIndex].state |= BALL_POSSESSION_BLUE;
                } else {
//                    qDebug() << "BALL POSSESSION" << frameIndex + startFrame << "YELLOW IN " << inPossession - PARAM::ROBOTMAXID;
                    _ballPossesionLabel->set_state(BallPossessionLabel_State_YELLOW_POSSES);
                    _ballPossesionLabel->set_robot_id(inPossession - PARAM::ROBOTMAXID);
                    frames[frameIndex].state |= BALL_POSSESSION_YELLOW;
                }
            } else {
                _ballPossesionLabel->set_state(BallPossessionLabel_State_NONE);
                inPossession = -1;
                frames[frameIndex].state |= BALL_POSSESSION_UNKNOWN;
            }
        }
    }

    //Dribbling
    CGeoPoint startDribblingPoint;
    bool isDribbling = false;
    for (int frameIndex = 0; frameIndex < _frameDuration ; frameIndex++) {
        DribblingLabel* _dribblelLabel = resultLabels.add_dribbling_labels();
        _dribblelLabel->set_is_dribbling(false);
        if (!isDribbling) {
            //还没吸球
            if (frames[frameIndex].realBall.valid && (frames[frameIndex].state & BALL_POSSESSION_BLUE)) {
                isDribbling = true;
                startDribblingPoint = frames[frameIndex].realBall.pos;
            } else if(frames[frameIndex].realBall.valid && (frames[frameIndex].state & BALL_POSSESSION_YELLOW)) {
                isDribbling = true;
                startDribblingPoint = frames[frameIndex].realBall.pos;
            }
        } else {
            //吸球状态
            if (frames[frameIndex].state & BALL_POSSESSION_UNKNOWN) {
                isDribbling = false;
            } else if(frames[frameIndex].realBall.valid &&
                      startDribblingPoint.dist(frames[frameIndex].realBall.pos) >= 1000.0) {
                _dribblelLabel->set_is_dribbling(true);
                _dribblelLabel->set_robot_id(resultLabels.ball_possession_labels(frameIndex + startFrame).robot_id());
                if (frames[frameIndex].state & BALL_POSSESSION_BLUE)
                    _dribblelLabel->set_team(TeamBLUE);
                else
                    _dribblelLabel->set_team(TeamYELLOW);
//                qDebug() << "found Dribbling at Frame" << frameIndex + startFrame << "with TEAM" << _dribblelLabel->team()
//                         << "at" << frames[frameIndex].realBall.pos.x() << frames[frameIndex].realBall.pos.y()
//                         << "with dist of" << startDribblingPoint.dist(frames[frameIndex].realBall.pos);
            }
        }
    }
    //Passing
    bool isPassing = false;
    int passingStartFrame, passingTeam = -1, passingRobotID = -1;
    for (int frameIndex = 1; frameIndex < _frameDuration; frameIndex++) {
        if (!isPassing  && (frames[frameIndex].state & BALL_POSSESSION_UNKNOWN)) {
            //不在传球且球自由状态
            if ((frames[frameIndex - 1].state & BALL_POSSESSION_BLUE)
                    && frames[frameIndex].realBall.valid && frames[frameIndex].realBall.vel.mod() > 100) {
                isPassing = true;
                passingTeam = PARAM::BLUE;
                passingStartFrame = frameIndex;
                passingRobotID = resultLabels.ball_possession_labels(frameIndex + startFrame).robot_id();
                qDebug() << "MAYBE blue from" << passingRobotID;
            } else if ((frames[frameIndex - 1].state & BALL_POSSESSION_YELLOW)
                       && frames[frameIndex].realBall.valid && frames[frameIndex].realBall.vel.mod() > 100) {
                isPassing = true;
                passingTeam = PARAM::YELLOW;
                passingStartFrame = frameIndex;
                passingRobotID = resultLabels.ball_possession_labels(frameIndex + startFrame).robot_id();
                qDebug() << "MAYBE YELLOW from" << passingRobotID;
            }
        } else if (isPassing) {
            if (frames[frameIndex].state & BALL_POSSESSION_BLUE) {
                PassingLabel* _passingLabel =  resultLabels.add_passing_labels();
                _passingLabel->set_start_frame(passingStartFrame + startFrame);
                _passingLabel->set_end_frame(frameIndex + startFrame);
                _passingLabel->set_passer_id(passingRobotID);
                if (passingTeam == PARAM::BLUE) {
                    _passingLabel->set_successful(true);
                    _passingLabel->set_passer_team(TeamBLUE);
                    _passingLabel->set_receiver_id(resultLabels.ball_possession_labels(frameIndex + startFrame).robot_id());
                    qDebug() << "WTF" << resultLabels.ball_possession_labels(frameIndex + startFrame).robot_id() << passingRobotID << _passingLabel->passer_id()
                             << QString::fromStdString(_passingLabel->ShortDebugString());
                } else {
                    _passingLabel->set_successful(false);
                    _passingLabel->set_passer_team(TeamYELLOW);
                    qDebug() << QString::fromStdString(_passingLabel->ShortDebugString());
                }
                isPassing = false;
                passingStartFrame = -1;
                passingTeam = -1;
                passingRobotID = -1;
            } else if (frames[frameIndex].state & BALL_POSSESSION_YELLOW) {
                PassingLabel* _passingLabel =  resultLabels.add_passing_labels();
                _passingLabel->set_start_frame(passingStartFrame + startFrame);
                _passingLabel->set_end_frame(frameIndex + startFrame);
                _passingLabel->set_passer_id(passingRobotID);
                if (passingTeam == PARAM::YELLOW) {
                    _passingLabel->set_successful(true);
                    _passingLabel->set_passer_team(TeamYELLOW);
                    _passingLabel->set_receiver_id(resultLabels.ball_possession_labels(frameIndex + startFrame).robot_id());
                    qDebug() << "WTF" << resultLabels.ball_possession_labels(frameIndex + startFrame).robot_id() << passingRobotID << _passingLabel->passer_id()
                             << QString::fromStdString(_passingLabel->ShortDebugString());
                } else {
                    _passingLabel->set_successful(false);
                    _passingLabel->set_passer_team(TeamBLUE);
                    qDebug() << QString::fromStdString(_passingLabel->ShortDebugString());
                }
                isPassing = false;
                passingStartFrame = -1;
                passingTeam = -1;
                passingRobotID = -1;
            } else if (frameIndex - passingStartFrame >= 30 && frames[frameIndex].realBall.valid
                       && std::abs(frames[frameIndex].realBall.vel.theta(frames[passingStartFrame].realBall.vel)) > 3.14159 / 9) {
                //球速线变化20度
                PassingLabel* _passingLabel =  resultLabels.add_passing_labels();
                _passingLabel->set_start_frame(passingStartFrame + startFrame);
                _passingLabel->set_end_frame(frameIndex + startFrame);
                _passingLabel->set_passer_id(passingRobotID);
                _passingLabel->set_successful(false);
                if (passingTeam == PARAM::BLUE)
                    _passingLabel->set_passer_team(TeamBLUE);
                else
                    _passingLabel->set_passer_team(TeamYELLOW);
                qDebug() << "FUCK!!!" << QString::fromStdString(_passingLabel->ShortDebugString());
                isPassing = false;
                passingStartFrame = -1;
                passingTeam = -1;
                passingRobotID = -1;
            }
        }
    }
    if (isPassing) {
        //最后还是在PASSING 状态
        PassingLabel* _passingLabel =  resultLabels.add_passing_labels();
        _passingLabel->set_start_frame(passingStartFrame + startFrame);
        _passingLabel->set_end_frame(endFrame);
        _passingLabel->set_passer_id(passingRobotID);
        _passingLabel->set_successful(false);
        if (passingTeam == PARAM::BLUE)
            _passingLabel->set_passer_team(TeamBLUE);
        else
            _passingLabel->set_passer_team(TeamYELLOW);
        qDebug() << "FUCK!!!" << QString::fromStdString(_passingLabel->ShortDebugString());
    }

    return;
}

void LogEventLabel::processLabel() {
    QString logOutput = "Start Process " + fileName;
    setlogName(logOutput);
    LabelerFrameGroup _labelerFrameGroup;
    LabelerFrame _labelerFrame;
    int currentRefereeCommandCount = -1, startFrame = -1;
    for (int frameGroupIndex = 0; frameGroupIndex < metaData.message_offsets_size() ; frameGroupIndex++) {
//    for (int frameGroupIndex = 2000; frameGroupIndex < 4000 ; frameGroupIndex++) {
        _labelerFrameGroup = readFrame(frameGroupIndex);
        for(int frameIndex = 0; frameIndex < _labelerFrameGroup.frames_size(); frameIndex++) {
            _labelerFrame = _labelerFrameGroup.frames(frameIndex);
            if (_labelerFrame.has_referee_frame() && _labelerFrame.referee_frame().command_counter() != currentRefereeCommandCount) {
                processSingelReferee(startFrame, frameGroupIndex);
                startFrame = frameGroupIndex;
                currentRefereeCommandCount = _labelerFrame.referee_frame().command_counter();
            }
        }
    }
    processSingelReferee(startFrame, metaData.message_offsets_size());//处理最后一个裁判盒阶段
    qDebug() << "Finally process " << resultLabels.ball_possession_labels_size() << "ball Possesion\t"
             << resultLabels.dribbling_labels_size() << "dribbling Events"
             << resultLabels.passing_labels_size() << "passing Events";
    logOutput = "Process Done " + fileName;
    setlogName(logOutput);
    saveLabel("result_ZJUNlict");
    return;
}

void LogEventLabel::saveLabel(std::string outFileName){
    outFileName.append(".label");
    out.open(outFileName,std::ios::out | std::ios::trunc | std::ios::binary);
    if (!resultLabels.SerializePartialToOstream(&out)){
        qDebug()<<"Failed to write result to file";
        return ;
    }
    std::cout<<"Successed  write result to file "<<outFileName;
    out.close();
    return;
}

LabelerFrameGroup LogEventLabel::readFrame(int index) {
    LabelerFrameGroup frameGroup;
    in.seekg(metaData.message_offsets(index), in.beg);
    quint32_be* messageLength;
    messageLength = (quint32_be* ) malloc (sizeof(quint32_be));
    in.read((char*)messageLength, 4);
    buf = (char*) malloc (sizeof(char) * *messageLength);
    in.read(buf, *messageLength);
    std::string s (buf, buf + *messageLength);
    if (! frameGroup.ParseFromString(s)) {
        qDebug() << "END of frame message Failed to parse framegroup";
    }
    return frameGroup;
}

bool LogEventLabel::loadFile(QString _fileName) {
#ifdef WIN32
    _fileName.remove(QString("file:///"));
#else
    _fileName.remove(QString("file://"));
#endif
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    if (in.is_open()) {
        in.close();
    }
    fileName = _fileName;
    in.open(fileName.toStdString().c_str(), std::ios::in | std::ios::binary);
    if (!in) {
        qDebug() << "failed to open" << fileName;
        setlogName("Fail to OPEN labelfile");
        return  false;
    } else {
        qDebug() << "open successully" << fileName;
    }
    //header
    quint32_be* formatVesion;
    formatVesion = (quint32_be* ) malloc (sizeof(quint32_be));
    buf = (char*) malloc (sizeof(char) * 22);
    in.read(buf, 20);
    formatVesion = (quint32_be *)&buf[16];
    //metaData
    in.seekg(-4, in.end);
    quint32_be* metaDataLength;
    metaDataLength = (quint32_be* ) malloc (sizeof(quint32_be));
    buf = (char*) malloc (sizeof(char) * 4);
    in.read(buf, 4);
    metaDataLength = (quint32_be *)&buf[0];
    in.seekg(-4 - (int)(*metaDataLength), in.end);
    buf = (char*) malloc (sizeof(char) * *metaDataLength);
    in.read(buf, *metaDataLength);
    std::string s (buf, buf + *metaDataLength);
    if (! metaData.ParseFromString(s)) {
        qDebug() << "END of frame message Failed to parse framegroup";
        setlogName("Fail to LOAD labelfile");
    } else {
        qDebug() << "successfully read" << metaData.message_offsets_size() << "frames"
                 << "metaData from" << metaData.num_cameras() + 1 << "cameras";
        QString logOutput = "read success " + fileName;
        setlogName(logOutput);
    }
    return true;
}

int LogEventLabel::ballInMouth(int team, int frameIndex) {
    if (!frames[frameIndex].realBall.valid) {
//        qDebug() << "frame" << frameIndex << "found NO ball";
        return -1;
    }
    for (int robotID = 0 ; robotID < PARAM::ROBOTMAXID; robotID++) {
        if (frames[frameIndex].bot[team][robotID].valid &&
                frames[frameIndex].realBall.pos.dist(frames[frameIndex].bot[team][robotID].pos) >= 70 &&
                frames[frameIndex].realBall.pos.dist(frames[frameIndex].bot[team][robotID].pos) <= 140 &&
                std::abs( frames[frameIndex].bot[team][robotID].dir.theta
                          (CVector(frames[frameIndex].realBall.pos - frames[frameIndex].bot[team][robotID].pos ))) < 3.1416 / 3) {
//            qDebug() << "frame" << frameIndex << "found ball in" << team << robotID << "mouth";
            return robotID;
        }
    }
    return -1;
}


