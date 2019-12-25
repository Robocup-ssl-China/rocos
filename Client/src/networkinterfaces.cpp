#include "networkinterfaces.h"
NetworkInterfaces::NetworkInterfaces()
{

}
void NetworkInterfaces::updateInterfaces(){
    this->interfaces.clear();
    for(auto interface : QNetworkInterface::allInterfaces()){
        interfaces.append(interface.humanReadableName());
    }
}
