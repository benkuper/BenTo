/*
  ==============================================================================

    Spatializer.h
    Created: 23 Apr 2018 9:42:35pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "SpatLayout.h"
#include "SpatializerEvent.h"

class Spatializer :
	public BaseManager<SpatLayout>
{
public:
	Spatializer();
	~Spatializer();

	//ui
	FloatParameter * textureOpacity;
	BoolParameter * showHandles;
	BoolParameter * showPixels;

	SpatLayout * currentLayout;
	void setCurrentLayout(SpatLayout * newLayout);

	bool isInit;

	//gl
	std::unique_ptr<LowLevelGraphicsContext> context;
	OpenGLFrameBuffer fbo;
	OpenGLImageType fboImage;
	OpenGLGraphicsContextCustomShader shader;
	
	void init();
	void computeSpat(Image tex, SpatLayout * forceLayout = nullptr);

	SpatItem * getItemForProp(Prop * p, SpatLayout * forceLayout = nullptr);

	// ASYNC
	QueuedNotifier<SpatializerEvent> spatNotifier;
	void addAsyncSpatListener(SpatializerAsyncListener* newListener) { spatNotifier.addListener(newListener); }
	void addAsyncCoalescedSpatListener(SpatializerAsyncListener* newListener) { spatNotifier.addAsyncCoalescedListener(newListener); }
	void removeAsyncSpatListener(SpatializerAsyncListener* listener) { spatNotifier.removeListener(listener); }
};