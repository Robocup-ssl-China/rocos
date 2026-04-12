#include "field.h"

#include "fieldlayers.h"
#include "globaldata.h"
#include "globalsettings.h"
#include "parammanager.h"
#include "simulator.h"
#include "themecolor.h"
#include "zss_debug.pb.h"

#include <QFont>
#include <QtMath>

#include <algorithm>
#include <cmath>

namespace {
constexpr float kMinSelectionLength = 500.0F; // mm
constexpr qreal kZoomStep = 0.05;
constexpr qreal kZoomMin = 0.1;
constexpr int kBallSpeedRatio = 3;

using ZSS::Protocol::Debug_Msgs;

QTransform currentTransform(FieldTransform& tf) {
    std::shared_lock<std::shared_mutex> lock(tf.mutex);
    return tf._;
}

QPointF mapFieldPoint(const QTransform& tf, const QPointF& fieldPoint) {
    return tf.map(fieldPoint);
}

QRectF mapFieldRect(const QTransform& tf, double minX, double maxX, double minY, double maxY) {
    const QPointF p1 = tf.map(QPointF(minX, minY));
    const QPointF p2 = tf.map(QPointF(maxX, maxY));
    return QRectF(p1, p2).normalized();
}

QPointF toFieldPoint(FieldTransform& tf, const QPointF& screenPoint) {
    std::shared_lock<std::shared_mutex> lock(tf.mutex);
    bool ok = false;
    const QTransform inverse = tf._.inverted(&ok);
    if (!ok) {
        return QPointF(0.0, 0.0);
    }
    return inverse.map(screenPoint);
}

template <typename T>
T clampValue(T value, T minValue, T maxValue) {
    return std::max(minValue, std::min(value, maxValue));
}

const Msg::Robot* findRobotById(const OriginMessage& vision, int team, int id) {
    if (team < PARAM::BLUE || team > PARAM::YELLOW || id < 0 || id >= PARAM::ROBOTMAXID) {
        return nullptr;
    }
    const int idx = vision.robotIndex[team][id];
    if (idx < 0 || idx >= vision.robotSize[team]) {
        return nullptr;
    }
    return &vision.robot[team][idx];
}

void paintRobotShape(QPainter& painter,
                     const QTransform& tf,
                     const QColor& color,
                     quint8 id,
                     qreal x,
                     qreal y,
                     qreal radian,
                     bool drawId,
                     const QColor& textColor,
                     bool selectedCircle) {
    const qreal carDiameter = FP::_()->s_carDiameter;
    const qreal carFaceWidth = FP::_()->s_carFaceWidth;
    const qreal radius = carDiameter / 2.0;
    const qreal chordAngle = qRadiansToDegrees(qAcos(carFaceWidth / carDiameter));
    const qreal scale = std::abs(tf.m11());

    painter.setBrush(QBrush(color));
    painter.setPen(QPen(Color::_()->ROBOT_DIR, std::max(1.0, scale * 0.3), Qt::DotLine));

    const QPointF center = tf.map(QPointF(x, y));
    const QPointF dir = tf.map(QPointF(x, y) + QPointF(140 * qSin(-radian + M_PI_2), 140 * qCos(-radian + M_PI_2)));
    painter.drawLine(center, dir);

    painter.setPen(Qt::NoPen);
    painter.drawChord(tf.mapRect(QRectF(x - radius, y - radius, 2 * radius, 2 * radius)),
                     int((-90.0 + chordAngle + 180.0 / M_PI * radian) * 16),
                     int((-180.0 - 2 * chordAngle) * 16));

    if (selectedCircle) {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(QColor(27, 129, 62), std::max(1.0, scale * 0.5)));
        painter.drawChord(tf.mapRect(QRectF(x - radius, y - radius, 2 * radius, 2 * radius)),
                         int((-90.0 + chordAngle + 180.0 / M_PI * radian) * 16),
                         int((-180.0 - 2 * chordAngle) * 16));
    }

    if (drawId) {
        QFont font;
        font.setPixelSize(std::max(10, int(scale * 0.9)));
        painter.setFont(font);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(textColor, std::max(1.0, scale * 0.4)));
        painter.drawText(tf.map(QPointF(x - FP::_()->s_number * 0.8, y + FP::_()->s_carDiameter * 0.35)),
                        QString::number(id, 16).toUpper());
    }
}

