#ifndef QSINGLETON_HPP
#define QSINGLETON_HPP
#include <QQmlEngine>
#include <QJSEngine>
template<typename T>
QObject* qinstance(QQmlEngine* engine = nullptr, QJSEngine* scriptEngine = nullptr){
    return new T();
};
#endif // QSINGLETON_HPP
