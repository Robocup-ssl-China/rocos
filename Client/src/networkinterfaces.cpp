#include <QProcess>
#include "networkinterfaces.h"
NetworkInterfaces::NetworkInterfaces(){}
void NetworkInterfaces::updateInterfaces(){
    this->interfaces.clear();
    for(auto interface : QNetworkInterface::allInterfaces()){
        interfaces.append(interface.humanReadableName());
    }
}
bool NetworkInterfaces::set(const std::string& name,const int index){
    indexes[name] = index;
    return true;
}
bool NetworkInterfaces::legal(const std::string& name){
    // check if exist
    if(indexes.find(name) == indexes.end()){
        return false;
    }
    // check value
    if(indexes[name] < 0 || indexes[name] >= interfaces.length()){
        return false;
    }
    return true;
}
QNetworkInterface NetworkInterfaces::get(const std::string& name){
    if(legal(name)){
        return QNetworkInterface::interfaceFromName(interfaces[indexes[name]]);
    }
    return QNetworkInterface();
}