#pragma once

#include "output/RGBLedsManager.h"

class LedManager :
    public Component
{
public:
    LedManager();
    ~LedManager(){}

    RGBLedsManager rgbManager;

    void init();
    void update();
};