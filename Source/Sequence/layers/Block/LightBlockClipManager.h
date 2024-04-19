/*
  ==============================================================================

    LightBlockClipManager.h
    Created: 17 Apr 2018 5:11:44pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class LightBlockLayer;

class LightBlockClipManager :
	public LayerBlockManager,
	public LightBlockClip::ClipListener
{
public:
	LightBlockClipManager(LightBlockLayer * layer);
	~LightBlockClipManager();

	LightBlockLayer * lightBlockLayer;
	LayerBlock * createItem() override;

	void addItemInternal(LayerBlock* block, var) override;
	void addItemsInternal(Array<LayerBlock*> blocks, var) override;
	void removeItemInternal(LayerBlock* block) override;
	void removeItemsInternal(Array<LayerBlock*> blocks) override;

	void onControllableFeedbackUpdate(ControllableContainer * cc, Controllable *c) override;

	void clipFadesChanged(LightBlockClip* clip) override;
	void computeFadesForClip(LightBlockClip * clip, bool propagate);
};