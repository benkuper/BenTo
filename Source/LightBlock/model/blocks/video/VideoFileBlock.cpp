#include "VideoFileBlock.h"
/*
  ==============================================================================

	VideoFileBlock.cpp
	Created: 10 Apr 2018 6:58:49pm
	Author:  Ben

  ==============================================================================
*/

VideoFileBlock::VideoFileBlock(var params) :
	VideoBlock(getTypeString(), params)
{
	videoPath = addFileParameter("Video Path", "Path of the video file");
}

VideoFileBlock::~VideoFileBlock()
{

}

Image VideoFileBlock::getImage()
{
	return Image();
}

void VideoFileBlock::setupVideo()
{
	//
}

void VideoFileBlock::onContainerParameterChangedInternal(Parameter* p)
{
	if (p == videoPath) setupVideo();
}