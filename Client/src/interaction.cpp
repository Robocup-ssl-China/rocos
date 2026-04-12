#include "interaction.h"
#include "visionmodule.h"
#include "globaldata.h"
#include "maintain.h"
#include "actionmodule.h"
#include "globalsettings.h"
#include "simulator.h"
#include "parammanager.h"
#include "simmodule.h"
#include "rec_recorder.h"
#include "networkinterfaces.h"
#include <QCoreApplication>
#include <QProcess>
namespace {
QProcess *medusaProcess = nullptr;
QProcess *medusaProcess2 = nullptr;
QProcess *simProcess = nullptr;
QProcess *crazyProcess = nullptr;
QProcess *monitorProcess = nullptr;
}
Interaction::Interaction(QObject *parent) : QObject(parent) {
}
Interaction::~Interaction() {
    if (simProcess != nullptr) {
        if (simProcess->isOpen()) {
            simProcess->close();
        }
        delete simProcess;
        simProcess = nullptr;
        QTextStream(stdout) << "\n------------------------------------\nexit Sim\n------------------------------------\n";
    }
    if (medusaProcess != nullptr) {
        if (medusaProcess->isOpen()) {
            medusaProcess->close();
        }
        delete medusaProcess;
        medusaProcess = nullptr;
        QTextStream(stdout) << "\n------------------------------------\nexit Core\n------------------------------------\n";
    }
    if (medusaProcess2 != nullptr) {
        if (medusaProcess2->isOpen()) {
            medusaProcess2->close();
        }
        delete medusaProcess2;
        medusaProcess2 = nullptr;
        QTextStream(stdout) << "\n------------------------------------\nexit 2 Core\n------------------------------------\n";
    }
    if (crazyProcess != nullptr) {
        if (crazyProcess->isOpen()) {
            crazyProcess->close();
        }
        delete crazyProcess;
        crazyProcess = nullptr;
        QTextStream(stdout) << "\n------------------------------------\nexit Crazy\n------------------------------------\n";
    }
    if (monitorProcess != nullptr) {
        if (monitorProcess->isOpen()) {
            monitorProcess->close();
        }
        delete monitorProcess;
        monitorProcess = nullptr;
        QTextStream(stdout) << "\n------------------------------------\nexit Monitor\n------------------------------------\n";
    }
}
void Interaction::updateInterfaces(){
    NetworkInterfaces::instance()->updateInterfaces();
}
QStringList Interaction::getInterfaces(){
    return NetworkInterfaces::instance()->getInterfaces();
}
void Interaction::changeVisionInterface(int index){
    NetworkInterfaces::instance()->set("vision",index);
}
void Interaction::changeRadioInterface(bool ifBlue,bool ifSender,int index){
//    qDebug() << "radio  interface : " << ifBlue << ifSender << index;
}
void Interaction::setVision(bool needStart, bool real) {
    // ZSS::ZActionModule::instance()->setSimulation(!real);
    if (needStart) {
        ZSS::ZParamManager::instance()->changeParam("Alert/IsSimulation", !real);
        ZCommunicator::instance()->reloadSimulation();
        VisionModule::instance()->udpSocketConnect(real);
//        if (real) RefereeThread::instance()->start();
//        Maintain::instance()->udpSocketConnect();
    } else {
        ZSS::ZParamManager::instance()->changeParam("Alert/IsSimulation", !real);
        ZCommunicator::instance()->reloadSimulation();
        VisionModule::instance()->udpSocketDisconnect();
//        RefereeThread::instance()->disconnectTCP();
        //Maintain::instance()->udpSocketDisconnect();
    }
}
void Interaction::controlCamera(int index, bool state) {
    GlobalData::instance()->cameraControl[index] = state;
}
bool Interaction::connectRadio(bool sw, int id, int frq) {
    if(sw) {
        ZCommunicator::instance()->disconnectMedusa(id);
        ZCommunicator::instance()->connectMedusa(id);
        // ZSS::ZActionModule::instance()->disconnectRadio(id);
        // return ZSS::ZActionModule::instance()->connectRadio(id, frq);
    } else {
//        return ZSS::ZActionModule::instance()->disconnectRadio(id);
        return true;
    }
    return false;
}
bool Interaction::connectSim(bool sw, int id, bool color) {
    if(sw) {
        ZCommunicator::instance()->disconnectMedusa(id);
        ZCommunicator::instance()->connectMedusa(id);
        ZSS::SimModule::instance()->disconnectSim(color);
        return ZSS::SimModule::instance()->connectSim(color);
    } else {
        return ZSS::SimModule::instance()->disconnectSim(color);
    }
}
bool Interaction::controlMonitor(bool control) {
#ifdef WIN32
    if(control == false) {
        if (monitorProcess != nullptr) {
            if (monitorProcess->isOpen()) {
                monitorProcess->close();
            }
            delete monitorProcess;
            monitorProcess = nullptr;
            QTextStream(stdout) << "\n------------------------------------\nexit Monitor\n------------------------------------\n";
        }
    } else {
        monitorProcess = new QProcess();
        QString name = "./ProcessAlive.exe";
        monitorProcess->start(name);
        QTextStream(stdout) << "\n------------------------------------\n" << "running " << name << "\n------------------------------------\n";
    }
    return true;
#else
    if(control == false) {
        if (monitorProcess != nullptr) {
            if (monitorProcess->isOpen()) {
                monitorProcess->close();
            }
            delete monitorProcess;
            monitorProcess = nullptr;
            QTextStream(stdout) << "\n------------------------------------\nexit Monitor\n------------------------------------\n";
        }
    } else {
        monitorProcess = new QProcess();
        QString name = "sh ProcessAlive.sh";
        monitorProcess->start(name);
        QTextStream(stdout) << "\n------------------------------------\n" << "start : " << name << "\n------------------------------------\n";
    }
    return true;
#endif
}
bool Interaction::controlMedusa(bool control) {
    if(control == false) {
        if (medusaProcess != nullptr) {
            if (medusaProcess->isOpen()) {
                medusaProcess->close();
            }
            delete medusaProcess;
            medusaProcess = nullptr;
            QTextStream(stdout) << "\n------------------------------------\nexit Core\n------------------------------------\n";
        }
        emit GlobalSettings::instance()->clearOutput();
    } else {
        medusaProcess = new QProcess();
        QString name = "./Core";
        connect(medusaProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(medusaPrint()));
        medusaProcess->start(name);
        QTextStream(stdout) << "\n------------------------------------\n" << "running " << name << "\n------------------------------------\n";
    }
    return true;
}
bool Interaction::controlMedusa2(bool control) {
    if(control == false) {
        if (medusaProcess2 != nullptr) {
            if (medusaProcess2->isOpen()) {
                medusaProcess2->close();
            }
            delete medusaProcess2;
            medusaProcess2 = nullptr;
            QTextStream(stdout) << "\n------------------------------------\nexit 2 Core\n------------------------------------\n";
        }
    } else {
        medusaProcess2 = new QProcess();
        QString name = "./Core";
        medusaProcess2->start(name);
        QTextStream(stdout) << "\n------------------------------------\n" << "running 2 " << name << "\n------------------------------------\n";
    }
    return true;
}
bool Interaction::controlSim(bool control, bool show) {
    if(control == false) {
        if (simProcess != nullptr) {
            if (simProcess->isOpen()) {
                simProcess->close();
            }
            delete simProcess;
            simProcess = nullptr;
            QTextStream(stdout) << "\n------------------------------------\nexit Sim\n------------------------------------\n";
        }
    } else {
        simProcess = new QProcess();
        QString name = "./grsim";
        QStringList args;
        if(!show) args << "-H";
        simProcess->start(name, args);
        QTextStream(stdout) << "\n------------------------------------\n" << "running " << name << "\n------------------------------------\n";
    }
    return true;
}
bool Interaction::controlCrazy(bool control) {
    if(control == false) {
        if (crazyProcess != nullptr) {
            if (crazyProcess->isOpen()) {
                crazyProcess->close();
            }
            delete crazyProcess;
            crazyProcess = nullptr;
            QTextStream(stdout) << "\n------------------------------------\nexit Crazy\n------------------------------------\n";
        }
    } else {
        crazyProcess = new QProcess();
        QString name = "./Crazy.exe";
        crazyProcess->start(name);
        QTextStream(stdout) << "\n------------------------------------\n" << "running " << name << "\n------------------------------------\n";
    }
    return true;
}
bool Interaction::changeMedusaSettings(bool color, bool side) {
    ZSS::ZParamManager::instance()->changeParam("ZAlert/IsYellow", color);
    ZSS::ZParamManager::instance()->changeParam("ZAlert/IsRight", side);
    ZSS::NActionModule::instance()->setMedusaSettings(color,side);
    return true;
}
bool Interaction::changeTestSettings(bool color, bool test,int script_index){
    QString team = color ? "Yellow" : "Blue";
    QString script_name;
    bool test_res = false;
    bool res = false;
    if(test){
        if(script_index < 0 || script_index >= _test_script_show_name_list.size()) {
            script_name = "";
            test_res = false;
            res = false;
        }else{
            script_name = _test_script_show_name_list[script_index];
            test_res = true;
            res = true;
        }
    }else{
        script_name = "";
        test_res = false;
        res = true;
    }
    ZSS::ZParamManager::instance()->changeParam("ZAlert/"+team+"_IsTest", test_res);
    ZSS::ZParamManager::instance()->changeParam("ZAlert/"+team+"_TestScriptName", script_name);
    return res;
}
bool Interaction::changeRefConfigSettings(bool color, bool ref,int config_index){
    QString team = color ? "Yellow" : "Blue";
    QString config_name;
    bool ref_res = false;
    bool res = false;
    if(ref){
        if(config_index < 0 || config_index >= _ref_config_show_name_list.size()) {
            config_name = "";
            ref_res = false;
            res = false;
        }else{
            config_name = _ref_config_show_name_list[config_index];
            ref_res = true;
            res = true;
        }
    }else{
        config_name = "";
        ref_res = false;
        res = true;
    }
    ZSS::ZParamManager::instance()->changeParam("ZAlert/"+team+"_UseRefConfig", ref_res);
    ZSS::ZParamManager::instance()->changeParam("ZAlert/"+team+"_RefConfigName", config_name);
    return res;
}
bool Interaction::getTestSettings(bool color){
    QString team = color ? "Yellow" : "Blue";
    bool res;
    ZSS::ZParamManager::instance()->loadParam(res,"ZAlert/"+team+"_IsTest");
    return res;
}
bool Interaction::getRefConfigSetting(bool color){
    QString team = color ? "Yellow" : "Blue";
    bool res;
    ZSS::ZParamManager::instance()->loadParam(res,"ZAlert/"+team+"_UseRefConfig");
    return res;
}

