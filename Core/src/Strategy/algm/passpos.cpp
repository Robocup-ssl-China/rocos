#include "algm_registry.h"
#include "fmt/core.h"
#include <thread>
#include <string>
class PassPosCalculate: public Algm{
public:
    PassPosCalculate() = default;
    void process(DataMap& data) override;
};
REGISTER_ALGM(PassPosCalculate, PassPosCalculate);

void PassPosCalculate::process(DataMap& data){
    fmt::print("PassPosCalculate process\n");
    std::this_thread::sleep_for(std::chrono::seconds(1));
}