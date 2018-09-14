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
	FileParameter * pictureFile;
	Image picture;

	Array<Colour> getColors(int id, int resolution, double time, var params) override;

	void onContainerParameterChangedInternal(Parameter *) override;

	LightBlockModelUI * createUI() override;

	var getJSONData() override;

	String getTypeString() const override { return "Picture"; }

	static PictureBlock * create(var params) { return new PictureBlock(params); }

};