#ifndef LAYER_H
#define LAYER_H
#include <mutex>
#include <string>
#include <QImage>
#include <QPainter>
#include <QElapsedTimer>
#include "zos/core.h"
#include "fieldconfig.h"
class Layer{
public:
    Layer(const std::string& name="undefined",FieldTransform* _tf=nullptr);
    virtual ~Layer();
    virtual bool draw() = 0;
    virtual void resize(int,int) final;
    QImage* get() const{ return _image;}
public:
    QImage* _image = nullptr;
    QPainter _painter;
    QRect _area;
    mutable std::shared_mutex _mutex;
    std::string _name;
    zos::Publisher p_draw_done;
    zos::Subscriber<2> s_need_draw;
public:
    bool addChild(Layer*);
    bool addSibling(Layer*);
    bool linkDrawFinish(Layer*);
    mutable std::shared_mutex _mutex_tree;
    Layer* _child = nullptr;
    Layer* _sibling = nullptr;
    bool _need_exit = false;
public:
    QElapsedTimer _timer;
    bool _first = false;
public:
    FieldTransform* _=nullptr;
};

class LayerNode : public Layer{
public:
    LayerNode(const std::string& name="LayerNode");
    virtual ~LayerNode();
    virtual bool draw() override;
};

#endif // LAYER_H
