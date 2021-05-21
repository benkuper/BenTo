#include "VideoBlock.h"
/*
  ==============================================================================

	VideoBlock.cpp
	Created: 10 Apr 2018 6:58:49pm
	Author:  Ben

  ==============================================================================
*/

VideoBlock::VideoBlock(var params) :
	TextureBlock(getTypeString(), params)
{
	videoPath = addFileParameter("Video Path", "Path of the video file");
}

VideoBlock::~VideoBlock()
{

}

Image VideoBlock::getImage()
{
	return Image();
}

void VideoBlock::setupVideo()
{
	//
}

void VideoBlock::onContainerParameterChangedInternal(Parameter* p)
{
	if (p == videoPath) setupVideo();
}