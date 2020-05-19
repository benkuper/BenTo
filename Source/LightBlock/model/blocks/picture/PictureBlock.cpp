/*
  ==============================================================================

	PictureBlock.cpp
	Created: 22 Apr 2018 8:30:42pm
	Author:  Ben

  ==============================================================================
*/

#include "PictureBlock.h"
#include "ui/PictureBlockUI.h"

PictureBlock::PictureBlock(var params) :
	LightBlockModel(getTypeString(), params)
{
	pictureFile = new FileParameter("Picture File", "The file, the picture, the thing", "");
	addParameter(pictureFile);

	loopTime = paramsContainer->addFloatParameter("Loop Time", "The time to do one cycle loop", 1, .01f);
	loopTime->defaultUI = FloatParameter::TIME;
	offsetByID = paramsContainer->addFloatParameter("Offset by ID", "The offset", 0, 0, 1);
	brightness = paramsContainer->addFloatParameter("Brightness", "The brightness", 1, 0, 1);
	saturation = paramsContainer->addFloatParameter("Saturation", "The saturation", 1, 0, 3);
	hue = paramsContainer->addFloatParameter("Hue", "The hue", 0, -1, 1);
}

void PictureBlock::getColorsInternal(Array<Colour> * result, Prop * p, double time, int id, int resolution, var params)
{
	if (picture.getWidth() == 0) return;

	float bLoopTime = getParamValue<float>(loopTime, params);
	float bOffsetByID = getParamValue<float>(offsetByID, params);
	float bBrightness = getParamValue<float>(brightness, params);
	float bSaturation = getParamValue<float>(saturation, params);
	float bHue = getParamValue<float>(hue, params);
	
	int numPixelsH = picture.getHeight();
	float txRel = fmodf(time / bLoopTime + bOffsetByID * id, 1);
	int  tx = jmin<int>(txRel * picture.getWidth(), picture.getWidth() - 1);

	for (int i = 0; i < resolution; i++)
	{
		float ty = jmin(i*numPixelsH / resolution, numPixelsH - 1);
		
		float h = 0, s = 0, b = 0;
		picture.getPixelAt(tx, ty).getHSB(h, s, b);
		result->set(i, Colour::fromHSV(h + bHue, jmin(1.0f, s * bSaturation), jmin(1.0f, b * bBrightness), 1));
	}
}

void PictureBlock::onContainerParameterChangedInternal(Parameter * p)
{
	LightBlockModel::onContainerParameterChangedInternal(p);

	if (p == pictureFile)
	{
		File f = pictureFile->getFile();
		picture = ImageCache::getFromFile(f);
	}
}


LightBlockModelUI * PictureBlock::createUI()
{
	return new PictureBlockUI(this);
}

var PictureBlock::getJSONData()
{
	return LightBlockModel::getJSONData();
}