void paintBallShape(QPainter& painter, const QTransform& tf, const QColor& color, qreal x, qreal y, qreal diameter) {
    painter.setBrush(QBrush(color));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(tf.mapRect(QRectF(x - diameter / 2.0, y - diameter / 2.0, diameter, diameter)));
}

void paintOriginVision(QPainter& painter, const QTransform& tf) {
    for (int cam = 0; cam < PARAM::CAMERA; ++cam) {
        if (!GlobalData::instance()->cameraControl[cam]) {
            continue;
        }
        const OriginMessage& msg = GlobalData::instance()->camera[cam][0];

        for (int team = PARAM::BLUE; team <= PARAM::YELLOW; ++team) {
            for (int i = 0; i < msg.robotSize[team]; ++i) {
                const auto& robot = msg.robot[team][i];
                paintRobotShape(painter,
                                tf,
                                Color::_()->ROBOT[team],
                                robot.id,
                                robot.pos.x(),
                                robot.pos.y(),
                                robot.angle,
                                true,
                                Qt::white,
                                false);
            }
        }

        for (int i = 0; i < msg.ballSize; ++i) {
            const auto& ball = msg.ball[i];
            paintBallShape(painter, tf, Color::_()->BALL, ball.pos.x(), ball.pos.y(), FP::_()->s_ballDiameter);
        }
    }
}

void paintDebugMessages(QPainter& painter, const QTransform& tf, int team) {
    Debug_Msgs msgs;
    {
        QMutexLocker lock(&GlobalData::instance()->debugMutex);
        if (team == PARAM::BLUE) {
            msgs.ParseFromArray(GlobalData::instance()->debugBlueMessages.data(),
                                GlobalData::instance()->debugBlueMessages.size());
        } else {
            msgs.ParseFromArray(GlobalData::instance()->debugYellowMessages.data(),
                                GlobalData::instance()->debugYellowMessages.size());
        }
    }

    painter.setBrush(QBrush(Color::_()->DEBUG_BRUSH_COLOR));
    QFont font("Ubuntu Mono", std::max(10, int(std::abs(tf.m11()) * 0.8)), QFont::Normal);
    painter.setFont(font);

    for (int i = 0; i < msgs.msgs_size(); ++i) {
        const auto& msg = msgs.msgs(i);
        const int colorIdx = clampValue(int(msg.color()), 0, 9);
        painter.setPen(QPen(Color::_()->DEBUG_MSG[colorIdx], std::max(1.0, std::abs(tf.m11()) * 0.12)));

        switch (msg.type()) {
        case ZSS::Protocol::Debug_Msg_Debug_Type_ARC: {
            const double x1 = msg.arc().rect().point1().x();
            const double y1 = msg.arc().rect().point1().y();
            const double x2 = msg.arc().rect().point2().x();
            const double y2 = msg.arc().rect().point2().y();
            const QRectF rect = tf.mapRect(QRectF(std::min(x1, x2), std::min(y1, y2), std::abs(x2 - x1), std::abs(y2 - y1)));
            painter.drawArc(rect, int(msg.arc().start() * -16), int(msg.arc().span() * -16));
            break;
        }
        case ZSS::Protocol::Debug_Msg_Debug_Type_LINE:
            painter.drawLine(tf.map(QPointF(msg.line().start().x(), msg.line().start().y())),
                             tf.map(QPointF(msg.line().end().x(), msg.line().end().y())));
            break;
        case ZSS::Protocol::Debug_Msg_Debug_Type_POINTS: {
            QVector<QLineF> lines;
            const double d = FP::_()->s_debugPoint;
            for (int k = 0; k < msg.points().point_size(); ++k) {
                const auto& p = msg.points().point(k);
                lines.push_back(tf.map(QLineF(p.x() + d, p.y() + d, p.x() - d, p.y() - d)));
                lines.push_back(tf.map(QLineF(p.x() - d, p.y() + d, p.x() + d, p.y() - d)));
            }
            painter.drawLines(lines);
            break;
        }
        case ZSS::Protocol::Debug_Msg_Debug_Type_TEXT:
            font.setPointSizeF(std::max(8.0, std::abs(tf.m11()) * msg.text().size() * 0.01));
            font.setWeight(static_cast<QFont::Weight>(msg.text().weight()));
            painter.setFont(font);
            painter.drawText(tf.map(QPointF(msg.text().pos().x(), msg.text().pos().y())),
                             QString::fromStdString(msg.text().text()));
            break;
        default:
            break;
        }
    }
}
} // namespace

