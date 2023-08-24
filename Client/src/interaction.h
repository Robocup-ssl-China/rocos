#ifndef __INTERACTION_H__
#define __INTERACTION_H__

#include <QObject>
#include "visionmodule.h"
#include "staticparams.h"
#include "field.h"
#include "dealball.h"
#include "communicator.h"
enum type{
     Serial_24L01 = 1,
     UDP_24L01 = 2,
     UDP_WiFi = 3,
};
class Interaction : public QObject {
    Q_OBJECT
  public:
    explicit Interaction(QObject *parent = nullptr);
    Q_INVOKABLE void setIfEdgeTest(bool);
    Q_INVOKABLE void showIfEdgeTest();
    Q_INVOKABLE bool controlMedusa(bool);
    Q_INVOKABLE bool controlMedusa2(bool);
    Q_INVOKABLE bool controlSim(bool,bool);
    Q_INVOKABLE bool controlCrazy(bool);
    Q_INVOKABLE bool controlMonitor(bool);
    Q_INVOKABLE bool connectRadio(bool,int,int);
    Q_INVOKABLE bool connectSim(bool,int,bool);
    Q_INVOKABLE bool changeMedusaSettings(bool, bool);
    Q_INVOKABLE QStringList getInterfaces();
    Q_INVOKABLE QStringList getGrsimInterfaces();
    Q_INVOKABLE void updateInterfaces();
    Q_INVOKABLE void changeVisionInterface(int);
    Q_INVOKABLE void changeGrsimInterface(int);
    Q_INVOKABLE void changeRadioInterface(bool ifBlue,bool ifSender,int);
    Q_INVOKABLE void changeActionWifiCommInterface(int);

    Q_INVOKABLE void setVision(bool, bool);
    Q_INVOKABLE int  getCameraNumber() {
        return PARAM::CAMERA;
    }
    Q_INVOKABLE void controlCamera(int, bool);
    Q_INVOKABLE void controlProcess(int, bool);
    Q_INVOKABLE void kill();
    Q_INVOKABLE int getFPS() {
        return VisionModule::instance()->getFPS();
    }
    Q_INVOKABLE double getBallVelocity() {
        return DealBall::instance()->getBallSpeed();
    }
    Q_INVOKABLE void robotControl(int, int);

    Q_INVOKABLE bool connectSerialPort(bool,int);
    Q_INVOKABLE bool changeSerialFrequency(int,int);
    Q_INVOKABLE bool changeSerialPort(int,int);
    Q_INVOKABLE QStringList getSerialPortsList();
    Q_INVOKABLE int getFrequency(int);
    Q_INVOKABLE QStringList getAllAddress();
    Q_INVOKABLE QString getRealAddress(int index);
    Q_INVOKABLE void changeAddress(int team, int index);
    Q_INVOKABLE void setSerial24L01Car(int index,bool);
    Q_INVOKABLE bool getSerial24L01Car(int index);
    Q_INVOKABLE void setUDP24L01Car(int index,bool);
    Q_INVOKABLE bool getUDP24L01Car(int index);
    Q_INVOKABLE void setUDPWiFiCar(int index,bool);
    Q_INVOKABLE bool getUDPWiFiCar(int index);
    Q_INVOKABLE bool defaultCar(int index);
    Q_INVOKABLE bool checkedChange(int index,int type,bool checked);
    Q_INVOKABLE QStringList getAllCarChoose();
    Q_INVOKABLE void changeUnityCarVariaty(int index);
//    Q_INVOKABLE QString getCurrentPort(int team);
//    Q_INVOKABLE void setCanStart();
    ~Interaction();
public slots:
    void medusaPrint();
};

#endif // __INTERACTION_H__
