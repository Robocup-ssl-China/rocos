#ifndef __FIELD_CONFIG_H__
#define __FIELD_CONFIG_H__
#include "zos/utils/singleton.h"
#include <shared_mutex>
#include <QTransform>
class FieldParam{
public:
    FieldParam();
    ~FieldParam();
    void reload();
public:
    int p_margin; // param field margin
    int p_width; // param_width
    int p_height; // param_height
    int p_canvas_width; // param_canvas_width
    int p_canvas_height; // param_canvas_height
    int p_goalWidth; // param_goal_width
    int p_goalDepth; // param_goal_depth
    int p_penaltyWidth; // param_penalty_width
    int p_penaltyLength; // param_penalty_length
    int p_centerCircleRadius; // param_center_circle_radius
    int p_lineWidth; // param_line_width
    int s_ballDiameter; // size_ball_diameter
    int s_shadowDiameter; // size_shadow_diameter
    int s_carDiameter; // size_robot_diameter
    int s_carFaceWidth; // size_face_width
    int s_number; // size_number_text
    int s_debugPoint; // size_debug_point
};
typedef Singleton<FieldParam> FP;

struct FieldTransform{
    std::shared_mutex mutex;
    QTransform _;
    void update(double,double);
    double scale();
    double dx();
    double dy();
    double s(double);
    double a(double);
    void limit(double,double);
};
#endif // __FIELD_CONFIG_H__