std::shared_mutex Field::_global_mutex;
QTransform Field::_global_tf;
int Field::_global_width = 0;
int Field::_global_height = 0;

Field::Field(QQuickItem* parent)
    : QQuickPaintedItem(parent)
    , _root("FieldRoot")
    , s_need_draw("field_draw_trigger", [this](const zos::Data&) { sendSignal(); }) {
    int defaultHeight = 960;
    int defaultWidth = 1280;
    ZSS::ZParamManager::_()->loadParam(defaultHeight, "canvas/height", 960);
    ZSS::ZParamManager::_()->loadParam(defaultWidth, "canvas/width", 1280);

    setFillColor(Color::_()->BACKGROUND);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MiddleButton);

    _selected_ids.fill(-1);
    _buttons = Qt::NoButton;
    _mouse_modifiers = Qt::NoModifier;

    _layer_field_line = new FieldLineLayer(&_tf);
    _layer_vision = new VisionLayer(&_tf);
    _root.addChild(_layer_field_line);
    _root.addChild(_layer_vision);

    VisionModule::instance()->p_draw_signal.link(&_layer_vision->s_draw_data);
    VisionModule::instance()->p_draw_signal.link(&s_need_draw);

    connect(this, &Field::needDraw, this, &Field::draw, Qt::QueuedConnection);
    connect(GlobalSettings::instance(), &CGlobalSettings::needRepaint, this, &Field::draw);

    // setImplicitWidth/Height may trigger geometryChange -> resize.
    // Keep these calls after layer initialization to avoid null dereference.
    setImplicitWidth(defaultWidth);
    setImplicitHeight(defaultHeight);

    GlobalSettings::instance()->resetArea();
    resize(defaultWidth, defaultHeight, false);
}

Field::~Field() {
    std::scoped_lock<std::shared_mutex> lock(_mutex);
    if (_painter.isActive()) {
        _painter.end();
    }
    delete _pm;
    _pm = nullptr;
}

void Field::paint(QPainter* painter) {
    std::shared_lock<std::shared_mutex> lock(_mutex);
    if (_pm == nullptr) {
        return;
    }
    painter->drawPixmap(_area, *_pm);
}

void Field::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) {
    QQuickPaintedItem::geometryChange(newGeometry, oldGeometry);
    if (_layer_field_line == nullptr || _layer_vision == nullptr) {
        return;
    }
    const int w = int(newGeometry.width());
    const int h = int(newGeometry.height());
    if (w > 0 && h > 0) {
        resize(w, h, true);
    }
}

void Field::resize(int width, int height, bool needUpdate) {
    if (width <= 0 || height <= 0) {
        return;
    }

    setSize(width, height);
    _area = QRect(0, 0, width, height);

    {
        std::scoped_lock<std::shared_mutex> lock(_mutex);
        if (_pm == nullptr || _pm->size() != QSize(width, height)) {
            if (_painter.isActive()) {
                _painter.end();
            }
            delete _pm;
            _pm = new QPixmap(width, height);
            _pm->fill(Color::_()->BACKGROUND);
            _painter.begin(_pm);
            _painter.setRenderHint(QPainter::Antialiasing, true);
            _painter.setRenderHint(QPainter::TextAntialiasing, true);
        }
    }

    _root.resize(width, height);

    {
        std::scoped_lock<std::shared_mutex> lock(_tf.mutex);
        _tf.update(width, height);
    }
    _tf.limit(width, height);

    {
        std::scoped_lock<std::shared_mutex> lock(_global_mutex);
        _global_tf = currentTransform(_tf);
        _global_width = width;
        _global_height = height;
    }

    if (_layer_field_line != nullptr) {
        _layer_field_line->draw();
    }

    if (needUpdate) {
        draw();
    }
}

void Field::setSize(int width, int height) {
    if (width <= 0 || height <= 0) {
        return;
    }
    ZSS::ZParamManager::_()->changeParam("canvas/height", height);
    ZSS::ZParamManager::_()->changeParam("canvas/width", width);
}

float Field::fieldXFromCoordinate(int x) {
    std::shared_lock<std::shared_mutex> lock(_global_mutex);
    bool ok = false;
    const QTransform inv = _global_tf.inverted(&ok);
    if (!ok) {
        return 0.0F;
    }
    return float(inv.map(QPointF(x, 0.0)).x());
}

