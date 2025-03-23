#include "field.h"
#include <QtGlobal>
#include <QtDebug>
#include <QtMath>
#include "simulator.h"
#include "fieldlayers.h"
#include "interaction.h"
#include "parammanager.h"
#include "visionmodule.h"
#include "themecolor.h"

Field::Field(QQuickItem *parent)
    : QQuickPaintedItem(parent),
      s_need_draw("field_draw_signal",[this](const zos::Data&){this->draw();}){
    _timer.start();
    int canvasWidth,canvasHeight;
    ZSS::ZParamManager::_()->loadParam(canvasWidth,"GUI/canvasWidth",300);
    ZSS::ZParamManager::_()->loadParam(canvasHeight,"GUI/canvasHeight",200);
    setImplicitWidth(canvasWidth);
    setImplicitHeight(canvasHeight);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MiddleButton);
    auto* fieldLineLayer = new FieldLineLayer(&_tf);
    auto* visionLayer = new VisionLayer(&_tf);
    auto* debugBlueLayer = new DebugLayer(&_tf,PARAM::BLUE);
    auto* debugYellowLayer = new DebugLayer(&_tf,PARAM::YELLOW);

    CInteraction::_()->_draw_signal.link(&fieldLineLayer->s_need_draw);
    CInteraction::_()->_draw_signal.link(&visionLayer->s_need_draw);
    CInteraction::_()->_draw_signal.link(&debugBlueLayer->s_need_draw);
    CInteraction::_()->_draw_signal.link(&debugYellowLayer->s_need_draw);

    VisionModule::instance()->p_draw_signal.link(&visionLayer->s_draw_data);

    _root.addChild(fieldLineLayer);
    _root.addChild(visionLayer);
    _root.addChild(debugBlueLayer);
    _root.addChild(debugYellowLayer);

    _root.p_draw_done.link(&this->s_need_draw);
    CInteraction::_()->_extra_draw_signal.link(&this->s_need_draw);

    connect(this,SIGNAL(needDraw()),this, SLOT(updateArea()));
}
void Field::resize(int width,int height,bool update){
    if (width <= 0 || height <= 0) return;
    if (width == _area.width() && height == _area.height()) return;
    if (update){
        ZSS::ZParamManager::_()->changeParam("GUI/canvasWidth",width);
        ZSS::ZParamManager::_()->changeParam("GUI/canvasHeight",height);
    }
    {
        std::scoped_lock<std::shared_mutex> lock(_tf.mutex);
        _tf.update(width,height);
    }
    {
        std::scoped_lock lock(_mutex);
        if(_painter.isActive())
            _painter.end();
        delete _pm;
        _pm = new QPixmap(QSize(width, height));
        _pm->fill(Color::_()->BACKGROUND);
        _area = QRect(0, 0, width, height);
        _painter.begin(_pm);
        _painter.setRenderHint(QPainter::Antialiasing, true);
        _painter.setRenderHint(QPainter::TextAntialiasing, true);
    }
    _root.resize(width,height);
    sendSignal();
}

void Field::paint(QPainter* painter) {
    if(_mutex.try_lock()){
        if(_pm != nullptr){
            painter->drawPixmap(_area, *_pm);
        }
        _mutex.unlock();
    }
}
void Field::updateArea(){
    this->update(_area);
}
void Field::draw(){
    if(_timer.elapsed()>1000.0/PARAM::GUI_DRAW_RATE || !_first){
        _timer.restart();
        _first = true;
        std::scoped_lock lock(_mutex);
        _pm->fill(Color::_()->BACKGROUND);
        {
            std::scoped_lock lock(_root._mutex);
            _painter.drawImage(0,0,*(_root.get()));
        }
        emit needDraw();
    }
}
void Field::mousePressEvent(QMouseEvent *e) {
    _check_press_robot.res = false;
    {
        std::shared_lock lock(_tf.mutex);
        _start = _tf._.inverted().map(e->pos());
        _old_tf = _tf._;
    }
    _end = _start;
    _buttons = e->buttons();
    _mouse_modifiers = e->modifiers();

    _check_press_robot = VisionModule::_()->checkRobot(_start.x(),_start.y());
    qDebug() << "press event : " << _check_press_robot.res << _check_press_robot.team << _check_press_robot.id;
    mouseMoveEvent(e);
}
void Field::mouseMoveEvent(QMouseEvent *e) {
    {
        std::shared_lock lock(_tf.mutex);
        _end = _tf._.inverted().map(e->pos());
    }
    switch(e->buttons()) {
    case Qt::LeftButton:
        e_left();
        break;
    case Qt::RightButton:
        e_right();
        break;
    case Qt::MiddleButton:
        e_middle();
        break;
    default:
        break;
    }
    sendSignal();
}
void Field::mouseReleaseEvent(QMouseEvent *e) {
    {
        std::shared_lock lock(_tf.mutex);
        _end = _tf._.inverted().map(e->pos());
    }
    mouseMoveEvent(e);
    if(!_check_press_robot.res && _buttons==Qt::RightButton){
        e_right_r();
    }
    sendSignal();
}

