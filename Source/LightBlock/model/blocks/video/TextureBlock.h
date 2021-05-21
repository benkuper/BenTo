/*
  ==============================================================================

	TextureBlock.h
	Created: 10 Apr 2018 6:58:49pm
	Author:  Ben

  ==============================================================================
*/

#pragma once

class Spatializer;

class TextureBlock :
	public LightBlockModel,
	public BaseManager<SpatLayout>::ManagerListener
{
public:
	TextureBlock(const String & name= "", var params = var());
	~TextureBlock();

	EnumParameter* currentLayout;
	BoolParameter* inputIsLive;

	void clear() override;
	void setSelectedInternal(bool value) override;

	virtual Image getImage() { return Image(); }

	void updateLayoutOptions();

	Array<Colour> getColors(Prop* p, double time, var params) override;

	void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;

	void itemAdded(SpatLayout*) override;
	void itemRemoved(SpatLayout*) override;

	//Listener
	class  TextureListener
	{
	public:
		/** Destructor. */
		virtual ~TextureListener() {}
		virtual void textureUpdated(TextureBlock*) {}
		virtual void connectionChanged(TextureBlock*) {}
	};

	ListenerList<TextureListener> textureListeners;
	void addTextureListener(TextureListener* newListener) { textureListeners.add(newListener); }
	void removeTextureListener(TextureListener* listener) { textureListeners.remove(listener); }
};
