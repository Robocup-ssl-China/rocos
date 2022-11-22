#ifndef NETWORKINTERFACES_H
#define NETWORKINTERFACES_H
#include <QStringList>
#include <QNetworkInterface>
#include "singleton.hpp"
class NetworkInterfaces{
public:
    NetworkInterfaces();
    void updateInterfaces();
    QStringList getInterfaces(){ return interfaces; }
    QStringList getGrsimInterfaces(){ return grsimInterfaces; }
    QString getLocalAddress();
    bool Ping(const QString ip);
    QNetworkInterface getFromIndex(const int index){ return QNetworkInterface::interfaceFromName(interfaces[index]); }
private:
    QStringList interfaces;
    QStringList grsimInterfaces;
};
typedef Singleton<NetworkInterfaces> ZNetworkInterfaces;
#endif // NETWORKINTERFACES_H
