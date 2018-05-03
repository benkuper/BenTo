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
	LightBlockModel(getTypeString(),params)
{
	pictureFile = new FileParameter("Picture File", "The file, the picture, the thing", "");
	addParameter(pictureFile);

	speed = paramsContainer->addFloatParameter("Speed", "The speed", 1, 0, 50);
}

Array<Colour> PictureBlock::getColors(int id, int resolution, float time, var params)
{
	if (picture.getWidth() == 0) return LightBlockModel::getColors(id, resolution, time, params);

	Array<Colour> result;
	result.resize(resolution);

	float bSpeed = params.getProperty("speed", speed->floatValue());

	int numPixelsH = picture.getHeight();
	float txRel = fmodf(time*bSpeed, 1);
	int  tx = jmin<int>(txRel * picture.getWidth(), picture.getWidth() - 1);

	for (int i = 0; i < resolution; i++)
	{
		float ty = jmin(i*numPixelsH / resolution, numPixelsH - 1);
		
		result.set(i, picture.getPixelAt(tx, ty));
	}

	return result;
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
