#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "visionmodule.h"
#include "interaction.h"
#include "interaction4field.h"
#include "field.h"
#include "display.h"
#include "parammanager.h"
#include "paraminterface.h"
#include "simulator.h"
#include "refereebox.h"
#include "debugger.h"
#include "test.h"
#include "documenthandler.h"
#include "viewerinterface.h"
#include "messageinfo.h"
#include "log/log_slider.h"
#include "log/logeventlabel.h"
#include "rec_slider.h"

namespace ZSS {
}
void qmlRegister() {
    qmlRegisterType<Field>("ZSS", 1, 0, "Field");
    qmlRegisterType<Display>("ZSS", 1, 0, "Display");
    qmlRegisterType<Interaction>("ZSS", 1, 0, "Interaction");
    qmlRegisterType<Interaction4Field>("ZSS", 1, 0, "Interaction4Field");
    qmlRegisterType<RefBoxCommand>("ZSS", 1, 0, "RefBoxCommand");
    qmlRegisterType<GameState>("ZSS", 1, 0, "GameState");
    qmlRegisterType<RefereeBox>("ZSS", 1, 0, "RefereeBox");
    qmlRegisterType<ParamInterface>("ZSS", 1, 0, "ParamModel");
    qmlRegisterType<DocumentHandler>("ZSSDocument", 1, 0, "DocumentHandler");
    qmlRegisterType<ViewerInterface>("ZSS", 1, 0, "ViewerInterface");
    qmlRegisterType<LogSlider>("ZSS", 1, 0, "LogSlider");
    qmlRegisterType<LogEventLabel>("ZSS", 1, 0, "LogEventLabel");
    //qmlRegisterType<Test>("ZSS", 1, 0, "Test");
    qmlRegisterSingletonType<MessageInfo>("ZSS", 1, 0, "MessageInfo", &MessageInfo::instance);
    qmlRegisterSingletonType<rec_slider>("ZSS", 1, 0, "RecSlider", &rec_slider::instance);
    ZCommunicator::instance();
    Simulator::instance();
    ZDebugger::instance();
    ZSS::LParamManager::instance()->clear();
}

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    app.setOrganizationName("Turing-zero");
    app.setOrganizationDomain("turing-zero.com");
    qmlRegister();
    app.setFont(QFont("Microsoft YaHei", 9));
    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/src/qml/main.qml")));
    return app.exec();
}
