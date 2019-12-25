#include "log_slider.h"
#include "parammanager.h"
#include <QDebug>


LogSlider::LogSlider(QObject* parent) :
    QObject(parent),
    m_currentFrame(0),
    m_stopped(true) {
    connect(&m_player, SIGNAL(positionChanged(int, double)), SLOT(updateTime(int, double)));
}


void LogSlider::loadFile(QString filename) {
    m_currentFrame = 0;
#ifdef WIN32
    filename.remove(QString("file:///"));
#else
    filename.remove(QString("file://"));
#endif
    int maxFrame;
    double duration;

    if (m_player.load(filename, maxFrame, duration)) {

        setmaxTime(QString("%1:%2.%3")
                   .arg((int) (duration / 1E9) / 60)
                   .arg((int) (duration / 1E9) % 60, 2, 10, QChar('0'))
                   .arg((int) (duration / 1E6) % 1000, 3, 10, QChar('0')));

        setmaximumValue(maxFrame - 1);
        setstepSize((maxFrame - 1) / (duration / 1E9));

        setlogName(filename);
    }
}

void LogSlider::toggleStopped() {
    setStopped(!m_stopped);
//    qDebug() << "toggleStopped worked" << endl;
}

void LogSlider::setStopped(bool p) {
    m_stopped = p | !m_player.good();

    if (m_stopped) {
        m_player.stop();
    } else {
        m_player.start(m_currentFrame);
    }
}

void LogSlider::seekFrame(int value) {
    m_currentFrame = value;

    if (!m_stopped) {
        m_player.stop();
        m_player.start(m_currentFrame);
    }
}

void LogSlider::updateTime(int frame, double time) {
    m_currentFrame = frame;

    setdurTime(QString("%1:%2.%3")
               .arg((int) (time / 1E9) / 60)
               .arg((int) (time / 1E9) % 60, 2, 10, QChar('0'))
               .arg((int) (time / 1E6) % 1000, 3, 10, QChar('0')));
}

bool LogSlider::setStartPoint(QString filename, int breakpoint) {
    filename.remove(QString("file:///"));
    if (m_filename != filename) bpNumber = 0;
    bpNumber++;
    QString value;
    m_filename = filename;
    value.append("Start").append(QString::number(bpNumber));
    filename.replace("/", "...");
    ZSS::LParamManager::instance()->changeParam(filename, value, breakpoint);
    qDebug() << filename;
    return true;
}

bool LogSlider::setStopPoint(QString filename, int breakpoint) {
    QString value;
    value.append("Stop").append(QString::number(bpNumber));
    filename.remove(QString("file:///"));
    filename.replace("/", "...");
    ZSS::LParamManager::instance()->changeParam(filename, value, breakpoint);
    return true;
}
