#include <mutex>
#include "fieldconfig.h"
#include "parammanager.h"
FieldParam::FieldParam(){
    reload();
}
FieldParam::~FieldParam(){}
void FieldParam::reload(){
    ZSS::ZParamManager::_()->loadParam(p_margin, "field/margin", 300);
    ZSS::ZParamManager::_()->loadParam(p_width, "field/width", 12000);
    ZSS::ZParamManager::_()->loadParam(p_height, "field/height", 9000);
    ZSS::ZParamManager::_()->loadParam(p_canvas_width, "field/canvasWidth", 13200);
    ZSS::ZParamManager::_()->loadParam(p_canvas_height, "field/canvasHeight", 9900);
    ZSS::ZParamManager::_()->loadParam(p_goalWidth, "field/goalWidth", 1200);
    ZSS::ZParamManager::_()->loadParam(p_goalDepth, "field/goalDepth",  200);
    ZSS::ZParamManager::_()->loadParam(p_penaltyWidth, "field/penaltyWidth", 1200);
    ZSS::ZParamManager::_()->loadParam(p_penaltyLength, "field/penaltyLength", 2400);
    ZSS::ZParamManager::_()->loadParam(p_centerCircleRadius, "field/centerCircleRadius",  500);
    ZSS::ZParamManager::_()->loadParam(p_lineWidth,"field/lineWidth",20);
    ZSS::ZParamManager::_()->loadParam(s_ballDiameter, "size/ballDiameter", 50);
    ZSS::ZParamManager::_()->loadParam(s_shadowDiameter, "size/shadowDiameter", 30);
    ZSS::ZParamManager::_()->loadParam(s_carDiameter, "size/carDiameter", 180);
    ZSS::ZParamManager::_()->loadParam(s_carFaceWidth, "size/carFaceWidth", 120);
    ZSS::ZParamManager::_()->loadParam(s_number, "size/numberSize", 200);
    ZSS::ZParamManager::_()->loadParam(s_debugPoint, "size/debugPointSize", 5);
}
void FieldTransform::update(double w,double h){
    auto s = std::min(double(w)/FP::_()->p_canvas_width,double(h)/FP::_()->p_canvas_height);
    const double dx = w/2;
    const double dy = h/2;
    const double r = 0;
    _ = QTransform(s,0,0,-s,dx,dy).rotate(r);
}
double FieldTransform::scale(){
    auto m11 = _.m11();
    auto m12 = _.m12();
    auto s = std::sqrt(m11*m11+m12*m12);
    return s;
}
double FieldTransform::dx(){
    return _.dx();
}
double FieldTransform::dy(){
    return _.dy();
}
double FieldTransform::s(double s){
    return scale()*s;
}
double FieldTransform::a(double a){
    return -a*16;
}
void FieldTransform::limit(double w,double h){
    {
        std::shared_lock lock(mutex);
        auto _s = scale();
        auto _dx = dx();
        auto _dy = dy();
    }
    auto s_min = std::min(double(w)/FP::_()->p_canvas_width,double(h)/FP::_()->p_canvas_height);
    auto s = std::max(s_min,scale());
    auto ww = std::max(w,s*FP::_()->p_canvas_width);
    auto hh = std::max(h,s*FP::_()->p_canvas_height);
    auto dx_max = ww/2;
    auto dx_min = w-ww/2;
    auto dy_max = hh/2;
    auto dy_min = h-hh/2;
    auto ddx = std::min(std::max(dx_min,dx()),dx_max);
    auto ddy = std::min(std::max(dy_min,dy()),dy_max);
    {
        std::scoped_lock<std::shared_mutex> lock(mutex);
        _ = QTransform(s,0,0,-s,ddx,ddy);
    }
}
