#include "BallPredictor.h"
#include <utils.h>
#include <fstream>
#include <istream>
#include "geometry.h"
#include "WorldDefine.h"
#include "GDebugEngine.h"
#include "parammanager.h"

void CBallPredictor::updateVision( const VisualInfoT& vInfo, bool invert) {
    // 每次处理一个_visionLogger里的数据，只用来保留预测输出
    // 该函数的处理结果就是更新thisCycle帧里面的球信息
    BallVisionData& thisCycle = _visionLogger.getVision(vInfo.cycle);
    const int invertFactor = invert ? -1 : 1;

    thisCycle.SetVel(vInfo.ball.vel * invertFactor);
    thisCycle.SetRawPos(vInfo.ball.rawPos.x * invertFactor, vInfo.ball.rawPos.y * invertFactor);
    thisCycle.SetChipPredict(vInfo.ball.chipPredict.x * invertFactor, vInfo.ball.chipPredict.y * invertFactor);
    thisCycle.SetPos(vInfo.ball.pos.x * invertFactor,  vInfo.ball.pos.y * invertFactor);
    thisCycle.SetValid(vInfo.ball.valid);
    thisCycle.cycle =  vInfo.cycle;
    return;
}