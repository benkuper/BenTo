/*
  ==============================================================================

    VideoPreviewPanel.h
    Created: 4 Jan 2021 5:52:53pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class VideoClip;

class VideoPreviewPanel :
    public ShapeShifterContentComponent
{
public:
    VideoPreviewPanel(const String &name);
    ~VideoPreviewPanel();

#if FILMSTRO_USE_FFMPEG
    FFmpegVideoComponent videoComponent;
#endif

    void setVideoClip(VideoClip * clip);
    void resized() override;

    static VideoPreviewPanel* create(const String& name) { return new VideoPreviewPanel(name); }
};