#include "player.h"
#include "timer.h"
#include "visionmodule.h"
#include <iostream>
#include <QFileInfo>
#include <QtDebug>

Player::Player() {
//    // create referee socket
//    Q_ASSERT(m_referee == NULL);
//    m_referee = new Network(QHostAddress("224.5.23.1"), 0, 10003);
//    m_referee->connect();

//    // create vision socket
//    Q_ASSERT(m_vision == NULL);
//    m_vision = new Network(QHostAddress("224.5.23.2"), 0, 10006);
//    m_vision->connect();

//    // create legacy vision socket
//    Q_ASSERT(m_legacyVision == NULL);
//    m_legacyVision = new Network(QHostAddress("224.5.23.2"), 0, 10005);
//    m_legacyVision->connect();
}

Player::~Player() {
    stop();

    qDeleteAll(packets);
    packets.clear();
}

bool Player::load(const QString& filename, int& maxFrame, double& duration) {
    QFileInfo fileInfo(filename);

    bool compressed = false;

    if (fileInfo.suffix() == "gz") {
        compressed = true;
    }

    LogFile file(filename, compressed);

    if (!file.openRead()) {
        return false;
    }

    qDeleteAll(packets);
    packets.clear();

    for (;;) {

        Frame* packet = new Frame;

        if (!file.readMessage(packet->data, packet->time, packet->type) ||
                packet->time == 0 || packet->type == 0   ) {
            delete packet;
            break;
        }

        packets.append(packet);
        //qDebug() << packet->time <<" "<<packet->type<< endl;
    }

    maxFrame = packets.size() - 1;
    duration = packets.last()->time - packets.first()->time;
    return true;
}

bool Player::start(int position) {
    if (position > packets.size() - 1) {
        return false;
    }

    m_currentFrame = position;

    m_mayRun = true;
    QThread::start();

    return true;
}

void Player::stop() {
    m_mayRun = false;
    wait();
}

bool Player::good() {
    return packets.size() > 0;
}

void Player::sendMessage(const Frame* packet) {
    if (packet->type == MESSAGE_BLANK) {
        // ignore
    } else if (packet->type == MESSAGE_UNKNOWN) {
        std::cout << "Error unsupported or corrupt packet found in log file!" << std::endl;
    } else if (packet->type == MESSAGE_SSL_VISION_2010) {
        //this is 10005
        VisionModule::instance()->parse((void *)packet->data.data(), packet->data.size());
    } else if (packet->type == MESSAGE_SSL_REFBOX_2013) {
        //unknown
    } else if (packet->type == MESSAGE_SSL_VISION_2014) {
        VisionModule::instance()->parse((void *)packet->data.data(), packet->data.size());
    } else {
        std::cout << "Error unsupported message type found in log file!" << std::endl;
    }
}

void Player::run() {
    emit VisionModule::instance()->needDraw();
    sendMessage(packets.at(m_currentFrame));

    const qint64 startTime = Timer::systemTime();
    const qint64 referenceTime = packets.at(m_currentFrame)->time;

    while (m_mayRun && ++m_currentFrame < packets.size() && this->isRunning()) {
        Frame* packet = packets.at(m_currentFrame);

        qint64 sleepTime = ((packet->time - referenceTime) - (Timer::systemTime() - startTime)) / 1000;

        if (sleepTime > 0) {
            QThread::currentThread()->usleep(sleepTime);
        }

        sendMessage(packet);

        emit positionChanged(m_currentFrame, packet->time - packets.first()->time);
//        qDebug() << packet->time << packets.first()->time << packet->time - packets.first()->time << endl;
    }

    emit finished();
}
