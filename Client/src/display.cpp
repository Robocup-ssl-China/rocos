#include "display.h"
#include "visionmodule.h"
#include "parammanager.h"
#include "globaldata.h"
namespace{
const QColor COLOR_BACKGROUND(40,40,40);
const QColor COLOR_AXES(100,100,100);
auto zpm = ZSS::ZParamManager::instance();
template<typename T>
T limitRange(T value, T minValue, T maxValue) {
    return value > maxValue ? maxValue : (value < minValue) ? minValue : value;
}
float MAX_SPEED;// m/s
float LIMIT_SPEED;// m/s
float MAP_WIDTH;
float MAP_HEIGHT;
int DISPLAY_COLUMN;
float x(float a){return limitRange(a,0.0f,1.0f)*MAP_WIDTH;};
float y(float a){return (1 - limitRange(a,0.0f,MAX_SPEED)/MAX_SPEED)*MAP_HEIGHT;};
float w(float a){return a*MAP_WIDTH;};
float h(float a){return -a/MAX_SPEED*MAP_HEIGHT;};
}
Display::Display(QQuickItem *parent)
    : QQuickPaintedItem (parent)
    , pixmap(nullptr){
    connect(VisionModule::instance(), SIGNAL(needDraw()), this, SLOT(draw()));
    setImplicitWidth(200);
    setImplicitHeight(300);
    pixmap = new QPixmap(QSize(200, 300));
    pixmapPainter.begin(pixmap);
    pixmapPainter.setPen(Qt::NoPen);
    pixmapPainter.setRenderHint(QPainter::HighQualityAntialiasing, true);
    pixmapPainter.setRenderHint(QPainter::TextAntialiasing, true);
    init();
}
void Display::paint(QPainter* painter) {
    painter->drawPixmap(area, *pixmap);
}
void Display::draw() {
    repaint();
}
void Display::init(){
    initAxes();
    repaint();
}
void Display::initAxes(){
    MAX_SPEED = zpm->value("BallSpeed/maxSpeed",QVariant(8.0f)).toFloat();// m/s
    LIMIT_SPEED = zpm->value("BallSpeed/limitSpeed",QVariant(6.5f)).toFloat();// m/s
    MAP_WIDTH = this->property("width").toReal();
    MAP_HEIGHT = this->property("height").toReal();
    DISPLAY_COLUMN = zpm->value("BallSpeed/horizontLines",QVariant(6)).toInt();
//    axesPath = QPainterPath();
//    QFont font("Helvetica [Cronyx]");
//    font.setWeight(QFont::ExtraLight);
//    font.setPixelSize(h(-0.5));
//    for(int i=0;i<displayColumn;i++){
//        axesPath.moveTo(x(0),y(maxSpeed/displayColumn*i));
//        axesPath.lineTo(x(1),y(maxSpeed/displayColumn*i));
//        axesPath.addText(QPointF(x(0),y(maxSpeed/displayColumn*i)),font,QString::number(maxSpeed/displayColumn*i,'g',2));
//    }
}
void Display::repaint(){
    if(repaint_mutex.try_lock()){
        pixmap->fill(COLOR_BACKGROUND);
        paintAxes();
        paintData();
        this->update(area);
        repaint_mutex.unlock();
    }
}
void Display::paintAxes(){
//    pen.setColor(COLOR_AXES);
//    pen.setWidth(1);
//    pixmapPainter.setPen(pen);
//    pixmapPainter.setBrush(Qt::red);
//    pixmapPainter.strokePath(axesPath, pen);
    pen.setColor(COLOR_AXES);
    pen.setWidth(1);
    pixmapPainter.setPen(pen);
    pixmapPainter.setFont(QFont("Helvetica [Cronyx]"));
    for(int i=0;i<DISPLAY_COLUMN;i++){
        auto height = MAX_SPEED/DISPLAY_COLUMN*i;
        pixmapPainter.drawLine(::x(0),::y(height),::x(1),::y(height));
        pixmapPainter.drawText(QPointF(::x(0),::y(height+0.1)),QString::number(height,'g',2));
    }
    auto height = LIMIT_SPEED;
    pixmapPainter.setPen(Qt::red);
    pixmapPainter.drawLine(::x(0),::y(height),::x(1),::y(height));
    pixmapPainter.drawText(QPointF(::x(0),::y(height+0.1)),QString::number(height,'g',2));
}
void Display::paintData(){
    pixmapPainter.setPen(QColor(200,200,200));
    auto gd = GlobalData::instance()->maintain;
    QPoint lastPoint(::x(0.0f),::y(0.0f));
    QPoint newPoint = lastPoint;
    for(int i=0;i<ZSS::Athena::Vision::MAINTAIN_STORE_BUFFER;i+=1){
        auto speed = gd[1+i-ZSS::Athena::Vision::MAINTAIN_STORE_BUFFER].ball->velocity.mod();
        newPoint.setX(::x((float)i/ZSS::Athena::Vision::MAINTAIN_STORE_BUFFER));
        newPoint.setY(::y(speed/1000.0f));
        pixmapPainter.drawLine(lastPoint,newPoint);
        lastPoint = newPoint;
    }
}
void Display::resetSize(int width,int height){
    pixmapPainter.end();
    delete pixmap;
    pixmap = new QPixmap(QSize(this->property("width").toReal(), this->property("height").toReal()));
    pixmapPainter.begin(pixmap);
    area = QRect(0, 0, this->property("width").toReal(), this->property("height").toReal());
    init();
}