float Field::fieldYFromCoordinate(int y) {
    std::shared_lock<std::shared_mutex> lock(_global_mutex);
    bool ok = false;
    const QTransform inv = _global_tf.inverted(&ok);
    if (!ok) {
        return 0.0F;
    }
    return float(inv.map(QPointF(0.0, y)).y());
}

void Field::updateArea() {
    update(_area);
}

void Field::sendSignal() {
    emit needDraw();
}

void Field::draw() {
    if (_pm == nullptr || _area.width() <= 0 || _area.height() <= 0) {
        return;
    }
    if (!_draw && _type > 0) {
        return;
    }

    _layer_field_line->draw();
    if (_type != 1) {
        _layer_vision->draw();
    } else {
        std::scoped_lock<std::shared_mutex> lock(_layer_vision->_mutex);
        if (_layer_vision->_image != nullptr) {
            _layer_vision->_image->fill(Qt::transparent);
        }
    }
    _root.draw();

    const QTransform tf = currentTransform(_tf);

    {
        std::scoped_lock<std::shared_mutex> lock(_mutex);
        _pm->fill(Color::_()->BACKGROUND);

        {
            std::shared_lock<std::shared_mutex> rootLock(_root._mutex);
            if (_root._image != nullptr) {
                _painter.drawImage(0, 0, *_root._image);
            }
        }

        if (_type == 1) {
            paintOriginVision(_painter, tf);
        } else if (_type == 2) {
            paintDebugMessages(_painter, tf, PARAM::BLUE);
        } else if (_type == 3) {
            paintDebugMessages(_painter, tf, PARAM::YELLOW);
        }

        if (_select_robots) {
            const OriginMessage& vision = GlobalData::instance()->processRobot[0];
            for (int i = 0; i < _selected_count; ++i) {
                const Msg::Robot* robot = findRobotById(vision, _selected_team, _selected_ids[i]);
                if (robot == nullptr) {
                    continue;
                }
                paintRobotShape(_painter,
                                tf,
                                QColor(0, 0, 0, 0),
                                robot->id,
                                robot->pos.x(),
                                robot->pos.y(),
                                robot->angle,
                                false,
                                Qt::white,
                                true);
            }
        }

        _painter.setBrush(QBrush(QColor(255, 255, 255, 20)));
        _painter.setPen(Qt::NoPen);
        _painter.drawRect(mapFieldRect(tf,
                                       GlobalSettings::instance()->minimumX,
                                       GlobalSettings::instance()->maximumX,
                                       GlobalSettings::instance()->minimumY,
                                       GlobalSettings::instance()->maximumY));

        const QColor selectColor = _selected_team == PARAM::BLUE ? QColor(0, 137, 167, 50) : QColor(221, 210, 59, 50);
        _painter.setBrush(QBrush(selectColor));
        _painter.setPen(QPen(selectColor, 2));
        _painter.drawRect(mapFieldRect(tf,
                                       GlobalSettings::instance()->selectCarMinX,
                                       GlobalSettings::instance()->selectCarMaxX,
                                       GlobalSettings::instance()->selectCarMinY,
                                       GlobalSettings::instance()->selectCarMaxY));

        if (_buttons == Qt::RightButton) {
            _painter.setBrush(Qt::NoBrush);
            _painter.setPen(QPen(Qt::white, std::max(1.0, std::abs(tf.m11()) * 0.2), Qt::DashLine));
            _painter.drawLine(mapFieldPoint(tf, _start), mapFieldPoint(tf, _end));
            _painter.drawText(mapFieldPoint(tf, _end), QString::number(_display_data, 'f', 2));
        }

        bool ctrlC = false;
        {
            QMutexLocker lock(&GlobalData::instance()->ctrlCMutex);
            ctrlC = GlobalData::instance()->ctrlC;
        }
        if (ctrlC) {
            QFont font("Ubuntu Mono", std::max(24, _area.height() / 8), QFont::Bold);
            _painter.setFont(font);
            _painter.setPen(QPen(Qt::white));
            _painter.drawText(QPointF(20, font.pixelSize() + 20), "CTRL_C");
        }
    }

    updateArea();
}

