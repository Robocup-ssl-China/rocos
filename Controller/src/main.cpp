#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QtDebug>
#include <QFont>
#include <iostream>
#include "serialobject.h"
#include "radiopacket.h"
#include "udp24l01object.h"
#include "udpwifiobject.h"
#include "settings.hpp"
#include "translation.hpp"
using namespace std;

void qmlRegister(){
    qmlRegisterType<SerialObject>("Client.Component", 1, 0, "Serial");
    qmlRegisterType<TranslationTest>("Client.Component", 1, 0, "Translator");
    qmlRegisterType<SettingsObject>("Client.Component", 1, 0, "Settings");
    qmlRegisterType<udp24l01object>("Client.Component", 1, 0, "Udp24L01");
    qmlRegisterType<udpwifiobject>("Client.Component", 1, 0, "UdpWiFi");
}
int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

#if defined(Q_OS_WIN32)
    app.setFont(QFont("Microsoft YaHei",9));
#elif defined(Q_OS_MAC)
    app.setFont(QFont("Helvetica",14));
#endif
    qmlRegister();
    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/src/qml/main.qml")));
    return app.exec();
}
