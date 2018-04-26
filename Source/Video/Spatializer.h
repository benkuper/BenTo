/*
  ==============================================================================

    Spatializer.h
    Created: 23 Apr 2018 9:42:35pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "LightBlock/model/blocks/video/VideoBlock.h"
#include "SpatItem.h"

class Spatializer :
	public BaseManager<SpatItem>,
	public Inspectable::InspectableListener,
	public VideoBlock::VideoListener
{
public:
	juce_DeclareSingleton(Spatializer, true);

	Spatializer();
	~Spatializer();

	//ui
	BoolParameter * showTexture;
	BoolParameter * showHandles;
	BoolParameter * showPixels;

	VideoBlock * videoBlock;
	WeakReference<Inspectable> videoBlockRef;

	bool isInit;

	//gl
	LowLevelGraphicsContext * context;
	OpenGLFrameBuffer fbo;
	OpenGLImageType fboImage;
	OpenGLGraphicsContextCustomShader shader;

	void setVideoBlock(VideoBlock * vb);

	void init();
	void computeSpat();

	SpatItem * getItemWithPropID(int id);

	void inspectableDestroyed(Inspectable *) override;
	void textureUpdated(VideoBlock *) override;
};