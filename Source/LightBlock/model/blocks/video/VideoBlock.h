/*
  ==============================================================================

	VideoBlock.h
	Created: 10 Apr 2018 6:58:49pm
	Author:  Ben

  ==============================================================================
*/

#pragma once

class Spatializer;

class VideoBlock :
	public LightBlockModel,
	public Manager<SpatLayout>::ManagerListener
{
public:
	VideoBlock(const String & name= "", var params = var());
	~VideoBlock();

	EnumParameter* currentLayout;
	BoolParameter* inputIsLive;

	void clear() override;
	void setSelectedInternal(bool value) override;

	virtual Image getImage() { return Image(); }

	void updateLayoutOptions();

	Array<Colour> getColors(Prop* p, double time, var params) override;

	void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;

	void itemAdded(SpatLayout*) override;
	void itemsAdded(Array<SpatLayout*>) override;
	void itemRemoved(SpatLayout*) override;
	void itemsRemoved(Array<SpatLayout*>) override;

	//Listener
	class  TextureListener
	{
	public:
		/** Destructor. */
		virtual ~TextureListener() {}
		virtual void textureUpdated(VideoBlock*) {}
		virtual void connectionChanged(VideoBlock*) {}
	};

	ListenerList<TextureListener> textureListeners;
	void addTextureListener(TextureListener* newListener) { textureListeners.add(newListener); }
	void removeTextureListener(TextureListener* listener) { textureListeners.remove(listener); }
};
