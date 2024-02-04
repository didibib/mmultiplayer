#pragma once

#include "../addon.h"

class Logging : public Addon {
    public:
        bool Initialize();
        std::string GetName();

};