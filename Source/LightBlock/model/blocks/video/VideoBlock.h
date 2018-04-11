/*
  ==============================================================================

    VideoBlock.h
    Created: 10 Apr 2018 6:58:49pm
    Author:  Ben

  ==============================================================================
*/

#pragma once


#include "../../LightBlockModel.h"

class VideoBlock :
	public LightBlockModel
{
public:
	VideoBlock(var params = var());
	~VideoBlock();

	String getTypeString() const override { return "Video"; }

	static VideoBlock * create(var params) { return new VideoBlock(params); }
};