#ifndef LOGEVENTLABEL_H
#define LOGEVENTLABEL_H
#include "log_labeler_data.pb.h"
#include "log_labels.pb.h"
#include <QObject>
#include <geometry.h>
#include <fstream>
#include <QList>
#include <staticparams.h>
namespace {
struct ball {
    CGeoPoint pos;
    CVector vel;
    int camera_id;
    bool valid = false;
};
struct robot {
    CGeoPoint pos;
    CVector vel;
    CVector dir;
    double dirvel;
    int camera_id;
    bool valid = false;
};
struct frame {
    QList<ball> ballList;
    QList<robot> botList[PARAM::TEAMS][PARAM::ROBOTMAXID];
    ball realBall;
    robot bot[PARAM::TEAMS][PARAM::ROBOTMAXID];
    qint64 timestamp;
    int state = 0;
};

}


class LogEventLabel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString logName READ rlogName WRITE setlogName NOTIFY logNamechanged)

  public:
    explicit LogEventLabel(QObject *parent = nullptr);
    ~LogEventLabel();
    Q_INVOKABLE bool loadFile(QString filename);
    Q_INVOKABLE QString logName = "No label file is loaded.";
    Q_INVOKABLE void processLabel();
    LabelerFrameGroup readFrame(int index);

    QString rlogName() {
        return logName;
    }
    void setlogName(QString slogName) {
        logName = slogName;
        emit logNamechanged();
    }
  signals:
    void logNamechanged();
  public slots:

  private:
    void parseDetection(const SSL_DetectionFrame& detection, ::frame & _frame);
    void processSingelReferee(int startFrame, int endFrame);
    void saveLabel(std::string);
    int ballInMouth(int team, int frameIndex);
    std::ifstream in;
    std::ofstream out;
    LabelerMetadata metaData;
    QString fileName;
    Labels resultLabels;
    char *buf;
    QList<::frame> frames;
    const int BALL_POSSESSION_BLUE = 0x01;
    const int BALL_POSSESSION_YELLOW = 0x02;
    const int BALL_POSSESSION_UNKNOWN = 0x04;

};

#endif // LOGEVENTLABEL_H
