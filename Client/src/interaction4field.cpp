#include "interaction4field.h"
#include "globalsettings.h"
#include "globaldata.h"
#include "rec_recorder.h"
Interaction4Field::Interaction4Field(QObject *parent) : QObject(parent) {
}
Interaction4Field::~Interaction4Field() {
}
void Interaction4Field::setArea(int a,int b,int c,int d){
    GlobalSettings::instance()->setArea(a,b,c,d);
}
void Interaction4Field::resetArea(){
    GlobalSettings::instance()->resetArea();
}
void Interaction4Field::setSize(int width,int height){
    Field::setSize(width,height);
}
//void Interaction4Field::setPlacementPoint(int x,int y){
//    GlobalSettings::instance()->setBallPlacementPos(x,y);
//}
void Interaction4Field::setCtrlC(){
    GlobalData::instance()->ctrlCMutex.lock();
    GlobalData::instance()->ctrlC = !GlobalData::instance()->ctrlC;
    GlobalData::instance()->ctrlCMutex.unlock();
}
int Interaction4Field::getRealX(int x){// mm
    return (int)Field::fieldXFromCoordinate(x);
}
int Interaction4Field::getRealY(int y){// mm
    return (int)Field::fieldYFromCoordinate(y);
}
void Interaction4Field::setRecorder(bool isRecording) {
    if (isRecording) {
        ZRecRecorder::instance()->init();
    } else {
        ZRecRecorder::instance()->stop();
    }
}

void keyPress(QKeyEvent event);
void keyRelease(QKeyEvent event);