int Interaction::getTestScriptIndex(bool color){
    QString team = color ? "Yellow" : "Blue";
    QString res;
    ZSS::ZParamManager::instance()->loadParam(res,"ZAlert/"+team+"_TestScriptName");
    qDebug() << "getTestScriptIndex : " << res;
    int index = _test_script_show_name_list.indexOf(res);
    return index > 0 ? index : 0;
}
int Interaction::getRefConfigIndex(bool color){
    QString team = color ? "Yellow" : "Blue";
    QString res;
    ZSS::ZParamManager::instance()->loadParam(res,"ZAlert/"+team+"_RefConfigName");
    qDebug() << "getRefConfigIndex : " << res;
    int index = _ref_config_show_name_list.indexOf(res);
    return index > 0 ? index : 0;
}

void Interaction::medusaPrint() {
    emit GlobalSettings::instance()->addOutput(medusaProcess->readAllStandardOutput());
}
void Interaction::robotControl(int id, int team) {
    Simulator::instance()->controlRobot(id, team == 1);
}

void Interaction::kill() {
    ZRecRecorder::instance()->stop();

    // Stop managed helper processes first.
    controlMonitor(false);
    controlMedusa(false);
    controlMedusa2(false);
    controlSim(false, false);
    controlCrazy(false);

#ifdef WIN32
//    RefereeThread::instance()->disconnectTCP();
    QProcess::execute("taskkill", {"-im", "Core.exe", "-f"});
    QProcess::execute("taskkill", {"-im", "Client.exe", "-f"});
#else
    QProcess::execute("pkill", {"-x", "Core"});
    QProcess::execute("pkill", {"-x", "grsim"});
    QProcess::execute("pkill", {"-x", "Client"});
#endif

    // Always close current UI as a fallback in case external kill command fails.
    if (QCoreApplication::instance()) {
        QCoreApplication::quit();
    }

    if (monitorProcess != nullptr) {
        if (monitorProcess->isOpen()) {
            monitorProcess->close();
        }
        delete monitorProcess;
        monitorProcess = nullptr;
    }
}
bool Interaction::connectSerialPort(bool sw){
    if(sw){
        return ZSS::NActionModule::instance()->init();
    }
    return ZSS::NActionModule::instance()->closeSerialPort();
}

