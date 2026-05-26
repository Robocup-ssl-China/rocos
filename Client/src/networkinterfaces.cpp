#include "networkinterfaces.h"
#include <QHostAddress>
#include <QNetworkAddressEntry>
#include <QSet>
#include <QUdpSocket>
#include "staticparams.h"

NetworkInterfaces::NetworkInterfaces(){
    updateInterfaces();
    QMutexLocker locker(&mutex);
    grsimInterfaces.clear();
    grsimInterfaces.append(QStringLiteral("127.0.0.1"));
    ipMap["grSim"] = 0;
}

QList<QNetworkInterface> NetworkInterfaces::activeNonLoInterfaces() const {
    QList<QNetworkInterface> result;
    for (const QNetworkInterface &iface : QNetworkInterface::allInterfaces()) {
        if (iface.flags().testFlag(QNetworkInterface::IsLoopBack))
            continue;
        if (!iface.flags().testFlag(QNetworkInterface::IsUp))
            continue;
        if (!iface.flags().testFlag(QNetworkInterface::IsRunning))
            continue;

        bool hasIPv4 = false;
        for (const QNetworkAddressEntry &entry : iface.addressEntries()) {
            if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                hasIPv4 = true;
                break;
            }
        }
        if (hasIPv4)
            result.append(iface);
    }
    return result;
}

QString NetworkInterfaces::getLocalAddress()
{
    QString ip_address;
    for(auto interface : QNetworkInterface::allAddresses()){
//        qDebug() << interface.toString() << (interface != QHostAddress::LocalHost) << (interface.toIPv4Address());
        if (interface != QHostAddress::LocalHost && interface.toIPv4Address()){
            ip_address = interface.toString();
        }
    }
    if (ip_address.isEmpty()){
        return QHostAddress(QHostAddress::LocalHost).toString();
    }
    return ip_address;
}

void NetworkInterfaces::updateInterfaces(){
    QStringList newInterfaces;
    for (const QNetworkInterface &interface : QNetworkInterface::allInterfaces()) {
        newInterfaces.append(interface.humanReadableName());
    }

    QMutexLocker locker(&mutex);
    interfaces = newInterfaces;
    for (auto &entry : interfaceMap) {
        if (interfaces.isEmpty()) {
            entry.second = -1;
        } else if (entry.second < 0 || entry.second >= interfaces.size()) {
            entry.second = 0;
        }
    }
}

int NetworkInterfaces::addGrsimIP(const QString& ip, const std::string& key) {
    QHostAddress addr;
    if (!addr.setAddress(ip.trimmed()) || addr.protocol() != QAbstractSocket::IPv4Protocol) {
        return -1;
    }

    QString normalized = addr.toString();
    QMutexLocker locker(&mutex);
    int index = grsimInterfaces.indexOf(normalized);
    if (index < 0) {
        grsimInterfaces.append(normalized);
        index = grsimInterfaces.size() - 1;
    }
    ipMap[key] = index;
    return index;
}

void NetworkInterfaces::refreshGrsimInterfaces(QList<quint16> ports, int perIfaceTimeoutMs) {
    updateInterfaces();

    QSet<quint16> uniquePorts;
    QList<quint16> candidatePorts;
    for (quint16 port : ports) {
        if (port == 0 || uniquePorts.contains(port))
            continue;
        uniquePorts.insert(port);
        candidatePorts.append(port);
    }
    if (candidatePorts.isEmpty()) {
        candidatePorts.append(10020);
        candidatePorts.append(10006);
    }

    QStringList newList;
    newList.append(QStringLiteral("127.0.0.1"));

    const QHostAddress multicastGroup(ZSS::SSL_ADDRESS);
    QSet<QString> detectedSenders;
    const QList<QNetworkInterface> candidateIfaces = activeNonLoInterfaces();
    for (const QNetworkInterface &iface : candidateIfaces) {
        for (quint16 port : candidatePorts) {
            QUdpSocket sock;
            sock.setSocketOption(QAbstractSocket::MulticastLoopbackOption, 0);

            if (!sock.bind(QHostAddress::AnyIPv4, port,
                           QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
                continue;
            }
            if (!sock.joinMulticastGroup(multicastGroup, iface)) {
                sock.close();
                continue;
            }

            const bool hasData = sock.waitForReadyRead(perIfaceTimeoutMs);
            if (hasData) {
                while (sock.hasPendingDatagrams()) {
                    QByteArray datagram;
                    datagram.resize(static_cast<int>(sock.pendingDatagramSize()));

                    QHostAddress senderAddress;
                    quint16 senderPort = 0;
                    sock.readDatagram(datagram.data(), datagram.size(), &senderAddress, &senderPort);

                    if (senderAddress.protocol() == QAbstractSocket::IPv4Protocol) {
                        const QString senderIp = senderAddress.toString();
                        if (!senderIp.isEmpty() && senderIp != QStringLiteral("0.0.0.0")) {
                            detectedSenders.insert(senderIp);
                        }
                    }
                }
            }
            sock.close();
        }
    }

    for (const QString &senderIp : detectedSenders) {
        if (!newList.contains(senderIp)) {
            newList.append(senderIp);
        }
    }

    QMutexLocker locker(&mutex);
    grsimInterfaces = newList;
    if (ipMap.find("grSim") == ipMap.end()) {
        ipMap["grSim"] = grsimInterfaces.isEmpty() ? -1 : 0;
    }
    for (auto &entry : ipMap) {
        if (grsimInterfaces.isEmpty()) {
            entry.second = -1;
        } else if (entry.second < 0 || entry.second >= grsimInterfaces.size()) {
            entry.second = 0;
        }
    }
}
