#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include "taskflow/taskflow.hpp"
#include "algm_registry.h"
#include "VisionTypeDef.h"
#include "Semaphore.h"
#include "singleton.hpp"
class AlgmManager{
public:
    AlgmManager() = default;
    ~AlgmManager() = default;
    void init();
    void signal(const VisualInfoT&, const RefRecvMsg&);
private:
    void _step();
    std::unordered_map<std::string, std::unique_ptr<Algm>> algms_;
    std::unordered_map<std::string, tf::Task> algm_tasks_;
    tf::Taskflow taskflow_;
    tf::Executor executor_;
    DataMap data_map_, input_swap_, output_swap_;
    std::mutex input_mutex_, output_mutex_;
    Semaphore signal_to_step_;
    std::thread _step_thread_;
};
typedef Singleton< AlgmManager > AlgmMnger;
