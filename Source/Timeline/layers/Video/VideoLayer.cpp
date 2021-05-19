/*
  ==============================================================================

    VideoLayer.cpp
    Created: 4 Jan 2021 10:34:27pm
    Author:  bkupe

  ==============================================================================
*/

#include "VideoLayer.h"
#include "VideoClip.h"
#include "ui/VideoLayerTimeline.h"

VideoLayer::VideoLayer(Sequence* sequence, var params) :
    AudioLayer(sequence, params)
{
    setNiceName("Video");
}

VideoLayer::~VideoLayer()
{
}

AudioLayerClip* VideoLayer::createAudioClip()
{
    return new VideoClip();
}

void VideoLayer::updateCurrentClip()
{
    bool wasNull = currentClip == nullptr;
    AudioLayer::updateCurrentClip();
    if (wasNull && currentClip != nullptr)
    {
        startTimer(10);
    }
}

SequenceLayerTimeline* VideoLayer::getTimelineUI()
{
    return new VideoLayerTimeline(this);
}

void VideoLayer::timerCallback()
{
    if (currentClip != nullptr)
    {
        float pos = currentClip->clipStartOffset->floatValue() + (sequence->hiResAudioTime - currentClip->time->floatValue()) / currentClip->stretchFactor->floatValue();
        currentClip->transportSource.setPosition(pos);
    }
    stopTimer();
}
