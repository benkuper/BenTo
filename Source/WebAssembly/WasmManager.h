/*
  ==============================================================================

    WasmManager.h
    Created: 7 Nov 2021 11:15:18am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "WasmScript.h"

class WasmManager :
    public BaseManager<WasmScript>
{
public:
    juce_DeclareSingleton(WasmManager, true);
    WasmManager();
    ~WasmManager();
};