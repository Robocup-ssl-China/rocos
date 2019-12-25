#ifndef REC_PLAYER_H
#define REC_PLAYER_H
#include <QThread>
#include <QFile>


class RecPlayer : public QThread
{
    Q_OBJECT
public:
    RecPlayer();
    bool loadRec(QString& filename, int& maxframe);
    bool start(int position);
    void stop();
    bool good();
    void pause();
    void slowly(int position);
signals:
    void positionChanged(int frame);
private:
    void run();
    void sendMessage(const QByteArray& packet);
    bool _run;
    int _currentFrame;
    QFile* replayFile;
    QList <QByteArray> packets;
};

#endif // REC_PLAYER_H
