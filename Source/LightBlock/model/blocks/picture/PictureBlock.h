/*
  ==============================================================================

    PictureBlock.h
    Created: 22 Apr 2018 8:30:42pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "LightBlock/model/LightBlockModel.h"

class PictureBlock :
	public LightBlockModel
{
public:
	PictureBlock(var params = var());
	~PictureBlock() {}

	FloatParameter * speed;
	FloatParameter * offsetByID;
	FloatParameter * brightness;
	FloatParameter * saturation;
	FloatParameter * hue;
	FileParameter * pictureFile;
	Image picture;

	void getColorsInternal(Array<Colour> * result, Prop * p, double time, int id, int resolution, var params) override;

	void onContainerParameterChangedInternal(Parameter *) override;

	LightBlockModelUI * createUI() override;

	var getJSONData() override;

	String getTypeString() const override { return "Picture"; }

	static PictureBlock * create(var params) { return new PictureBlock(params); }

};
