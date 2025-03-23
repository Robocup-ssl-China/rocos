#ifndef FIELDLAYERS_H
#define FIELDLAYERS_H

#include <QPainterPath>
#include <QTransform>
#include "layer.h"
#include "field.h"
#include "zos/socketplugin.h"
#include "vision_detection.pb.h"
#include "zss_debug.pb.h"
class FieldLineLayer : public Layer{
public:
    FieldLineLayer(FieldTransform*);
    ~FieldLineLayer(){}
    void drawPath();
    bool draw() override;
private:
    FieldParam* _fp = FP::_();
};

class VisionLayer: public Layer{
public:
    VisionLayer(FieldTransform* _tf):
        Layer("Vision",_tf),
        s_draw_data("vision_draw_signal",[this](const zos::Data& data){receiveData(data);}){}
    ~VisionLayer(){}
    bool draw() override;
    zos::Subscriber<1> s_draw_data;
private:
//    void drawOriginVision(const Vision_DetectionFrame&);
    void drawFusionVision(const Vision_DetectionFrame& frame);
private:
    void paintRobot(const QColor& color,quint8 num,qreal x,qreal y,qreal radian
                      ,bool ifDrawNum = true,const QColor& textColor = Qt::white,bool needCircle = false);
    void paintBall(const QColor& color,qreal x,qreal y,qreal r);
    void paintFocus(const QColor& color, qreal x, qreal y, qreal radian, qreal count);
private:
    void receiveData(const zos::Data& data);

    Vision_DetectionFrame _msg;
    std::shared_mutex _dataMutex;
};
class DebugLayer: public Layer{
public:
    DebugLayer(FieldTransform*,int);
    ~DebugLayer(){}
    bool draw() override;
private:
    void receiveData(const void*,size_t);
    void paintDebug(const ZSS::Protocol::Debug_Msgs&);
    ZSS::Protocol::Debug_Msgs _msg;
    zos::udp::Plugin<ZSS::Protocol::Debug_Msgs> _receiver;
    std::shared_mutex _dataMutex;
};
class MouseLayer: public Layer{
    MouseLayer(FieldTransform*);
    ~MouseLayer(){}
    bool draw() override;
private:
};

#endif // FIELDLAYERS_H
