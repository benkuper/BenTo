/*
  ==============================================================================

    VideoPreviewPanel.cpp
    Created: 4 Jan 2021 5:52:53pm
    Author:  bkupe

  ==============================================================================
*/

#include "VideoPreviewPanel.h"
#include "LightBlock/model/blocks/timeline/TimelineBlock.h"
#include "Audio/AudioManager.h"
#include "Timeline/layers/Video/VideoClip.h"

VideoPreviewPanel::VideoPreviewPanel(const String& name) :
    ShapeShifterContentComponent(name)
{
#if FILMSTRO_USE_FFMPEG
    addAndMakeVisible(&videoComponent);
#endif
}

VideoPreviewPanel::~VideoPreviewPanel()
{
    setVideoClip(nullptr);
}

void VideoPreviewPanel::setVideoClip(VideoClip* clip)
{
#if FILMSTRO_USE_FFMPEG
    videoComponent.setVideoReader(clip != nullptr ? &clip->videoReader : nullptr);
    MessageManagerLock mmLock;
    resized();
    videoComponent.repaint();
#endif
}

void VideoPreviewPanel::resized()
{
#if FILMSTRO_USE_FFMPEG
    videoComponent.setBounds(getLocalBounds());
    videoComponent.resized();
#endif
}
