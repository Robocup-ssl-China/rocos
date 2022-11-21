#include "networkinterfaces.h"
NetworkInterfaces::NetworkInterfaces()
{
    this->grsimInterfaces.clear();
    grsimInterfaces.append(QString("127.0.0.1"));
    grsimInterfaces.append(QString("192.168.0.12"));
    grsimInterfaces.append(QString("192.168.0.15"));
    grsimInterfaces.append(QString("10.12.225.233"));
}
void NetworkInterfaces::updateInterfaces(){
    this->interfaces.clear();
    for(auto interface : QNetworkInterface::allInterfaces()){
        interfaces.append(interface.humanReadableName());
    }
}
