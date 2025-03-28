#pragma once
#include "datamap.h"
class Algm{
public:
    Algm() = default;
    virtual void process(DataMap& data) = 0;
};
