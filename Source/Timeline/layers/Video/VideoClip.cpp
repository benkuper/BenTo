/*
  ==============================================================================

    VideoClip.cpp
    Created: 4 Jan 2021 10:34:30pm
    Author:  bkupe

  ==============================================================================
*/

#include "VideoClip.h"
#include "VideoPreview/VideoPreviewPanel.h"

VideoClip::VideoClip() 
#if FILMSTRO_USE_FFMPEG
	: videoReader(192000, 50)
#endif
{
#if FILMSTRO_USE_FFMPEG
	videoReader.addVideoListener(this);
#endif
}

VideoClip::~VideoClip()
{
#if FILMSTRO_USE_FFMPEG
	videoReader.releaseResources();
#endif
}

void VideoClip::setupFromSource()
{
#if FILMSTRO_USE_FFMPEG
	bool result = videoReader.loadMovieFile(filePath->getFile());

	if (!result)
	{
		LOGERROR("FUck.");
	}
#endif

}

void VideoClip::prepareToPlay(int blockSize, int sampleRate)
{
	AudioLayerClip::prepareToPlay(blockSize, sampleRate);
}

#if FILMSTRO_USE_FFMPEG
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
#endif

void VideoClip::onContainerParameterChangedInternal(Parameter* p)
{
	AudioLayerClip::onContainerParameterChangedInternal(p);
	if (p == isActive)
	{
		if (VideoPreviewPanel* panel = ShapeShifterManager::getInstance()->getContentForType<VideoPreviewPanel>())
		{
			panel->setVideoClip(isActive->boolValue() ? this : nullptr);
		}
	}
}