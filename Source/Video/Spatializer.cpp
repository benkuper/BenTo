/*
  ==============================================================================

    Spatializer.cpp
    Created: 23 Apr 2018 9:42:35pm
    Author:  Ben

  ==============================================================================
*/

#include "Spatializer.h"
#include "LightBlock/model/LightBlockModelLibrary.h"
#include "LightBlock/model/blocks/video/VideoBlock.h"


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
	currentLayout(nullptr),
	isInit(false),
	shader(frag),
	spatNotifier(10)
{
	saveAndLoadRecursiveData = true;

	textureOpacity = addFloatParameter("Texture Opacity", "Opacity of the background texture",1,0,1);
	showHandles = addBoolParameter("Show Handles", "", true);
	showPixels = addBoolParameter("Show Pixels", "", true);
	selectItemWhenCreated = false;
}

Spatializer::~Spatializer()
{
}

void Spatializer::setCurrentLayout(SpatLayout * newLayout)
{
	if (currentLayout == newLayout) return;
	currentLayout = newLayout;
	spatNotifier.addMessage(new SpatializerEvent(SpatializerEvent::LAYOUT_CHANGED));
}


void Spatializer::init()
{
	fbo.initialise(*OpenGLContext::getCurrentContext(), 256, 256);

	context = createOpenGLGraphicsContext(*OpenGLContext::getCurrentContext(), fbo);
	
	isInit = true;
}

void Spatializer::computeSpat(Image &tex, SpatLayout * forceLayout)
{
	SpatLayout * targetLayout = forceLayout != nullptr ? forceLayout : currentLayout;
	if (targetLayout == nullptr) return;

	if (!isInit) init();

	//Later, will only have to check Fbo's output and assign in order

	//OpenGLShaderProgram * program = shader.getProgram(*context);
	//program->setUniform("sourceTex", pointCoords, pointCoords.size());
	//program->setUniform("pixMap", pointCoords, pointCoords.size());

	const Image::BitmapData data(tex, 0,0, tex.getWidth(), tex.getHeight());

	for (auto &si : targetLayout->spatItemManager.items)
	{
		int numPoints = si->resolution->intValue();
		for (int i = 0; i < numPoints; i++)
		{
			si->colors.set(i, data.getPixelColour(jlimit<int>(0,tex.getWidth()-1,si->points[i].x*(tex.getWidth()-1)), jlimit<int>(0,tex.getHeight()-1,si->points[i].y*(tex.getHeight()-1))));
		}
	}
}

SpatItem * Spatializer::getItemForProp(Prop * p, SpatLayout * forceLayout)
{
	SpatLayout * targetLayout = forceLayout != nullptr ? forceLayout : currentLayout;
	if (targetLayout == nullptr) return nullptr; 

	SpatItem * defaultSI = nullptr;
	for (auto &si : targetLayout->spatItemManager.items)
	{
		int id = si->filterManager.getTargetIDForProp(p);
		if(id >= 0) return si;

		if (si->isDefault->boolValue() && defaultSI != nullptr) defaultSI = si;
	}

	return defaultSI;
}