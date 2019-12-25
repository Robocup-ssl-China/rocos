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
    QNetworkInterface getFromIndex(const int index){ return QNetworkInterface::interfaceFromName(interfaces[index]); }
private:
    QStringList interfaces;
};
typedef Singleton<NetworkInterfaces> ZNetworkInterfaces;
#endif // NETWORKINTERFACES_H
