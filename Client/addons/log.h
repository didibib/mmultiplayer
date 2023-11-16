#pragma once

#include "../addon.h"

class Log : public Addon {
  public:
    bool Initialize();
    std::string GetName();
};