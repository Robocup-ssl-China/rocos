#ifndef NETWORKINTERFACES_H
#define NETWORKINTERFACES_H
#include <QStringList>
#include <QNetworkInterface>
#include <map>
#include "zos/utils/singleton.h"
class NetworkInterfaces: public Singleton<NetworkInterfaces>{
public:
    NetworkInterfaces();
    void updateInterfaces();
    QStringList getInterfaces(){ return interfaces; }
    QNetworkInterface getFromIndex(const int index){ return QNetworkInterface::interfaceFromName(interfaces[index]); }
    bool set(const std::string& name,const int);
    bool legal(const std::string& name);
    QNetworkInterface get(const std::string&);
private:
    std::map<std::string,int> indexes;
    QStringList interfaces;
};
#endif // NETWORKINTERFACES_H
