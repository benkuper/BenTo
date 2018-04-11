/*
  ==============================================================================

	LightBlockModelGroupUI.cpp
	Created: 10 Apr 2018 11:17:09pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlockModelGroupUI.h"

LightBlockModelGroupUI::LightBlockModelGroupUI(ControllableContainer * group) :
	group(group)
{
	for (auto &cc : group->controllableContainers)
	{
		LightBlockModel * m = dynamic_cast<LightBlockModel *>(cc.get());
		if (m == nullptr) continue;
		LightBlockModelUI * mui = new LightBlockModelUI(m);
		modelsUI.add(mui);
		addAndMakeVisible(mui);
	}

	setSize(128, 128);
}

LightBlockModelGroupUI::~LightBlockModelGroupUI()
{
}

void LightBlockModelGroupUI::paint(Graphics & g)
{
}

void LightBlockModelGroupUI::resized()
{
	//grid
}
