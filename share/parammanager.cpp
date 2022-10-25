#include "parammanager.h"

namespace ZSS {
ParamManager::ParamManager():settings(nullptr){
    settings = new QSettings("./config.ini",QSettings::IniFormat);
}
ParamManager::ParamManager(const QString& name):settings(nullptr){
    settings = new QSettings(name,QSettings::IniFormat);
}
ParamManager::~ParamManager(){
    if(settings)
        delete settings;
}
bool ParamManager::loadParam(QChar& value, const QString& key, QChar defaultValue){
    value = settings->value(key, defaultValue).toChar();
    if (!settings->contains(key)){
        settings->setValue(key, defaultValue);
        settings->sync();
        return false;
    }
    return true;
}
bool ParamManager::loadParam(int& value, const QString& key, int defaultValue){
    value = settings->value(key, defaultValue).toInt();
    if (!settings->contains(key)){
        settings->setValue(key, defaultValue);
        settings->sync();
        return false;
    }
    return true;
}
bool ParamManager::loadParam(double& value, const QString& key, double defaultValue){
    value = settings->value(key, defaultValue).toDouble();
    if (!settings->contains(key)){
        settings->setValue(key, defaultValue);
        settings->sync();
        return false;
    }
    return true;
}
bool ParamManager::loadParam(QString& value, const QString& key , QString defaultValue){
    value = settings->value(key, defaultValue).toString();
    if (!settings->contains(key)){
        settings->setValue(key, defaultValue);
        settings->sync();
        return false;
    }
    return true;
}
bool ParamManager::loadParam(bool& value, const QString& key, bool defaultValue){
    value = settings->value(key, defaultValue).toBool();
    if (!settings->contains(key)){
        settings->setValue(key, defaultValue);
        settings->sync();
        return false;
    }
    return true;
}
bool ParamManager::changeParam(const QString & key, const QVariant & value){
    settings->setValue(key,value);
    settings->sync();
    return true;
}
bool ParamManager::changeParam(const QString & group,const QString & key,const QVariant & value){
    settings->beginGroup(group);
    settings->setValue(key,value);
    settings->endGroup();
    settings->sync();
    return true;
}
QStringList ParamManager::allKeys(){
    return settings->allKeys();
}
QStringList ParamManager::allKeys(const QString& group){
    settings->beginGroup(group);
    auto&& res = settings->allKeys();
    settings->endGroup();
    return res;
}
QStringList ParamManager::allGroups(){
    return settings->childGroups();
}
QVariant ParamManager::value(const QString& key,const QVariant& defaultValue){
    if (!settings->contains(key)){
        settings->setValue(key, defaultValue);
    }
    return settings->value(key,defaultValue);
}
QVariant ParamManager::value(const QString& group,const QString& key,const QVariant& defaultValue){
    if (!settings->contains(group+'/'+key)){
        settings->setValue(group+'/'+key, defaultValue);
    }
    return settings->value(group+'/'+key,defaultValue);
}
void ParamManager::sync(){
    settings->sync();
}
void ParamManager::clear() {
    settings->clear();
    settings->sync();
}
}
