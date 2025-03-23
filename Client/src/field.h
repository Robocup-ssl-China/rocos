#ifndef __TINY_FIELD_H__
#define __TINY_FIELD_H__

#include <QQuickPaintedItem>
#include <mutex>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include "layer.h"
#include "visionmodule.h"
#include "fieldconfig.h"
class Field : public QQuickPaintedItem{
    Q_OBJECT
public:
    Field(QQuickItem *parent = 0);
    void paint(QPainter* painter) override;
    Q_INVOKABLE void resize(int,int,bool update = true);
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent * event) override; // canvas scale
#endif
private:
    // event + left/right/middle + [none/ctrl/shift/alt] + [press/move/release]
    void e_left(); // ball || robot set position
    void e_right(); // robot dir
    void e_right_r(); // ball vel
    void e_middle(); // canvas translation
    void e_middle_none(); // move
    void e_middle_alt(); // set BP Point
    void e_middle_ctrl(); // set vision area
private:
    VisionModule::CheckRobotResult _check_press_robot;
    QPointF _start;
    QPointF _end;
    Qt::MouseButtons _buttons;
    Qt::KeyboardModifiers _mouse_modifiers;
    QTransform _old_tf;
signals:
    void needDraw();
public:
public slots:
    void updateArea();
public:
    void sendSignal();
    void draw();
private:
    QPixmap *_pm = nullptr;
    QPainter _painter;
    mutable std::shared_mutex _mutex;
    QRect _area;
    LayerNode _root;
    zos::Subscriber<1> s_need_draw;
    FieldTransform _tf;
private:
    QElapsedTimer _timer;
    bool _first = false;
};

#endif // __TINY_FIELD_H__
