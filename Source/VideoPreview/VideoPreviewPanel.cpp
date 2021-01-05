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
    addAndMakeVisible(&videoComponent);
}

VideoPreviewPanel::~VideoPreviewPanel()
{
    setVideoClip(nullptr);
}

void VideoPreviewPanel::setVideoClip(VideoClip * clip)
{
    videoComponent.setVideoReader(clip != nullptr ? &clip->videoReader : nullptr);
    
    MessageManagerLock mmLock;
    resized();
    videoComponent.repaint();
}

void VideoPreviewPanel::resized()
{
    videoComponent.setBounds(getLocalBounds());
    videoComponent.resized();
}
