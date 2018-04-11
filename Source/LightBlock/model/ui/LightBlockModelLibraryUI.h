/*
  ==============================================================================

    LightBlockLibraryUI.h
    Created: 10 Apr 2018 10:58:08pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../LightBlockModelLibrary.h"
#include "LightBlockModelManagerUI.h"
#include "LightBlockModelGroupUI.h"

class LightBlockModelLibraryUI :
	public ShapeShifterContentComponent
{
public:
	LightBlockModelLibraryUI(const String &contentName, LightBlockModelLibrary * library);
	~LightBlockModelLibraryUI();

	LightBlockModelLibrary * library;
	LightBlockModelGroupUI genericGroupUI;
	LightBlockModelGroupUI liveFeedGroupUI;
	LightBlockModelManagerUI userModelManagerUI;

	void paint(Graphics &g) override;
	void resized() override;

	static LightBlockModelLibraryUI * create(const String &contentName) { return new LightBlockModelLibraryUI(contentName, LightBlockModelLibrary::getInstance()); }
};