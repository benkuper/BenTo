/*
  ==============================================================================

	LightBlockClipManager.cpp
	Created: 17 Apr 2018 5:11:44pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlockClipManager.h"
#include "LightBlockLayer.h"

LightBlockClipManager::LightBlockClipManager(LightBlockLayer * layer) :
	LayerBlockManager(layer),
	lightBlockLayer(layer)
{
	itemDataType = "LightBlockClip";
	blocksCanOverlap = true;
}


LightBlockClipManager::~LightBlockClipManager()
{
}

LayerBlock * LightBlockClipManager::createItem()
{
	return new LightBlockClip(lightBlockLayer);
}

void LightBlockClipManager::addItemInternal(LayerBlock* block, var data)
{
	LayerBlockManager::addItemInternal(block, data);
	LightBlockClip* clip = dynamic_cast<LightBlockClip*>(block);
	clip->addClipListener(this);
}

void LightBlockClipManager::removeItemInternal(LayerBlock* block)
{
	LayerBlockManager::removeItemInternal(block);
	LightBlockClip* clip = dynamic_cast<LightBlockClip*>(block);
	clip->removeClipListener(this);
}

void LightBlockClipManager::onControllableFeedbackUpdate(ControllableContainer * cc, Controllable * c)
{
	LightBlockClip * b = c->getParentAs<LightBlockClip>();
	if (b != nullptr)
	{
		if (c == b->time || c == b->coreLength || c == b->loopLength)
		{
			if (!blocksCanOverlap) return;
			computeFadesForClip(b, true);
		}
	}
}

void LightBlockClipManager::clipFadesChanged(LightBlockClip* clip)
{
	computeFadesForClip(clip, false);
}

void LightBlockClipManager::computeFadesForClip(LightBlockClip * clip, bool propagate)
{
	int bIndex = items.indexOf(clip);
	
	int prevIndex = bIndex - 1;
	LightBlockClip* prevBlock = nullptr;
	while (prevBlock == nullptr && prevIndex >= 0)
	{
		LightBlockClip* b = (LightBlockClip*)items[prevIndex];
		if (b->enabled->boolValue()) prevBlock = b;
		prevIndex--;
	}

	int nextIndex = bIndex + 1;;
	LightBlockClip* nextBlock = nullptr; 
	while (nextBlock == nullptr && nextIndex < items.size())
	{
		LightBlockClip * b = (LightBlockClip*)items[nextIndex];
		if (b->enabled->boolValue()) nextBlock = b;
		nextIndex++;
	}

	if (prevBlock != nullptr && prevBlock->time->floatValue() > clip->time->floatValue())
	{
		reorderItems();
		computeFadesForClip(clip, propagate);
		return;
	}

	if (nextBlock != nullptr && nextBlock->time->floatValue() < clip->time->floatValue())
	{
		reorderItems();
		computeFadesForClip(clip, propagate);
		return;
	}

	if (!clip->fadeIn->enabled)
	{
		float fadeIn = prevBlock == nullptr ? 0 : jmax(prevBlock->getEndTime() - clip->time->floatValue(), 0.f);
		clip->fadeIn->setValue(fadeIn);
	}
	
	if (!clip->fadeOut->enabled)
	{
		float fadeOut = nextBlock == nullptr ? 0 : jmax(clip->getEndTime() - nextBlock->time->floatValue(), 0.f);
		clip->fadeOut->setValue(fadeOut);
	}

	if (propagate)
	{
		if (prevBlock != nullptr) computeFadesForClip(prevBlock, false);
		if (nextBlock != nullptr) computeFadesForClip(nextBlock, false);
	}
}