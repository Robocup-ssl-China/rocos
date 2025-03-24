#include "skill_registry.h"
#include <torch/script.h>
#include <fmt/core.h>
#include "CommandFactory.h"
#include "parammanager.h"
#include <GDebugEngine.h>
#include <chrono>

#include "tbk/tbk.h"

class NNGoto : public Skill{
public:
    NNGoto();
    virtual CPlayerCommand* execute(const CVisionModule* pVision) override;
    virtual void toStream(std::ostream& os) const override {os << "NNGoto";}
private:
    torch::jit::script::Module __module;
    tbk::Publisher __pub;
    tbk::Subscriber<3> __sub;
};
REGISTER_SKILL(NNGoto, NNGoto);

NNGoto::NNGoto():__pub("NNGoto","NNGoto_Vision"),__sub("NNGoto","NNGoto_Cmd"){
    QString modulePath;
    ZSS::ZParamManager::instance()->loadParam(modulePath,"NNModule/Goto","model/NNGoto.pt");
    try{
        __module = torch::jit::load(modulePath.toStdString());
        __module.eval();
        std::cout << typeid(__module).name() << std::endl;
        for( const auto& param : __module.named_parameters() ){
            std::cout << param.name << ": " << param.value.sizes() << std::endl;
        }
        // python : 
        //     x = torch.rand(3,4);
        //     h = torch.rand(3,4);
        //     y = __module(x,h);
        //     print(y);
        // c++ :
        // torch::Tensor x = torch::rand({3,4});
        // torch::Tensor h = torch::zeros({3,4});
        // auto outputs = __module.forward({x,h}).toTuple().get()->elements();
        // for(int i=0;i<100;i++){
        //     torch::Tensor x = torch::rand({3,4});
        //     torch::Tensor h = torch::zeros({3,4});
        //     auto outputs = __module.forward({x,h}).toTuple().get()->elements();
        //     // std::cout << "outputs : " << typeid(outputs).name() << std::endl;
        //     // torch::Tensor y = outputs[0].toTensor();
        //     // std::cout << "x : " << x << "\ny : " << y << std::endl;
        // }
    }catch(const c10::Error& e){
        GDebugEngine::Instance()->keep_warning("Error loading the module - "+modulePath.toStdString());
        std::cout << e.what() << std::endl;
    }
}
/***************
 * Observations : original:self.rawPos
 * 1. self.vel + self.rawVel (2+2d)
 * 2. ball.rawPos (2d)
 * 3. ball.vel + ball.rawVel (2+2d)
 * 4. last ball.rawPos (2d)
 * 5. last ball.vel + last ball.rawVel (2+2d)
 * 6. target.pos (2d)
 * 
 * Actions :
 * 1. self.acc (2d) (-1,1)
 * 2. self.rotAcc (1d) (-1,1)
*/

CPlayerCommand* NNGoto::execute(const CVisionModule* pVision){
    static tbk::Data recvData;
    static int count = 0;
    const int vecNumber = task().executor;
    std::string s = fmt::format("NNGoto:{}",count++);
    __pub.publish(s.c_str(),s.size());
    auto res = __sub.try_get(recvData);
    if (res){
        std::string recvDataStr((const char*)(recvData.data()),recvData.size());
        std::cout << "sentString : " << s << "\n";
        std::cout << "recvString : " << recvDataStr << std::endl;
    }else{
        std::cout << "no data" << std::endl;
    }
    return CmdFactory::Instance()->newCommand(CPlayerSpeedV2(vecNumber, 0, 0, 1, 0));
}