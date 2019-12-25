#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <QQuickPaintedItem>
#include <QPainter>
#include <QPixmap>
#include <QRect>
#include <QMutex>
#include <QPainterPath>
class Display : public QQuickPaintedItem{
    Q_OBJECT
    Q_PROPERTY(int type READ type WRITE setType)
public:
    Q_INVOKABLE void resetSize(int,int);
    void paint(QPainter* painter) override;
    Display(QQuickItem *parent = 0);
    inline int type() { return this->_type; }
    inline void setType(int t) { this->_type = t; }
    virtual ~Display() {}
public slots:
    void draw();
private slots:
    void repaint();
private:
    void init();
    void initAxes();
private:
    void paintAxes();
    void paintData();
private:
    float x();
    float y();
private:
    int _type;
    QPixmap *pixmap;
    QPainter pixmapPainter;
    QRect area;
    QMutex repaint_mutex;
    QPainterPath axesPath;
    QPen pen;
};

#endif // __DISPLAY_H__