void Field::e_left() {
    switch(_mouse_modifiers) {
    case Qt::NoModifier :
        if(_check_press_robot.res) {
            Simulator::instance()->setRobot(_end.x()/1000.0,_end.y()/1000.0,_check_press_robot.id,_check_press_robot.team,_check_press_robot.orientation/M_PI*180);
        } else {
            Simulator::instance()->setBall(_end.x()/1000.0,_end.y()/1000.0);
        }
        break;
    default:
        break;
    }
}
void Field::e_right() {
    auto&& _r = _check_press_robot;
    QLineF line(_start, _end);
    if(_check_press_robot.res) {
        Simulator::instance()->setRobot(_r.x/1000.0,_r.y/1000.0,_r.id,_r.team,-line.angle());
    } else {
        Simulator::instance()->setBall(_start.x()/1000.0,_start.y()/1000.0);
    }
}
void Field::e_right_r() {
    auto&& _r = _check_press_robot;
    QLineF line(_start, _end);
    if(_check_press_robot.res) {
        Simulator::instance()->setRobot(_r.x/1000.0,_r.y/1000.0,_r.id,_r.team,-line.angle());
    } else {
        Simulator::instance()->setBall(_start.x()/1000.0,_start.y()/1000.0,line.dx()/1000.0,line.dy()/1000.0);
    }
}
void Field::e_middle() {
    switch(_mouse_modifiers) {
    case Qt::NoModifier:
        e_middle_none();
        break;
    case Qt::AltModifier:
        e_middle_alt();
        break;
    case Qt::ControlModifier:
        e_middle_ctrl();
        break;
    default:
        break;
    }
}
void Field::e_middle_none() {
    auto d = _end-_start;
    {
        std::scoped_lock<std::shared_mutex> lock(_tf.mutex);
        _tf._ = _old_tf.translate(d.x(),d.y());
    }
    _tf.limit(_area.width(),_area.height());
}
void Field::e_middle_alt() {
//    GlobalSettings::instance()->setBallPlacementPos(::rx(e->x()), ::ry(e->y()));
}
void Field::e_middle_ctrl() {
//    auto x1 = ::rx(e->x());
//    auto x2 = ::rx(MiddleEvent::start.x());
//    auto y1 = ::ry(e->y());
//    auto y2 = ::ry(MiddleEvent::start.y());
//    auto minX = std::min(x1, x2);
//    auto maxX = std::max(x1, x2);
//    auto minY = std::min(y1, y2);
//    auto maxY = std::max(y1, y2);
//    if(maxX - minX < MIN_LENGTH * zoomRatio || maxY - minY < MIN_LENGTH * zoomRatio)
//        GlobalSettings::instance()->resetArea();
//    else
//        GlobalSettings::instance()->setArea(minX, maxX, minY, maxY);
}
void Field::wheelEvent (QWheelEvent *e) {
    QPointF p;
    {
        std::shared_lock lock(_tf.mutex);
        #if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        p = _tf._.inverted().map(e->position());
        #else
        p = _tf._.inverted().map(e->pos());
        #endif
    }
    auto ratio = e->angleDelta().y() < 0 ? 0.9 : 1/0.9;
    {
        std::scoped_lock<std::shared_mutex> lock(_tf.mutex);
        _tf._.translate(p.x(),p.y());
        _tf._.scale(ratio,ratio);
        _tf._.translate(-p.x(),-p.y());
    }
    _tf.limit(_area.width(),_area.height());
    sendSignal();
}
void Field::sendSignal(){
    CInteraction::_()->_draw_signal.publish();
}
