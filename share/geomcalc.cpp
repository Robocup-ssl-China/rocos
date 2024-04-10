#include <cmath>
#include "geomcalc.h"
double angleDiff(double angle1, double angle2){
    return std::atan2(std::sin(angle2 - angle1), std::cos(angle2 - angle1));
}