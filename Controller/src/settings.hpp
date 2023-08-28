#ifndef SETTINGSOBJECT_H
#define SETTINGSOBJECT_H

#include <QObject>
#include <QString>
#include <QDebug>
namespace  {
QStringList CommunicationWay = {"Serial_24L01","UDP_24L01","UDP_WIFI"};
};
class SettingsObject : public QObject
{
    Q_OBJECT
public:
    SettingsObject(QObject * parent=0):QObject(parent),currentComm("Serial_24L01"){}
    Q_INVOKABLE QStringList getCommunicationList() {return CommunicationWay;}
    Q_INVOKABLE void changeCommunication(int index) { currentComm = CommunicationWay[index]; }
    Q_INVOKABLE QString getCurrentComm(){  return currentComm; }
private:
    QString currentComm;
};
#endif
