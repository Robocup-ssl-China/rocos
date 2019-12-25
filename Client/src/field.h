#ifndef __FIELD_H__
#define __FIELD_H__

#include <QQuickPaintedItem>
#include <QPainter>
#include <QPainterPath>
#include <QRectF>
#include <QMutex>
#include "messageformat.h"
#include "staticparams.h"
#include "interaction.h"
class Field : public QQuickPaintedItem{
    Q_OBJECT
    Q_PROPERTY(int type READ type WRITE setType)
    Q_PROPERTY(bool draw READ ifDraw WRITE setDraw)
public:
    void paint(QPainter* painter) override;
    Field(QQuickItem *parent = 0);
    inline int type() { return this->_type; }
    inline void setType(int t) { this->_type = t; }
    inline bool ifDraw() { return this->_draw; }
    inline void setDraw(bool t) { this->_draw = t; }
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
//    void hoverMoveEvent(QHoverEvent*) override;
    static float fieldXFromCoordinate(int);
    static float fieldYFromCoordinate(int);
    static void setSize(int width,int height);
    double getpredict_x();
    double getpredict_y();
//    void mouseDoubleClickEvent(QMouseEvent *event) override;
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent * event) override;
#endif
    virtual ~Field() {}

public slots:
    void draw();
private slots:
    void repaint();
private:
    void init();
    void initPainterPath();
    void initField();
    void paintInit();
    void paintCar(const QColor& color,quint8 num,qreal x,qreal y,qreal radian
                  ,bool ifDrawNum = true,const QColor& textColor = Qt::white,bool needCircle = false);
    void paintSelectedCar();
    void paintCarShadow(const QColor& color,qreal x, qreal y, qreal radian);
    void paintBall(const QColor& color,qreal x,qreal y);
    void paintShadow(const QColor& color,qreal x,qreal y);
    void paintFocus(const QColor& color,qreal x,qreal y,qreal radian = 500,qreal count = 0);
    void drawOriginVision(int);
    void drawMaintainVision(int);
    void drawVision(const OriginMessage&,bool shadow = false);
    void leftMoveEvent(QMouseEvent *);
    void leftPressEvent(QMouseEvent *);
    void leftReleaseEvent(QMouseEvent *);
    void leftCtrlModifierMoveEvent(QMouseEvent *e);
    void leftCtrlModifierPressEvent(QMouseEvent *e);
    void leftCtrlModifierReleaseEvent(QMouseEvent *e);
    void leftAltModifierMoveEvent(QMouseEvent *e);
    void leftAltModifierPressEvent(QMouseEvent *e);
    void leftAltModifierReleaseEvent(QMouseEvent *e);
    void rightMoveEvent(QMouseEvent *);
    void rightPressEvent(QMouseEvent *);
    void rightReleaseEvent(QMouseEvent *);
    void middleMoveEvent(QMouseEvent *);
    void middlePressEvent(QMouseEvent *);
    void middleReleaseEvent(QMouseEvent *);
    void middleNoModifierMoveEvent(QMouseEvent *);
    void middleNoModifierPressEvent(QMouseEvent *);
    void middleNoModifierReleaseEvent(QMouseEvent *);
    void middleAltModifierMoveEvent(QMouseEvent *);
    void middleAltModifierPressEvent(QMouseEvent *);
    void middleAltModifierReleaseEvent(QMouseEvent *);
    void middleCtrlModifierMoveEvent(QMouseEvent *);
    void middleCtrlModifierPressEvent(QMouseEvent *);
    void middleCtrlModifierReleaseEvent(QMouseEvent *);
    void paintpredict();
    QPixmap *pixmap;
    QPainter pixmapPainter;
    QMutex repaint_mutex;
    bool cameraMode;
    int _type;
    bool _draw;

private:
    int pressed;
    bool pressedRobot;
    QPointF start,end;
    double displayData;
private:
    void checkClosestRobot(double,double);
    void resetAfterMouseEvent();
    void drawBallLine();
private:
    void drawDebugMessages(int team);
    void drawCtrlC();
    void drawSelectedArea();
    void paintArc();
    void paintLine();
    void paintText();
    void paintCurve();
    void paintPolygon();

    int ballFocusCount = 0;
// draw Score
private:
    const static int RECT_SIZE = 100;// mm
    const static int RECT_MAX_SUM = 10000;
    const static int COLOR_LEVEL = 256;
//    struct Score{
//        QRectF area[COLOR_LEVEL][RECT_MAX_SUM];
//        int size[COLOR_LEVEL];
//        Score();
//        void init();
//    };
//    Score score;
    QUdpSocket * socket_score;
    void receiveScore();
    void parseScores(QUdpSocket* const);

    QMutex score_mutex;
    QPixmap *score_pixmap;
    QPainter scorePainter;
};

#endif // __FIELD_H__
