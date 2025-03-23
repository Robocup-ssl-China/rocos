#include "themecolor.h"
#include <fstream>
#include "staticparams.h"
ThemeColor::ThemeColor(){
    ZSS::Theme::_()->loadParam(ROBOT[PARAM::BLUE],"field/robot_blue",QColor("#3e8ed0"));
    ZSS::Theme::_()->loadParam(ROBOT[PARAM::YELLOW],"field/robot_yellow",QColor("#f1d483"));
    ZSS::Theme::_()->loadParam(ROBOT_DIR,"field/robot_dir",QColor(220,53,47));
    ZSS::Theme::_()->loadParam(ROBOT_CIRCLE,"field/robot_circle",QColor(0,137,167));
    ZSS::Theme::_()->loadParam(BALL,"field/ball",QColor(247,91,0));
    ZSS::Theme::_()->loadParam(BALL_FOCUS,"field/ballFocus",QColor("#E66045"));
    ZSS::Theme::_()->loadParam(BACKGROUND,"field/background",QColor("#292929"));
    ZSS::Theme::_()->loadParam(FIELDLINE,"field/fieldlines",QColor("#959595"));
    ZSS::Theme::_()->loadParam(DEBUG_MSG,"debug/color",{
        QColor("#DDFFCC99"),// white
        QColor("#DDE62C29"),// red
        QColor("#DDE69929"),// orange
        QColor("#DDE6E429"),// yellow
        QColor("#DD16E070"),// green
        QColor("#DD078BE6"),// cran
        QColor("#DD2936E6"),// blue
        QColor("#DD7D1EE6"),// purple
        QColor("#DD997900"),// gray
        QColor("#DD5E3D80") // black
    });
    ZSS::Theme::_()->loadParam(DEBUG_BRUSH_COLOR,"debug/brush",QColor(255,255,255,20));
}
