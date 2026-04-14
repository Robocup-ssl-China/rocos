#ifndef __TINY_FIELD_H__
#define __TINY_FIELD_H__

#include <QQuickPaintedItem>
#include <mutex>
#include <shared_mutex>
#include <array>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include "layer.h"
#include "visionmodule.h"
#include "fieldconfig.h"

class FieldLineLayer;
class VisionLayer;
class Field : public QQuickPaintedItem{
    Q_OBJECT
    Q_PROPERTY(int type READ type WRITE setType)
    Q_PROPERTY(bool draw READ ifDraw WRITE setDraw)
public:
    Field(QQuickItem *parent = 0);
    ~Field() override;
    void paint(QPainter* painter) override;
    Q_INVOKABLE void resize(int,int,bool update = true);
    static float fieldXFromCoordinate(int);
    static float fieldYFromCoordinate(int);
    static void setSize(int width, int height);
    int type() const { return _type; }
    void setType(int t) { _type = t; }
    bool ifDraw() const { return _draw; }
    void setDraw(bool t) { _draw = t; }
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent * event) override; // canvas scale
#endif
protected:
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;
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
    QPointF _start_screen;
    QPointF _end_screen;
    bool _pressed_robot = false;
    bool _select_robots = false;
    int _selected_count = 0;
    int _selected_team = PARAM::BLUE;
    std::array<int, PARAM::ROBOTNUM> _selected_ids;
    OriginMessage _origin_robot;
    double _display_data = 0;
    int _event_stage = 0; // 0: press, 1: move, 2: release
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
    FieldLineLayer* _layer_field_line = nullptr;
    VisionLayer* _layer_vision = nullptr;
    zos::Subscriber<1> s_need_draw;
    FieldTransform _tf;
private:
    QElapsedTimer _timer;
    bool _first = false;
    int _type = 0;
    bool _draw = true;
private:
    static std::shared_mutex _global_mutex;
    static QTransform _global_tf;
    static int _global_width;
    static int _global_height;
};

#endif // __TINY_FIELD_H__
