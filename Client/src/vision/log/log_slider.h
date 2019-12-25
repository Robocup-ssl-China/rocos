#ifndef LOG_SLIDER_H
#define LOG_SLIDER_H

#include <QObject>
#include <QString>
#include <iostream>
#include <string>
#include "player.h"

class LogSlider : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString maxTime READ rmaxTime WRITE setmaxTime NOTIFY maxTimechanged)
    Q_PROPERTY(QString durTime READ rdurTime WRITE setdurTime NOTIFY durTimechanged)
    Q_PROPERTY(QString logName READ rlogName WRITE setlogName NOTIFY logNamechanged)
    Q_PROPERTY(double maximumValue READ rmaximumValue WRITE setmaximumValue NOTIFY maximumValuechanged)
    Q_PROPERTY(double stepSize READ rstepSize WRITE setstepSize NOTIFY stepSizechanged)

  public:
    explicit LogSlider(QObject* parent = nullptr);
    Q_INVOKABLE QString maxTime = "00:00.000";
    Q_INVOKABLE QString durTime = "00:00.000";
    Q_INVOKABLE QString logName = "No log file is playing.";
    Q_INVOKABLE double maximumValue = 1;
    Q_INVOKABLE double stepSize = 1;
    Q_INVOKABLE void toggleStopped();
    Q_INVOKABLE void seekFrame(int value);
    Q_INVOKABLE bool setStartPoint(QString filename, int breakpoint);
    Q_INVOKABLE bool setStopPoint(QString filename, int breakpoint);
    QString rmaxTime() {
        return maxTime;
    }
    QString rdurTime() {
        return durTime;
    }
    QString rlogName() {
        return logName;
    }
    double rmaximumValue() {
        return maximumValue;
    }
    double rstepSize() {
        return stepSize;
    }
    void setmaxTime(QString smaxTime) {
        maxTime = smaxTime;
        emit maxTimechanged();
    }
    void setdurTime(QString sdurTime) {
        durTime = sdurTime;
        emit durTimechanged();
    }
    void setlogName(QString slogName) {
        logName = slogName;
        emit logNamechanged();
    }
    void setmaximumValue(double smaximumValue) {
        maximumValue = smaximumValue;
        emit maximumValuechanged();
    }
    void setstepSize(double sstepSize) {
        stepSize = sstepSize;
        emit stepSizechanged();
    }
    Q_INVOKABLE void loadFile(QString filename);

    void setStopped(bool p);

  signals:
    void maxTimechanged();
    void durTimechanged();
    void logNamechanged();
    void maximumValuechanged();
    void stepSizechanged();

  private:
    Player m_player;
    int m_currentFrame;
    bool m_stopped;
    int bpNumber = 0;
    QString m_filename = NULL;

  private slots:
    void updateTime(int frame, double time);
};

#endif // LOG_SLIDER_H