bool Interaction::changeSerialFrequency(int frequency){
    return ZSS::NActionModule::instance()->changeFrequency(frequency);
}

bool Interaction::changeSerialPort(int index){
    return ZSS::NActionModule::instance()->changePorts(index);
}

QStringList Interaction::getSerialPortsList(){
    return ZSS::NActionModule::instance()->updatePortsList();
}
int Interaction::getFrequency(){
    return ZSS::NActionModule::instance()->getFrequency();
}

void Interaction::updateTestScriptList(){
    QProcess process;
    const QString program = QCoreApplication::applicationDirPath() + "/tools/scan_tool";
    process.start(program, {"scripts", "playname"});
    if (!process.waitForStarted(3000)) {
        QTextStream(stdout) << "scan_tool start failed: " << program
                            << " error=" << process.errorString() << "\n";
        _test_script_show_name_list.clear();
        return;
    }
    process.waitForFinished(-1);
    const QString commandOutput = QString::fromUtf8(process.readAllStandardOutput());
    _test_script_show_name_list = commandOutput.split('\n', Qt::SkipEmptyParts);
    for (QString& item : _test_script_show_name_list) {
        item = item.trimmed();
    }
    _test_script_show_name_list.removeAll(QString());
}

void Interaction::updateRefConfigList(){
    QProcess process;
    const QString program = QCoreApplication::applicationDirPath() + "/tools/scan_tool";
    process.start(program, {"ref_configs"});
    if (!process.waitForStarted(3000)) {
        QTextStream(stdout) << "scan_tool start failed: " << program
                            << " error=" << process.errorString() << "\n";
        _ref_config_show_name_list.clear();
        return;
    }
    process.waitForFinished(-1);
    const QString commandOutput = QString::fromUtf8(process.readAllStandardOutput());
    _ref_config_show_name_list = commandOutput.split('\n', Qt::SkipEmptyParts);
    for (QString& item : _ref_config_show_name_list) {
        item = item.trimmed();
    }
    _ref_config_show_name_list.removeAll(QString());
}


/*
 * CInteraction
 */
CInteraction::CInteraction():_draw_signal("need_draw"),_extra_draw_signal("extra_need_draw"){
    runUDPContext();
}
CInteraction::~CInteraction(){
}
void CInteraction::killProcess(const QString& name){
#ifdef WIN32
    auto cmd = "taskkill -im " + name + ".exe -f";
#else
    auto cmd = "pkill " + name;
#endif
    QProcess::execute(cmd);
}
void CInteraction::runUDPContext(){
    if(running) {
        return;
    }
    _asio_context = std::thread([this]{
        running = true;
        zos::__io::GetInstance()->run();
        zos::__io::GetInstance()->reset();
        running = false;
    });
    _asio_context.detach();
}
