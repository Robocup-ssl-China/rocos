#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H
#include <QObject>
#include "geometry.h"
#include <singleton.hpp>
class CGlobalSettings : public QObject{
    Q_OBJECT
public:
    CGlobalSettings(QObject *parent = 0);
    float minimumX,maximumX,minimumY,maximumY;
    float selectCarMinX,selectCarMaxX,selectCarMinY,selectCarMaxY;
    float ballPlacementX,ballPlacementY;
    bool inChosenArea(float x, float y);
    bool inChosenArea(CGeoPoint);
    void setArea(float,float,float,float);
    void resetArea();
    void setBallPlacementPos(float,float);
    void setSelectCarArea(float,float,float,float);
    void resetSelectCarArea();
signals:
    void needRepaint();
    void addOutput(const QString&);
    void clearOutput();
};
typedef Singleton<CGlobalSettings> GlobalSettings;
#endif // GLOBALSETTINGS_H
