#ifndef PLAYER_H
#define PLAYER_H

#include "log_file.h"
#include <QThread>

class Player : public QThread
{
    Q_OBJECT

public:
    struct Frame {
        qint64 time;
        MessageType type;
        QByteArray data;
    };

public:
    Player();
    ~Player();

signals:
    void positionChanged(int frame, double time);
    void finished();
//    void needDraw();

public:
    bool load(const QString& filename, int& maxFrame, double& duration);
    bool start(int position);
    void stop();
    void pause(bool pause);
    bool good();

private:
    void run();
    void sendMessage(const Frame* packet);

private:
    QList<Frame*> packets;
    bool m_mayRun;
    int m_currentFrame;
};

#endif // PLAYER_H
