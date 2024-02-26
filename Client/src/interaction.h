#ifndef __INTERACTION_H__
#define __INTERACTION_H__

#include <QObject>
#include "visionmodule.h"
#include "staticparams.h"
#include "field.h"
#include "dealball.h"
#include "communicator.h"
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
    Q_INVOKABLE bool changeTestSettings(bool, bool, int);
    Q_INVOKABLE bool changeRefConfigSettings(bool, bool, int);
    Q_INVOKABLE bool getTestSettings(bool);
    Q_INVOKABLE bool getRefConfigSetting(bool);
    Q_INVOKABLE int getTestScriptIndex(bool);
    Q_INVOKABLE int getRefConfigIndex(bool);
    Q_INVOKABLE QStringList getInterfaces();
    Q_INVOKABLE QStringList getGrsimInterfaces();
    Q_INVOKABLE void updateInterfaces();
    Q_INVOKABLE void changeVisionInterface(int);
    Q_INVOKABLE void changeGrsimInterface(int);
    Q_INVOKABLE void changeRadioInterface(bool ifBlue,bool ifSender,int);

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

    Q_INVOKABLE bool connectSerialPort(bool);
    Q_INVOKABLE bool changeSerialFrequency(int);
    Q_INVOKABLE bool changeSerialPort(int);
    Q_INVOKABLE QStringList getSerialPortsList();
    Q_INVOKABLE int getFrequency();
    Q_INVOKABLE QStringList getAllAddress();
    Q_INVOKABLE QString getRealAddress(int index);
    Q_INVOKABLE void changeAddress(int team, int index);
    Q_INVOKABLE void updateTestScriptList();
    Q_INVOKABLE void updateRefConfigList();
    Q_INVOKABLE QStringList getTestScriptList() {
        return _test_script_show_name_list;
    }
    Q_INVOKABLE QStringList getRefConfigList() {
        return _ref_config_show_name_list;
    }
    ~Interaction();
public slots:
    void medusaPrint();
private:
    // QStringList _test_script_real_name_list;
    QStringList _test_script_show_name_list;
    // QStringList _ref_config_real_name_list;
    QStringList _ref_config_show_name_list;
};

#endif // __INTERACTION_H__
