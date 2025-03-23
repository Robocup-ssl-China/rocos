#ifndef THEME_H
#define THEME_H
#include "zos/utils/singleton.h"
#include "yamlparams.h"
#include <yaml-cpp/yaml.h>
#include <QColor>
#include <array>

class ThemeColor{
public:
    ThemeColor();
    std::array<QColor,2> ROBOT;
    QColor ROBOT_DIR;
    QColor ROBOT_CIRCLE;
    QColor BALL;
    QColor BALL_FOCUS;
    std::array<QColor,10> DEBUG_MSG;
    QColor DEBUG_BRUSH_COLOR;
    QColor BACKGROUND;
    QColor FIELDLINE;
};
typedef Singleton<ThemeColor> Color;
#endif // THEME_H
