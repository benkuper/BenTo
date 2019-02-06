/*
  ==============================================================================

    VideoBlock.h
    Created: 10 Apr 2018 6:58:49pm
    Author:  Ben

  ==============================================================================
*/

#pragma once


#include "../../LightBlockModel.h"
#include "Video/Spatializer.h"

class VideoBlock :
	public LightBlockModel,
	public SharedTextureReceiver::Listener,
	public BaseManager<SpatLayout>::Listener
{
public:
	VideoBlock(var params = var());
	~VideoBlock();

	StringParameter * textureName;
	SharedTextureReceiver * receiver;

	Spatializer spat;

	EnumParameter * currentLayout;
	BoolParameter * inputIsLive;

	void setupReceiver();
	Image getImage();

	void updateLayoutOptions();

	Array<Colour> getColors(Prop * p, double time, var params) override;

	void onControllableFeedbackUpdateInternal(ControllableContainer *cc, Controllable *c) override;

	// Inherited via Listener
	virtual void textureUpdated(SharedTextureReceiver *) override;
	virtual void connectionChanged(SharedTextureReceiver *) override;

	void itemAdded(SpatLayout *) override;
	void itemRemoved(SpatLayout *) override;

	void clear() override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	//Listener
	class  VideoListener
	{
	public:
		/** Destructor. */
		virtual ~VideoListener() {}
		virtual void textureUpdated(VideoBlock *) {}
		virtual void connectionChanged(VideoBlock *) {}
	};

	ListenerList<VideoListener> videoListeners;
	void addVideoListener(VideoListener* newListener) { videoListeners.add(newListener); }
	void removeVideoListener(VideoListener* listener) { videoListeners.remove(listener); }


	void onContainerParameterChangedInternal(Parameter * p) override;

	LightBlockModelUI * createUI() override;

	String getTypeString() const override { return "Video"; }

	static VideoBlock * create(var params) { return new VideoBlock(params); }
};
