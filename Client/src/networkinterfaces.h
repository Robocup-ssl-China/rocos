#ifndef NETWORKINTERFACES_H
#define NETWORKINTERFACES_H
#include <QStringList>
#include <QNetworkInterface>
#include <QMutex>
#include <QMutexLocker>
#include <map>
#include "singleton.hpp"
class NetworkInterfaces{
public:
    NetworkInterfaces();
    void updateInterfaces();
    void refreshGrsimInterfaces(QList<quint16> ports = {10020, 10006}, int perIfaceTimeoutMs = 300);
    QStringList getInterfaces(){
        QMutexLocker locker(&mutex);
        return interfaces;
    }
    void setInterface(const std::string& key, const int index){
        QMutexLocker locker(&mutex);
        if (index >= 0 && index < interfaces.size()) {
            interfaceMap[key] = index;
        }
    }
    QNetworkInterface getInterface(const std::string& key){
        QMutexLocker locker(&mutex);
        auto it = interfaceMap.find(key);
        if (it != interfaceMap.end() && it->second >= 0 && it->second < interfaces.size()) {
            return QNetworkInterface::interfaceFromName(interfaces[it->second]);
        }
        return QNetworkInterface::interfaceFromName(QString());
    }
    QStringList getAvailableIPs(){
        QMutexLocker locker(&mutex);
        return grsimInterfaces;
    }
    void setIP(const std::string& key, const int index){
        QMutexLocker locker(&mutex);
        if (grsimInterfaces.isEmpty()) {
            ipMap[key] = -1;
        } else if (index < 0 || index >= grsimInterfaces.size()) {
            ipMap[key] = 0;
        } else {
            ipMap[key] = index;
        }
    }
    QString getIP(const std::string& key){
        QMutexLocker locker(&mutex);
        auto it = ipMap.find(key);
        if (it != ipMap.end()) {
            int index = it->second;
            if (index >= 0 && index < grsimInterfaces.size()) {
                return grsimInterfaces[index];
            }
        }
        return grsimInterfaces.isEmpty() ? QString() : grsimInterfaces.first();
    }
    QString getLocalAddress();
    QNetworkInterface getFromIndex(const int index){
        QMutexLocker locker(&mutex);
        if (index >= 0 && index < interfaces.size()) {
            return QNetworkInterface::interfaceFromName(interfaces[index]);
        }
        return QNetworkInterface::interfaceFromName(QString());
    }
    QString getIPFromIndex(const int index){
        QMutexLocker locker(&mutex);
        if (index < 0 || index >= grsimInterfaces.size()) {
            return QString();
        }
        return grsimInterfaces[index];
    }
private:
    QList<QNetworkInterface> activeNonLoInterfaces() const;

    QStringList interfaces;
    QStringList grsimInterfaces;
    std::map<std::string, int> interfaceMap;
    std::map<std::string, int> ipMap;
    mutable QMutex mutex;
};
typedef Singleton<NetworkInterfaces> ZNetworkInterfaces;
#endif // NETWORKINTERFACES_H
