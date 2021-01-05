/*
  ==============================================================================

    VideoClip.cpp
    Created: 4 Jan 2021 10:34:30pm
    Author:  bkupe

  ==============================================================================
*/

#include "VideoClip.h"
#include "VideoPreview/VideoPreviewPanel.h"

VideoClip::VideoClip() :
	videoReader(192000, 50)
{
	videoReader.addVideoListener(this);
}

VideoClip::~VideoClip()
{
	videoReader.releaseResources();
}

void VideoClip::setupFromSource()
{
	bool result = videoReader.loadMovieFile(filePath->getFile());

	if (!result)
	{
		LOGERROR("FUck.");
	}

}

void VideoClip::setIsCurrent(bool value)
{
	AudioLayerClip::setIsCurrent(value);

	if (VideoPreviewPanel* panel = ShapeShifterManager::getInstance()->getContentForType<VideoPreviewPanel>())
	{
		panel->setVideoClip(isCurrent ? this : nullptr);
	}
}

void VideoClip::prepareToPlay(int blockSize, int sampleRate)
{
	AudioLayerClip::prepareToPlay(blockSize, sampleRate);
}

void VideoClip::videoFileChanged(const File& f)
{
	transportSource.setSource(&videoReader, 0, nullptr, videoReader.getVideoSamplingRate(), videoReader.getVideoChannels());
	//videoAspectRatio = videoReader->getVideoAspectRatio();

	clipDuration = videoReader.getVideoDuration();
	clipLength->setValue(clipDuration);
	if (!coreLength->isOverriden)
	{
		coreLength->defaultValue = clipLength->floatValue();
		coreLength->resetValue();
	}

	//buffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);
	//reader->read(&buffer, 0, (int)reader->lengthInSamples, 0, true, true);
}

void VideoClip::videoSizeChanged(int w, int h, AVPixelFormat)
{
	LOG("Video size changed : " << w << ", " << h);
}

void VideoClip::onContainerParameterChangedInternal(Parameter* p)
{
	AudioLayerClip::onContainerParameterChangedInternal(p);
}