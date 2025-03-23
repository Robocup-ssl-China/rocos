#ifndef __ZOS_CONFIG_H__
#define __ZOS_CONFIG_H__
#include <chrono>
namespace zos{
namespace config{
    using namespace std::chrono_literals;
    constexpr auto timestep = 50ms;
    constexpr int threadpool_nums = 8;
}// zos::config
constexpr auto __timestep = config::timestep;
}// zos
#endif // __ZOS_CONFIG_H__
