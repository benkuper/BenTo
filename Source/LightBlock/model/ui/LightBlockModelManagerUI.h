/*
  ==============================================================================

    LightBlockModelManagerUI.h
    Created: 10 Apr 2018 7:56:53pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../UserLightBlockModelManager.h"
#include "LightBlockModelUI.h"

class LightBlockModelManagerUI :
	public BaseManagerShapeShifterUI<UserLightBlockModelManager, LightBlockModel, LightBlockModelUI>
{
public:
	LightBlockModelManagerUI(const String &name, UserLightBlockModelManager * manager);
	~LightBlockModelManagerUI();
};