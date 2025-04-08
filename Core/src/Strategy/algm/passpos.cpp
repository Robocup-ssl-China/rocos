#include <thread>
#include "fmt/core.h"
#include "algm_registry.h"
#include "VisionTypeDef.h"
#include "GDebugEngine.h"
class PassPosCalculate: public Algm{
public:
    PassPosCalculate() = default;
    void process(DataMap& data) override;
};
REGISTER_ALGM(PassPosCalculate, PassPosCalculate);

void PassPosCalculate::process(DataMap& data){
    fmt::print("PassPosCalculate::process\n");
    auto vision = data.get_as<VisualInfoT>("vision");
    auto& our_player = vision.player[VisualInfoT::ME];
    auto& oppo_player = vision.player[VisualInfoT::OPPONENT];
    auto& ball = vision.ball;

    for(int i = 0; i < PARAM::Field::MAX_PLAYER; ++i){
        if(our_player[i].valid){
            GDebugEngine::Instance()->gui_debug_x(our_player[i].pos, COLOR_GREEN, 0, 10);
            GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(our_player[i].pos) + CVector(50, 50), fmt::format("Player {}: ({:.1f}, {:.1f})", i, our_player[i].pos.x, our_player[i].pos.y), COLOR_RED, 0, 10, 30);
        }
        if(oppo_player[i].valid){
            GDebugEngine::Instance()->gui_debug_x(oppo_player[i].pos, COLOR_RED, 0, 10);
            GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(oppo_player[i].pos) + CVector(50, 50), fmt::format("Opponent {}: ({:.1f}, {:.1f})", i, oppo_player[i].pos.x, oppo_player[i].pos.y), COLOR_BLUE, 0, 10, 30);
        }
    }
    if (ball.valid){
        GDebugEngine::Instance()->gui_debug_x(ball.pos, COLOR_YELLOW, 0, 10);
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(ball.pos) + CVector(50, 50), fmt::format("Ball: ({:.1f}, {:.1f})", ball.pos.x, ball.pos.y), COLOR_YELLOW, 0, 10, 30);
    }
    GHeatmapEngine::instance()->gui_debug_heat(ball.pos.x, ball.pos.y, 1.0f, 1000);
}