#include "layer.h"
#include "setthreadname.h"
#include "zos/log.h"
#include "staticparams.h"
#include <QtDebug>
Layer::Layer(const std::string& name,FieldTransform* _tf):_name(name),p_draw_done("draw_done"),_(_tf)
    ,s_need_draw("need_draw",
        [this](const zos::Data&){
            if(this->draw())
                this->p_draw_done.publish();
        }){
    _timer.start();
}
Layer::~Layer(){
    delete _child;
    delete _sibling;
    std::scoped_lock lock(_mutex);
    _need_exit = true;
    if(_painter.isActive())
        _painter.end();
    delete _image;
    _image = nullptr;
}
void Layer::resize(int w,int h){
    if(w <= 0 || h <= 0) return;
    if(w == _area.width() && h == _area.height())
        return;
    {
        std::scoped_lock lock(_mutex);
        if(_need_exit) return;
        if(_painter.isActive())
            _painter.end();
        delete _image;
        _image = new QImage(QSize(w,h),QImage::Format_ARGB32_Premultiplied);
        _image->fill(Qt::transparent);
        _area = QRect(0,0,w,h);
        _painter.begin(_image);
        _painter.setRenderHint(QPainter::Antialiasing, true);
        _painter.setRenderHint(QPainter::TextAntialiasing, true);
        _first = false;
    }
    if(_child!=nullptr)
        _child->resize(w,h);
    if(_sibling!=nullptr)
        _sibling->resize(w,h);
}
//bool Layer::unlinkChild(Layer* l){}
//bool Lyaer::unlinkSibling(Layer* l){}
//bool Layer::removeChild(Layer* l){}
//bool Layer::removeSibling(Layer* l){}
bool Layer::addChild(Layer* l){
    qDebug() << QString::fromStdString(_name) << "addChild : " << QString::fromStdString(l->_name);
    std::scoped_lock s_lock(_mutex_tree);
    if(_child != nullptr){
        if(auto res = !_child->addSibling(l);res){
            return false;
        }
    }else{
        _child = l;
    }
    l->linkDrawFinish(this);
    return true;
}
bool Layer::addSibling(Layer* l){
    // sibling no need to lock
    if(_sibling != nullptr)
        return _sibling->addSibling(l);
    _sibling = l;
    qDebug() << QString::fromStdString(_name) << "addSibling : " << QString::fromStdString(l->_name);
    return true;
}
bool Layer::linkDrawFinish(Layer* l){
    this->p_draw_done.link(&(l->s_need_draw));
    return true;
}

LayerNode::LayerNode(const std::string& name):Layer(name,nullptr){}
LayerNode::~LayerNode(){}
bool LayerNode::draw(){
    if(_timer.elapsed()>1000.0/PARAM::GUI_DRAW_RATE || !_first){
        _first = true;
        _timer.restart();
        std::shared_lock s_lock(_mutex_tree);
        if(_mutex.try_lock()){
            if(_image == nullptr){
                _mutex.unlock();
                return false;
            }
            _image->fill(Qt::transparent);
            Layer* p_child = _child;
            while(p_child!=nullptr){
                {
                    std::shared_lock lock(p_child->_mutex);
                    _painter.drawImage(0,0,*(p_child->get()));
                }
                p_child = p_child->_sibling;
            }
            _mutex.unlock();
            return true;
        }
        return false;
    }
    return false;
}
