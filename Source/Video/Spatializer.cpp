/*
  ==============================================================================

    Spatializer.cpp
    Created: 23 Apr 2018 9:42:35pm
    Author:  Ben

  ==============================================================================
*/

#include "Spatializer.h"
#include "LightBlock/model/LightBlockModelLibrary.h"

juce_ImplementSingleton(Spatializer)

String frag =
"uniform int inverse; \
uniform int sourceSize; \
uniform sampler2D sourceTex; \
uniform vec2 pixMap[512]; \
 \
void main() \
{ \
    vec2 tcoord = pixMap[pixelPos.x + pixelPos.y*64]; \
    if(inverse == 1) tcoord.y = 1-tcoord.y; \
    gl_FragColor = pixelAlpha * texture2D(sourceTex,tcoord); \
}";



Spatializer::Spatializer() :
	BaseManager("Spatializer"),
	videoBlock(nullptr),
	isInit(false),
	shader(frag)
{
	showTexture = addBoolParameter("Show Texture", "", true);
	showHandles = addBoolParameter("Show Handles", "", true);
	showPixels = addBoolParameter("Show Pixels", "", true);

	setVideoBlock(static_cast<VideoBlock *>(LightBlockModelLibrary::getInstance()->videoBlock.get()));
}

Spatializer::~Spatializer()
{
	setVideoBlock(nullptr);
}


void Spatializer::setVideoBlock(VideoBlock * vb)
{
	if (videoBlock == vb) return;

	if (videoBlock != nullptr)
	{
		videoBlock->removeInspectableListener(this);
		videoBlock->removeVideoListener(this);
		videoBlockRef = nullptr;

	}

	videoBlock = vb;

	if (videoBlock != nullptr)
	{
		videoBlock->addInspectableListener(this);
		videoBlock->addVideoListener(this);
		videoBlockRef = videoBlock;
	}
}

void Spatializer::init()
{
	fbo.initialise(*OpenGLContext::getCurrentContext(), 256, 256);

	context = createOpenGLGraphicsContext(*OpenGLContext::getCurrentContext(), fbo);
	
	isInit = true;
}

void Spatializer::computeSpat()
{
	if (videoBlock == nullptr || !videoBlock->inputIsLive->boolValue()) return;

	if (!isInit) init();

	//Later, will only have to check Fbo's output and assign in order

	//OpenGLShaderProgram * program = shader.getProgram(*context);
	//program->setUniform("sourceTex", pointCoords, pointCoords.size());
	//program->setUniform("pixMap", pointCoords, pointCoords.size());

	Image tex = videoBlock->receiver->getImage();
	
	
	const Image::BitmapData data(tex, 0,0, tex.getWidth(), tex.getHeight());

	for (auto &si : items)
	{
		int numPoints = si->resolution->intValue();
		for (int i = 0; i < numPoints; i++)
		{
			si->colors.set(i, data.getPixelColour(jlimit<int>(0,tex.getWidth()-1,si->points[i].x*(tex.getWidth()-1)), jlimit<int>(0,tex.getHeight()-1,si->points[i].y*(tex.getHeight()-1))));
		}
	}
	
}

SpatItem * Spatializer::getItemForProp(Prop * p)
{
	SpatItem * defaultSI = nullptr;
	for (auto &si : items)
	{
		int id = si->filterManager.getTargetIDForProp(p);
		if(id >= 0) return si;

		if (si->isDefault->boolValue() && defaultSI != nullptr) defaultSI = si;
	}

	return defaultSI;
}

void Spatializer::inspectableDestroyed(Inspectable * i)
{
	if (i == videoBlock) setVideoBlock(nullptr);
}

void Spatializer::textureUpdated(VideoBlock *)
{
	computeSpat();
}
