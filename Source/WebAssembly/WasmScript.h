/*
  ==============================================================================

    WasmScript.h
    Created: 7 Nov 2021 11:15:33am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class WasmScript :
    public BaseItem
{
public:
    WasmScript();
    ~WasmScript();

    enum CompileType {COMPILE_DEBUG, COMPILE_OPTIMIZED,COMPILE_TINY};
    FileParameter* scriptFile;
    EnumParameter* compileType;
    BoolParameter* lowMemory;
    Trigger* compileTrigger;
    Trigger* uploadToPropsTrigger;
    Trigger* launchOnPropsTrigger;


    void compile();

    File getWasmFile();

    void onContainerTriggerTriggered(Trigger* t) override;
};