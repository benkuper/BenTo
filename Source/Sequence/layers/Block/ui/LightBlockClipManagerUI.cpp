/*
  ==============================================================================

	LightBlockClipManagerUI.cpp
	Created: 17 Apr 2018 7:20:49pm
	Author:  Ben

  ==============================================================================
*/

LightBlockClipManagerUI::LightBlockClipManagerUI(LightBlockLayerTimeline * _timeline, LightBlockClipManager * manager) :
	LayerBlockManagerUI(_timeline, manager),
	clipManager(manager),
	timeline(_timeline),
	dropClipX(-1)
{
	acceptedDropTypes.add("LightBlockModel");
	acceptedDropTypes.add("Script");
	acceptedDropTypes.add("Timeline");
	acceptedDropTypes.add("Picture");
	acceptedDropTypes.add("Node");

	addExistingItems();
}

LightBlockClipManagerUI::~LightBlockClipManagerUI()
{
}

void LightBlockClipManagerUI::paint(Graphics & g)
{
	LayerBlockManagerUI::paint(g);
	if (dropClipX >= 0)
	{
		g.fillAll(BLUE_COLOR.withAlpha(.3f));
		g.setColour(HIGHLIGHT_COLOR);
		g.drawLine(dropClipX, 0, dropClipX, getHeight(), 4);
	}
}

LayerBlockUI * LightBlockClipManagerUI::createUIForItem(LayerBlock * item)
{
	return new LightBlockClipUI((LightBlockClip *)item);
}


void LightBlockClipManagerUI::itemDragEnter(const SourceDetails & source)
{
	dropClipX = source.localPosition.x;
	repaint();
}

void LightBlockClipManagerUI::itemDragExit(const SourceDetails & source)
{
	dropClipX = -1;
	repaint();
}

void LightBlockClipManagerUI::itemDragMove(const SourceDetails & source)
{
	dropClipX = source.localPosition.x;
	repaint();
}

void LightBlockClipManagerUI::itemDropped(const SourceDetails & source)
{
	dropClipX = -1;

	LightBlockModelUI * modelUI = dynamic_cast<LightBlockModelUI *>(source.sourceComponent.get());
	LightBlockClip * clip = (LightBlockClip *)manager->addBlockAt(timeline->getTimeForX(source.localPosition.x));
	if (modelUI == nullptr || clip == nullptr) return;

	LightBlockColorProvider * provider = modelUI->item;

	bool shift = KeyPress::isKeyCurrentlyDown(16);
	if (shift)
	{
		PopupMenu m;
		m.addItem(-1, "Default");
		m.addSeparator();
		int index = 1;
		for (auto &p : modelUI->item->presetManager.items) m.addItem(index++, p->niceName);
		m.showMenuAsync(PopupMenu::Options(), [this, modelUI, clip](int result)
			{
				if (result > 0)
				{
					LightBlockColorProvider* provider = modelUI->item->presetManager.items[result - 1];
					if (LightBlockFilter* f = dynamic_cast<LightBlockFilter*>(provider)) clip->addEffectFromProvider(f);
					else clip->activeProvider->setValueFromTarget(provider);
				}
			}
		);
	}
	else
	{
		if (LightBlockFilter* f = dynamic_cast<LightBlockFilter*>(provider)) clip->addEffectFromProvider(f);
		else clip->activeProvider->setValueFromTarget(provider);
	}
}
