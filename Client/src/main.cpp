#include <kddockwidgets/qtquick/Platform.h>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "qsingleton.hpp"
#include "visionmodule.h"
#include "interaction.h"
#include "interaction4field.h"
#include "field.h"
#include "plotdisplay.h"
#include "parammanager.h"
#include "paraminterface.h"
#include "simulator.h"
#include "refereebox.h"
#include "debugger.h"
#include "documenthandler.h"
#include "viewerinterface.h"
#include "messageinfo.h"
#include "log/log_slider.h"
#include "log/logeventlabel.h"
#include "rec_slider.h"

void qmlRegister() {
    qmlRegisterType<Field>("ZSS", 1, 0, "Field");
    qmlRegisterType<PlotDisplay>("ZSS", 1, 0, "PlotDisplay");
    // qmlRegisterType<Interaction>("ZSS", 1, 0, "Interaction");
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
    qmlRegisterSingletonType<Interaction>("ZSS", 1, 0, "Interaction",&qinstance<Interaction>);
    ZCommunicator::instance();
    Simulator::instance();
    ZDebugger::instance();
    ZSS::LParamManager::instance()->clear();
}

// class CustomFrameworkWidgetFactory : public KDDockWidgets::QtQuick::ViewFactory{
// public:
//     ~CustomFrameworkWidgetFactory() override = default;
//     QUrl titleBarFilename() const override{ return QUrl("qrc:/src/qml/CustomDW/TitleBar.qml"); }
//     QUrl dockwidgetFilename() const override{ return QUrl("qrc:/src/qml/CustomDW/DockWidget.qml"); }
//     QUrl frameFilename() const override{ return QUrl("qrc:/src/qml/CustomDW/Frame.qml"); }
//     QUrl floatingWindowFilename() const override{ return QUrl("qrc:/src/qml/CustomDW/FloatingWindow.qml"); }
// };

int main(int argc, char *argv[]) {
#ifdef Q_OS_WIN
    QGuiApplication::setAttribute(Qt::AA_UseOpenGLES);
#endif
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QGuiApplication app(argc, argv);
    KDDockWidgets::initFrontend(KDDockWidgets::FrontendType::QtQuick);
    app.setOrganizationName("Turing-zero");
    app.setOrganizationDomain("turing-zero.com");
    qmlRegister();
    app.setFont(QFont("Microsoft YaHei", 9));
    QQmlApplicationEngine engine;
    KDDockWidgets::QtQuick::Platform::instance()->setQmlEngine(&engine);
    engine.load(QUrl(QLatin1String("qrc:/src/qml/main_test.qml")));
    return app.exec();
}
