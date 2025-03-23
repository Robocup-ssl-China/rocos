#include "fieldlayers.h"
#include "parammanager.h"
#include "staticparams.h"
#include "themecolor.h"
#include "interaction.h"
#include <QtMath>
namespace {
}
FieldLineLayer::FieldLineLayer(FieldTransform* _tf):Layer("FieldLine",_tf){
}
bool FieldLineLayer::draw(){
    std::scoped_lock<std::shared_mutex> lock(_mutex);
    if(_image == nullptr) return false;
    _image->fill(Qt::transparent);
    _painter.setPen(QPen(Color::_()->FIELDLINE,_->s(20)));
    drawPath();
    return true;
}
void FieldLineLayer::drawPath(){
    _painter.drawRect(_->_.mapRect(QRectF{(-_fp->p_width / 2.0-_fp->p_margin), (-_fp->p_height / 2.0-_fp->p_margin), double(_fp->p_width+2*_fp->p_margin), double(_fp->p_height+2*_fp->p_margin)}));
    _painter.drawRect(_->_.mapRect(QRectF{(-_fp->p_width / 2.0), (-_fp->p_height / 2.0), double(_fp->p_width), double(_fp->p_height)}));
    _painter.drawRect(_->_.mapRect(QRectF{(-_fp->p_width / 2.0), (-_fp->p_goalWidth / 2.0), double(-_fp->p_goalDepth), double(_fp->p_goalWidth)}));
    _painter.drawRect(_->_.mapRect(QRectF{(_fp->p_width / 2.0), (-_fp->p_goalWidth / 2.0), double(_fp->p_goalDepth), double(_fp->p_goalWidth)}));
    _painter.drawLine(_->_.map(QPointF(0,-_fp->p_height / 2.0)),_->_.map(QPointF(0,_fp->p_height / 2.0)));
    _painter.drawEllipse(_->_.mapRect(QRectF((-_fp->p_centerCircleRadius), (-_fp->p_centerCircleRadius), (2 * _fp->p_centerCircleRadius), (2 * _fp->p_centerCircleRadius))));
    _painter.drawRect(_->_.mapRect(QRectF{(-_fp->p_width / 2.0), (-_fp->p_penaltyLength / 2.0), double(_fp->p_penaltyWidth), double(_fp->p_penaltyLength)}));
    _painter.drawRect(_->_.mapRect(QRectF{(_fp->p_width / 2.0), (-_fp->p_penaltyLength / 2.0), double(-_fp->p_penaltyWidth), double(_fp->p_penaltyLength)}));
}
bool VisionLayer::draw(){
    if(_timer.elapsed()<1000.0/PARAM::GUI_DRAW_RATE)
        return false;
    _timer.restart();
    if (_dataMutex.try_lock_shared()) {
        drawFusionVision(_msg);
        _dataMutex.unlock_shared();
        this->p_draw_done.publish();
    }
    return true;
}
void VisionLayer::receiveData(const zos::Data &data) {
    {
        std::scoped_lock lock(_dataMutex);
        _msg.ParseFromArray(data.data(), data.size());
    }
    draw();
}
void VisionLayer::drawFusionVision(const Vision_DetectionFrame& frame){
    std::scoped_lock<std::shared_mutex> lock(_mutex);
    if(_image == nullptr) return;
    _image->fill(Qt::transparent);

    static int visionCount = 0;
    if (frame.has_balls()) {
        double ratio = 1;
        if(frame.balls().has_height() && frame.balls().height()>0){
            double cam_height = 1100;// TODO
            double r = 90;
            ratio = 1+1.0/(std::max(1.0+1.0/r,cam_height/frame.balls().height())-1);
        }
        paintBall(Color::_()->BALL, frame.balls().x(), frame.balls().y(),ratio*(FP::_()->s_ballDiameter/2));
        paintFocus(Color::_()->BALL_FOCUS,frame.balls().x(), frame.balls().y(),500,visionCount++);
    }
    int blue_size = frame.robots_blue_size();
    int yellow_size = frame.robots_yellow().size();
    for (int i = 0; i < blue_size; i++) {
        const Vision_DetectionRobot& robot = frame.robots_blue(i);
        paintRobot(Color::_()->ROBOT[PARAM::BLUE], robot.robot_id(), robot.x(), robot.y(), robot.orientation());
    }
    for (int i = 0; i < yellow_size; i++) {
        const Vision_DetectionRobot& robot = frame.robots_yellow(i);
        paintRobot(Color::_()->ROBOT[PARAM::YELLOW], robot.robot_id(), robot.x(), robot.y(), robot.orientation());
    }
}
void VisionLayer::paintRobot(const QColor& color,quint8 num,qreal x,qreal y,qreal radian
                             ,bool ifDrawNum,const QColor& textColor,bool needCircle){
    static qreal radius = FP::_()->s_carDiameter / 2.0;
    static qreal chordAngel = qRadiansToDegrees(qAcos(1.0 * FP::_()->s_carFaceWidth / FP::_()->s_carDiameter));
    _painter.setBrush(QBrush(color));
    _painter.setPen(QPen(Color::_()->ROBOT_DIR, _->s(30), Qt::DotLine));
    _painter.drawLine(_->_.map(QPointF(x,y)), _->_.map(QPointF(x,y)+QPointF(140 * qSin(-radian + M_PI_2), 140 * qCos(-radian + M_PI_2))));
    _painter.setPen(Qt::NoPen);
    _painter.drawChord(_->_.mapRect(QRectF{x - radius,y - radius,double(2 * radius), double(2 * radius)}), _->a(90.0 - chordAngel + 180 / M_PI * -radian), _->a(180.0 + 2 * chordAngel));
    if (needCircle) {
        _painter.setBrush(Qt::NoBrush);
        _painter.setPen(QPen(Color::_()->ROBOT_CIRCLE, _->s(30)));
        _painter.drawChord(_->_.mapRect(QRectF{x - radius,y - radius, double(2 * radius), double(2 * radius)}), _->a(90.0 - chordAngel + 180 / M_PI * -radian), _->a(180.0 + 2 * chordAngel));
    }
    if (ifDrawNum) {
        _painter.setBrush(Qt::NoBrush);
        _painter.setPen(QPen(textColor, _->s(30)));
        QFont font;
        int fontSize = _->s(FP::_()->s_number);
        font.setPixelSize(fontSize);
        _painter.setFont(font);
        _painter.drawText(_->_.map(QPointF(x - FP::_()->s_number,y + FP::_()->s_carDiameter * 0.4)), QString::number(num, 16).toUpper());
    }
}
void VisionLayer::paintBall(const QColor& color,qreal x,qreal y,qreal r){
    _painter.setBrush(QBrush(color));
    _painter.setPen(Qt::NoPen);
    _painter.drawEllipse(_->_.mapRect(QRectF(x - r, y - r, double(r*2), double(r*2))));
}
void VisionLayer::paintFocus(const QColor& color, qreal x, qreal y, qreal radian, qreal count) {
    static int length = 40;
    _painter.setBrush(Qt::NoBrush);
    _painter.setPen(QPen(color, _->s(20)));
    _painter.drawEllipse(_->_.mapRect(QRectF((x - radian), (y - radian), (2 * radian), (2 * radian))));
    _painter.setPen(QPen(color, _->s(40)));
    for(int i = 0; i < 4; i++) {
        _painter.drawLine(
                    _->_.map(
                        QPointF(
                            (x + (radian + length)*qCos(count / 60.0 + i * M_PI / 2)),
                            (y + (radian + length)*qSin(count / 60.0 + i * M_PI / 2))
                            )
                        ),
                    _->_.map(
                        QPointF(
                            (x + (radian - length)*qCos(count / 60.0 + i * M_PI / 2)),
                            (y + (radian - length)*qSin(count / 60.0 + i * M_PI / 2))
                            )
                        )
                    );
    }
}
DebugLayer::DebugLayer(FieldTransform* _tf,int team):Layer(team == PARAM::YELLOW ? "DebugYellow" : "DebugBlue",_tf)
    , _receiver(ZSS::Athena::DEBUG_MSG_RECEIVE[team],[this](const void* ptr, size_t size){receiveData(ptr,size);}){
    CInteraction::instance()->runUDPContext();
}
void DebugLayer::receiveData(const void* ptr,size_t size){
    {
        std::scoped_lock lock(_dataMutex);
        _msg.ParseFromArray(ptr,size);
    }
    draw();
}
bool DebugLayer::draw(){
    if(_timer.elapsed()<1000.0/PARAM::GUI_DRAW_RATE)
        return false;
    _timer.restart();
    if (_dataMutex.try_lock_shared()) {
        paintDebug(_msg);
        _dataMutex.unlock_shared();
        this->p_draw_done.publish();
    }
    return true;
}
void DebugLayer::paintDebug(const ZSS::Protocol::Debug_Msgs&){
    std::scoped_lock<std::shared_mutex> lock(_mutex);
    if(_image == nullptr) return;
    _image->fill(Qt::transparent);
    QFont font;
    int fontSize = _->s(130);
    font.setPixelSize(fontSize);
    _painter.setFont(font);
    _painter.setBrush(QBrush(Color::_()->DEBUG_BRUSH_COLOR));
    for(int i = 0; i < _msg.msgs_size(); i++) {
        auto& msg = _msg.msgs(i);
        if(msg.color() == ZSS::Protocol::Debug_Msg::Color::Debug_Msg_Color_USE_RGB) {
            int value = msg.rgb_value();
            int rgb[3];
            for(int i = 0; i < 3; i++) {
                rgb[2 - i] = value % 1000;
                value = (value - rgb[2 - i]) / 1000;
                if(rgb[2 - i]>255||rgb[2 - i]<0){//error value
                    rgb[0]=0;rgb[1]=0;rgb[2]=0;break;
                }
            }
            _painter.setPen(QPen(QColor(rgb[0], rgb[1], rgb[2]), _->s(15)));
        }else{
            _painter.setPen(QPen(Color::_()->DEBUG_MSG[msg.color()], _->s(15)));
        }
        double x1, x2, y1, y2, x3, y3, x4, y4;
        double minx,miny,maxx,maxy;
        QPainterPath newpath;
        switch(msg.type()) {
        case ZSS::Protocol::Debug_Msg_Debug_Type_ARC:
            x1 = msg.arc().rect().point1().x();
            x2 = msg.arc().rect().point2().x();
            y1 = msg.arc().rect().point1().y();
            y2 = msg.arc().rect().point2().y();
            minx = std::min(x1,x2);
            miny = std::min(y1,y2);
            maxx = std::max(x1,x2);
            maxy = std::max(y1,y2);
            _painter.drawArc(_->_.mapRect(QRectF((double(minx)),(double(miny)),((maxx - minx)),((maxy - miny)))),
                                  _->a(msg.arc().start()), _->a(msg.arc().span()));
            break;
        case ZSS::Protocol::Debug_Msg_Debug_Type_LINE:
            _painter.drawLine(_->_.map(QPointF(msg.line().start().x(),msg.line().start().y())),_->_.map(QPointF(msg.line().end().x(),msg.line().end().y())));
            break;
        case ZSS::Protocol::Debug_Msg_Debug_Type_POINTS: {
            QVector<QLineF> lines;
            for(int i = 0; i < msg.points().point_size(); i++) {
                lines.push_back(_->_.map(QLineF((msg.points().point(i).x() + FP::_()->s_debugPoint), (msg.points().point(i).y() + FP::_()->s_debugPoint), (msg.points().point(i).x() - FP::_()->s_debugPoint), (msg.points().point(i).y() - FP::_()->s_debugPoint))));
                lines.push_back(_->_.map(QLineF((msg.points().point(i).x() - FP::_()->s_debugPoint), (msg.points().point(i).y() + FP::_()->s_debugPoint), (msg.points().point(i).x() + FP::_()->s_debugPoint), (msg.points().point(i).y() - FP::_()->s_debugPoint))));
            }
            _painter.drawLines(lines);
            break;
        }
        case ZSS::Protocol::Debug_Msg_Debug_Type_TEXT:
            font.setPointSizeF(_->s(msg.text().size()));
            font.setWeight(static_cast<QFont::Weight>(msg.text().weight()));
            _painter.setFont(font);
            _painter.drawText(_->_.map(QPointF((msg.text().pos().x()), (msg.text().pos().y()))), QString::fromStdString(msg.text().text()));
            break;
        case ZSS::Protocol::Debug_Msg_Debug_Type_ROBOT:
            break;
        case ZSS::Protocol::Debug_Msg_Debug_Type_BEZIER:
            x1 = msg.bezier().start().x();
            y1 = msg.bezier().start().y();
            x2 = msg.bezier().c1().x();
            y2 = msg.bezier().c1().y();
            x3 = msg.bezier().end().x();
            y3 = msg.bezier().end().y();
            x4 = msg.bezier().c2().x();
            y4 = msg.bezier().c2().y();
            newpath.moveTo(_->_.map(QPointF((x1), (y1))));
            newpath.cubicTo(_->_.map(QPointF((x2),(y2))), _->_.map(QPointF((x4), (y4))), _->_.map(QPointF((x3),(y3))));
            _painter.drawPath(newpath);
            break;
        default:
            qDebug() << "debug message type not support!";
        }
    }
}
