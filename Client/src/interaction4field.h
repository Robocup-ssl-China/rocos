#ifndef __INTERACTION4FIELD_H__
#define __INTERACTION4FIELD_H__

#include <QObject>
#include "visionmodule.h"
#include "communicator.h"
#include "field.h"
class Interaction4Field : public QObject
{
    Q_OBJECT
public:
    explicit Interaction4Field(QObject *parent = 0);
    Q_INVOKABLE int getFPS(){ return VisionModule::instance()->getFPS();}
    Q_INVOKABLE int getMedusaFPS(int t){ return ZCommunicator::instance()->getFPS(t); }
    Q_INVOKABLE void setArea(int a,int b,int c,int d);
    Q_INVOKABLE void resetArea();
//    Q_INVOKABLE void moveField(int,int);
    Q_INVOKABLE void setSize(int,int);
//    Q_INVOKABLE void setPlacementPoint(int,int);
    Q_INVOKABLE void setCtrlC();
    Q_INVOKABLE int getRealX(int);
    Q_INVOKABLE int getRealY(int);
    Q_INVOKABLE void setRecorder(bool isRecording);
//    Q_INVOKABLE void keyPress(QKeyEvent* event);
//    Q_INVOKABLE void keyRelease(QKeyEvent* event);
    ~Interaction4Field();
Q_SIGNALS:
    void moveFieldSignal(int,int);
signals:
public slots:
};

#endif // __INTERACTION4FIELD_H__
