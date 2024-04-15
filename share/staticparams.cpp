#include "staticparams.h"
#include "parammanager.h"

auto zpm = ZSS::ZParamManager::instance();

namespace PARAM {
namespace Field {
const double PITCH_LENGTH = zpm->value("field/width",QVariant(9000)).toFloat();
const double PITCH_WIDTH = zpm->value("field/height",QVariant(6000)).toFloat();
const double PENALTY_AREA_WIDTH = zpm->value("field/penaltyLength",QVariant(2000)).toFloat();
const double PENALTY_AREA_DEPTH = zpm->value("field/penaltyWidth",QVariant(1000)).toFloat();
const double GOAL_WIDTH = zpm->value("field/goalWidth",QVariant(1000)).toFloat();
const double GOAL_DEPTH = zpm->value("field/goalDepth",QVariant(200)).toFloat();
}
}