void Field::mousePressEvent(QMouseEvent* event) {
    _buttons = event->button();
    _mouse_modifiers = event->modifiers();
    _start_screen = event->position();
    _end_screen = _start_screen;
    _start = _end = toFieldPoint(_tf, _start_screen);
    _event_stage = 0;

    _check_press_robot = VisionModule::instance()->checkRobot(_start.x(), _start.y());
    _pressed_robot = _check_press_robot.res;
    _origin_robot = GlobalData::instance()->processRobot[0];
    _old_tf = currentTransform(_tf);

    if (_pressed_robot && !_select_robots) {
        _selected_team = _check_press_robot.team;
        _selected_count = 1;
        _selected_ids.fill(-1);
        _selected_ids[0] = int(_check_press_robot.id);
    }

    switch (_buttons) {
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

    draw();
}

void Field::mouseMoveEvent(QMouseEvent* event) {
    if (_buttons == Qt::NoButton) {
        return;
    }
    _mouse_modifiers = event->modifiers();
    _end_screen = event->position();
    _end = toFieldPoint(_tf, _end_screen);
    _event_stage = 1;

    switch (_buttons) {
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

    draw();
}

void Field::mouseReleaseEvent(QMouseEvent* event) {
    if (_buttons == Qt::NoButton) {
        return;
    }

    _mouse_modifiers = event->modifiers();
    _end_screen = event->position();
    _end = toFieldPoint(_tf, _end_screen);
    _event_stage = 2;

    switch (_buttons) {
    case Qt::LeftButton:
        e_left();
        break;
    case Qt::RightButton:
        e_right();
        e_right_r();
        break;
    case Qt::MiddleButton:
        e_middle();
        break;
    default:
        break;
    }

    _buttons = Qt::NoButton;
    _mouse_modifiers = Qt::NoModifier;
    _pressed_robot = false;

    draw();
}

void Field::e_left() {
    if (_mouse_modifiers == Qt::ControlModifier || _mouse_modifiers == Qt::AltModifier) {
        _selected_team = _mouse_modifiers == Qt::ControlModifier ? PARAM::BLUE : PARAM::YELLOW;

        const double minX = std::min(_start.x(), _end.x());
        const double maxX = std::max(_start.x(), _end.x());
        const double minY = std::min(_start.y(), _end.y());
        const double maxY = std::max(_start.y(), _end.y());

        if (maxX - minX < kMinSelectionLength || maxY - minY < kMinSelectionLength) {
            GlobalSettings::instance()->resetSelectCarArea();
            if (_event_stage == 2) {
                _selected_count = 0;
                _selected_ids.fill(-1);
                _select_robots = false;
            }
            return;
        }

        GlobalSettings::instance()->setSelectCarArea(minX, maxX, minY, maxY);

        if (_event_stage == 2) {
            _selected_count = 0;
            _selected_ids.fill(-1);
            const auto& vision = GlobalData::instance()->processRobot[0];
            for (int i = 0; i < vision.robotSize[_selected_team]; ++i) {
                const auto& robot = vision.robot[_selected_team][i];
                if (robot.pos.x() > minX && robot.pos.x() < maxX && robot.pos.y() > minY && robot.pos.y() < maxY) {
                    if (_selected_count < PARAM::ROBOTNUM) {
                        _selected_ids[_selected_count++] = int(robot.id);
                    }
                }
            }
            _select_robots = _selected_count > 0;
            GlobalSettings::instance()->resetSelectCarArea();
        }
        return;
    }

    if (!_pressed_robot) {
        Simulator::instance()->setBall(_end.x() / 1000.0, _end.y() / 1000.0);
        return;
    }

    const QPointF delta = _end - _start;

    if (!_select_robots) {
        _selected_team = _check_press_robot.team;
        _selected_count = 1;
        _selected_ids.fill(-1);
        _selected_ids[0] = int(_check_press_robot.id);
    }

    const OriginMessage& current = GlobalData::instance()->processRobot[0];
    for (int i = 0; i < _selected_count; ++i) {
        const int id = _selected_ids[i];
        const Msg::Robot* originRobot = findRobotById(_origin_robot, _selected_team, id);
        const Msg::Robot* currentRobot = findRobotById(current, _selected_team, id);
        if (currentRobot == nullptr) {
            continue;
        }

        const QPointF base = originRobot == nullptr
            ? QPointF(currentRobot->pos.x(), currentRobot->pos.y())
            : QPointF(originRobot->pos.x(), originRobot->pos.y());

        const double dir = currentRobot->angle * 180.0 / M_PI;
        Simulator::instance()->setRobot((base.x() + delta.x()) / 1000.0,
                                        (base.y() + delta.y()) / 1000.0,
                                        id,
                                        _selected_team == PARAM::YELLOW,
                                        dir);
    }
}

void Field::e_right() {
    const QLineF line(_start, _end);
    if (_pressed_robot) {
        _display_data = -line.angle();
        if (_display_data < -180.0) {
            _display_data += 360.0;
        }

        const OriginMessage& current = GlobalData::instance()->processRobot[0];
        for (int i = 0; i < _selected_count; ++i) {
            const int id = _selected_ids[i];
            const Msg::Robot* robot = findRobotById(current, _selected_team, id);
            if (robot == nullptr) {
                continue;
            }
            Simulator::instance()->setRobot(robot->pos.x() / 1000.0,
                                            robot->pos.y() / 1000.0,
                                            id,
                                            _selected_team == PARAM::YELLOW,
                                            _display_data);
        }
    } else {
        _display_data = kBallSpeedRatio * line.length() / 1000.0;
    }
}

void Field::e_right_r() {
    if (_pressed_robot) {
        return;
    }
    const QLineF line(_start, _end);
    Simulator::instance()->setBall(_start.x() / 1000.0,
                                   _start.y() / 1000.0,
                                   kBallSpeedRatio * line.dx() / 1000.0,
                                   kBallSpeedRatio * line.dy() / 1000.0);
}

void Field::e_middle() {
    switch (_mouse_modifiers) {
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
    if (_event_stage == 0) {
        _old_tf = currentTransform(_tf);
        return;
    }

    const QPointF delta = _end_screen - _start_screen;
    const double scale = std::abs(_old_tf.m11());
    QTransform next(scale, 0, 0, -scale, _old_tf.dx() + delta.x(), _old_tf.dy() + delta.y());

    {
        std::scoped_lock<std::shared_mutex> lock(_tf.mutex);
        _tf._ = next;
    }
    _tf.limit(_area.width(), _area.height());

    {
        std::scoped_lock<std::shared_mutex> lock(_global_mutex);
        _global_tf = currentTransform(_tf);
    }

    _layer_field_line->draw();
}

void Field::e_middle_alt() {
    GlobalSettings::instance()->setBallPlacementPos(_end.x(), _end.y());
}

void Field::e_middle_ctrl() {
    const double minX = std::min(_start.x(), _end.x());
    const double maxX = std::max(_start.x(), _end.x());
    const double minY = std::min(_start.y(), _end.y());
    const double maxY = std::max(_start.y(), _end.y());

    if (maxX - minX < kMinSelectionLength || maxY - minY < kMinSelectionLength) {
        GlobalSettings::instance()->resetArea();
    } else {
        GlobalSettings::instance()->setArea(minX, maxX, minY, maxY);
    }
}

#if QT_CONFIG(wheelevent)
void Field::wheelEvent(QWheelEvent* event) {
    if (_area.width() <= 0 || _area.height() <= 0) {
        return;
    }

    const int delta = event->angleDelta().y();
    if (delta == 0) {
        return;
    }

    const QTransform cur = currentTransform(_tf);
    const double currentScale = std::abs(cur.m11());
    const double baseScale = std::min(double(_area.width()) / FP::_()->p_canvas_width,
                                      double(_area.height()) / FP::_()->p_canvas_height);
    if (baseScale <= 0.0) {
        return;
    }

    const qreal oldRatio = clampValue(baseScale / currentScale, kZoomMin, 1.0);
    qreal nextRatio = oldRatio + (delta < 0 ? kZoomStep : -kZoomStep);
    nextRatio = clampValue(nextRatio, kZoomMin, qreal(1.0));
    const double nextScale = baseScale / nextRatio;

    bool ok = false;
    const QTransform inv = cur.inverted(&ok);
    if (!ok) {
        return;
    }

    const QPointF cursor = event->position();
    const QPointF world = inv.map(cursor);

    QTransform next(nextScale,
                    0,
                    0,
                    -nextScale,
                    cursor.x() - nextScale * world.x(),
                    cursor.y() + nextScale * world.y());

    {
        std::scoped_lock<std::shared_mutex> lock(_tf.mutex);
        _tf._ = next;
    }
    _tf.limit(_area.width(), _area.height());

    {
        std::scoped_lock<std::shared_mutex> lock(_global_mutex);
        _global_tf = currentTransform(_tf);
    }

    _layer_field_line->draw();
    draw();
    event->accept();
}
#endif
