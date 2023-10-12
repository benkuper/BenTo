/*
  ==============================================================================

    EmbeddedScriptManagerUI.h
    Created: 7 Nov 2021 11:16:05am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../EmbeddedScriptManager.h"

class EmbeddedScriptManagerUI :
    public BaseManagerShapeShifterUI<EmbeddedScriptManager, EmbeddedScript, BaseItemUI<EmbeddedScript>>
{
public:
    EmbeddedScriptManagerUI();
    ~EmbeddedScriptManagerUI();


    static EmbeddedScriptManagerUI* create(const String& name) { return new EmbeddedScriptManagerUI(); }
};