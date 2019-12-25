#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <QObject>
#include <QUdpSocket>
#include "singleton.hpp"
// temp before merge medusa TODO
class Debugger : public QObject
{
    Q_OBJECT
public:
    explicit Debugger(QObject *parent = nullptr);
public slots:
    void receiveBlue();
    void receiveYellow();
private:
    QUdpSocket receiverBlue;
    QUdpSocket receiverYellow;
};
typedef Singleton<Debugger> ZDebugger;
#endif // DEBUGGER_H
