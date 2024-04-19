/*
  ==============================================================================

    EmbeddedScript.h
    Created: 7 Nov 2021 11:15:33am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

//#include "JuceHeader.h"
//
//class EmbeddedScript :
//    public BaseItem,
//    public Thread
//{
//public:
//    EmbeddedScript();
//    ~EmbeddedScript();
//
//    enum CompileType {COMPILE_DEBUG, COMPILE_OPTIMIZED,COMPILE_TINY};
//    FileParameter* scriptFile;
//    EnumParameter* compileType;
//    BoolParameter* lowMemory;
//    Trigger* compileTrigger;
//    Trigger* uploadToPropsTrigger;
//    Trigger* loadOnPropsTrigger;
//
//    BoolParameter* autoCompile;
//    BoolParameter* autoUpload;
//    BoolParameter* autoLaunch;
//    Trigger* stopOnPropsTrigger;
//    Time lastModTime;
//
//    void checkAutoCompile();
//    void compile();
//
//    File getWasmFile();
//
//    void run() override;
//
//    void onContainerParameterChangedInternal(Parameter* p) override;
//    void onContainerTriggerTriggered(Trigger* t) override;
//};