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
    ZNetworkInterfaces::instance()->updateInterfaces();
}
QStringList Interaction::getInterfaces(){
    return ZNetworkInterfaces::instance()->getInterfaces();
}
QStringList Interaction::getGrsimInterfaces(){
    return ZNetworkInterfaces::instance()->getGrsimInterfaces();
}
void Interaction::changeVisionInterface(int index){
//    if(portNum < ports.size() && portNum >= 0){
//        serial.setPortName(ports[portNum]);
//        return true;
//    }
    VisionModule::instance()->setInterfaceIndex(index);
//    qDebug() << "vision interface : " << index;
}
void Interaction::changeGrsimInterface(int index){
    ZCommunicator::instance()->setGrsimInterfaceIndex(index);
}
void Interaction::changeRadioInterface(bool ifBlue,bool ifSender,int index){
//    qDebug() << "radio  interface : " << ifBlue << ifSender << index;
}
void Interaction::changeActionWifiCommInterface(int index){
    ZSS::ZActionModule::instance()->resetWifiCommInterface(index);
}
void Interaction::setIfEdgeTest(bool ifEdgeTest) {
    VisionModule::instance()->setIfEdgeTest(ifEdgeTest);
}
void Interaction::showIfEdgeTest() {
    std::cout << VisionModule::instance()->showIfEdgeTest() << std::endl;
}
void Interaction::setVision(bool needStart, bool real) {
    ZSS::ZActionModule::instance()->setSimulation(!real);
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
void Interaction::controlProcess(int index, bool state) {
    GlobalData::instance()->processControl[index] = state;
}
bool Interaction::connectRadio(bool sw, int id, int frq) {
    if(sw) {
        auto res = ZCommunicator::instance()->disconnectMedusa(id);
        res = res && ZSS::ZActionModule::instance()->disconnectRadio(id);
        res = res && ZCommunicator::instance()->connectMedusa(id);
        return res && ZSS::ZActionModule::instance()->connectRadio(id, frq);
    } else {
//        return ZSS::ZActionModule::instance()->disconnectRadio(id);
    }
}
bool Interaction::connectSim(bool sw, int id, bool color) {
    if(sw) {
        ZCommunicator::instance()->disconnectMedusa(id);
        ZCommunicator::instance()->connectMedusa(id);
        ZSS::ZSimModule::instance()->disconnectSim(color);
        return ZSS::ZSimModule::instance()->connectSim(color);
    } else {
//        return ZSS::ZSimModule::instance()->disconnectSim(color); //fix a bug for Medusa out of Athena
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
void Interaction::medusaPrint() {
    emit GlobalSettings::instance()->addOutput(medusaProcess->readAllStandardOutput());
}
void Interaction::robotControl(int id, int team) {
    Simulator::instance()->controlRobot(id, team == 1);
}

void Interaction::kill() {
    ZRecRecorder::instance()->stop();
    QProcess killTask;
#ifdef WIN32
//    RefereeThread::instance()->disconnectTCP();
    QString athena = "taskkill -im Client.exe -f";
    QString medusa = "taskkill -im Core.exe -f";
    //QString grSim = "taskkill -im grSim.exe -f";
#else
    QString athena = "pkill Client";
    QString medusa = "pkill Core";
    //QString grSim = "pkill grsim";
#endif
    if (monitorProcess != nullptr) {
        if (monitorProcess->isOpen()) {
            monitorProcess->close();
        }
        delete monitorProcess;
        monitorProcess = nullptr;
    }
    killTask.execute(medusa);
    //killTask.execute(grSim);
    killTask.execute(athena);
    killTask.close();
}
bool Interaction::connectSerialPort(bool sw,int team){
    if(sw){
        return ZSS::NActionModule::instance()->init(team);
    }
    return ZSS::NActionModule::instance()->closeSerialPort(team);
}

bool Interaction::changeSerialFrequency(int frequency,int team){
    return ZSS::NActionModule::instance()->changeFrequency(frequency,team);
}

bool Interaction::changeSerialPort(int index,int team){
    return ZSS::NActionModule::instance()->changePorts(index,team);
}

QStringList Interaction::getSerialPortsList(){
    return ZSS::NActionModule::instance()->updatePortsList();
}
int Interaction::getFrequency(int team){
    return ZSS::NActionModule::instance()->getFrequency(team);
}
void Interaction::changeAddress(int team, int index){
    ZSS::ZActionModule::instance()->changeAddress(team,index);
}
QStringList Interaction::getAllAddress(){
    return ZSS::ZActionModule::instance()->getAllAddress();
}
QString Interaction::getRealAddress(int index){
    return ZSS::ZActionModule::instance()->getRealAddress(index);
};
void Interaction::setSerial24L01Car(int index, bool checked){
    ZSS::ZParamManager::instance()->changeParam(QString("Serial2401Car/%1_car").arg(index),checked);
    if(checked){
        //qDebug() << "viperDebug :" << "serial_car :" << index <<   "connect";
        ZSS::ZActionModule::instance()->setcar_varity(index,ZSS::CommType::SERIAL_24L01);
        ZSS::ZParamManager::instance()->changeParam(QString("UDP2401Car/%1_car").arg(index),false);
        ZSS::ZParamManager::instance()->changeParam(QString("UDPWiFiCar/%1_car").arg(index),false);
    }
    else if (ZSS::ZActionModule::instance()->getcar_varity(index) == ZSS::CommType::SERIAL_24L01){
        ZSS::ZActionModule::instance()->setcar_varity(index,ZSS::CommType::DEFAULT);
        //qDebug() << "viperDebug :" << "serial_car :" << index <<  "disconnect";
    }
}
bool Interaction::getSerial24L01Car(int index){
    bool res;
    ZSS::ZParamManager::instance()->loadParam(res,QString("Serial2401Car/%1_car").arg(index));
    return res;
}
void Interaction::setUDP24L01Car(int index, bool checked){
    ZSS::ZParamManager::instance()->changeParam(QString("UDP2401Car/%1_car").arg(index),checked);
    if(checked){
        //qDebug() << "viperDebug :" << "UDP2401_car :" << index <<   "connect";
        ZSS::ZActionModule::instance()->setcar_varity(index,ZSS::CommType::UDP_24L01);
        ZSS::ZParamManager::instance()->changeParam(QString("Serial2401Car/%1_car").arg(index),false);
        ZSS::ZParamManager::instance()->changeParam(QString("UDPWiFiCar/%1_car").arg(index),false);
    }
    else if (ZSS::ZActionModule::instance()->getcar_varity(index) == ZSS::CommType::UDP_24L01){
        ZSS::ZActionModule::instance()->setcar_varity(index,ZSS::CommType::DEFAULT);
        //qDebug() << "viperDebug :" << "UDP2401_car :" << index <<  "disconnect";
    }
}
bool Interaction::getUDP24L01Car(int index){
    bool res;
    ZSS::ZParamManager::instance()->loadParam(res,QString("UDP2401Car/%1_car").arg(index));
    return res;
}
void Interaction::setUDPWiFiCar(int index, bool checked){
    ZSS::ZParamManager::instance()->changeParam(QString("UDPWiFiCar/%1_car").arg(index),checked);
    if(checked){
        //qDebug() << "viperDebug :" << "UDPWiFiCar :" << index <<   "connect";
        ZSS::ZActionModule::instance()->setcar_varity(index,ZSS::CommType::UDP_WIFI);
        ZSS::ZParamManager::instance()->changeParam(QString("UDP2401Car/%1_car").arg(index),false);
        ZSS::ZParamManager::instance()->changeParam(QString("Serial2401Car/%1_car").arg(index),false);
    }
    else if (ZSS::ZActionModule::instance()->getcar_varity(index) == ZSS::CommType::UDP_WIFI){
        //qDebug() << "viperDebug :" << "UDPWiFiCar :" << index <<  "disconnect";
        ZSS::ZActionModule::instance()->setcar_varity(index,ZSS::CommType::DEFAULT);
    }
}
bool Interaction::getUDPWiFiCar(int index){
    bool res;
    ZSS::ZParamManager::instance()->loadParam(res,QString("UDPWiFiCar/%1_car").arg(index));
    return res;
}
bool Interaction::defaultCar(int index){
    return (getUDP24L01Car(index) || getUDPWiFiCar(index)) ? false : true;
}
bool Interaction::checkedChange(int index,int type,bool checked){
    if(type == ZSS::CommType::SERIAL_24L01){
            bool res1 = getUDPWiFiCar(index),res2 = getUDP24L01Car(index),res3 = checked;
            if(!res1 && !res2 && checked==false) res3 = true;
            return res3;
    }else if(type == ZSS::CommType::UDP_24L01){
            bool res1 = getSerial24L01Car(index),res2 = getUDPWiFiCar(index),res3 = checked;
            if(!res1 && !res2 && checked==false) res3 = true;
            return res3;
    }else if(type == ZSS::CommType::UDP_WIFI){
            bool res1 = getSerial24L01Car(index),res2 = getUDP24L01Car(index),res3 = checked;
            if(!res1 && !res2 && checked==false) res3 = true;
            return res3;
    }
}
QStringList Interaction::getAllCarChoose(){
    return ZSS::ZActionModule::instance()->getAllCarChoose();
}
void Interaction::changeUnityCarVariaty(int index){
    if(index == ZSS::CommType::SERIAL_24L01){
        for(int i = 0;i < PARAM::ROBOTNUM; i++){
            setSerial24L01Car(i,true);
        }
    }else if(index == ZSS::CommType::UDP_24L01){
        for(int i = 0;i < PARAM::ROBOTNUM; i++){
            setUDP24L01Car(i,true);
        }
    }else if(index == ZSS::CommType::UDP_WIFI){
        for(int i = 0;i < PARAM::ROBOTNUM; i++){
            setUDPWiFiCar(i,true);
        }
    }
}
//QString Interaction::getCurrentPort(int team){
//    return ZSS::NActionModule::instance()->getCurrentPort();
//}
//void Interaction::setCanStart(){
//    ZSS::NActionModule::instance()->setcanstart(true);
//}

