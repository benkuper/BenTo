/*
  ==============================================================================

    VideoClipUI.h
    Created: 5 Jan 2021 9:36:58pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "../VideoClip.h"

class VideoClipUI :
    public AudioLayerClipUI
#if FILMSTRO_USE_FFMPEG
    ,public FFmpegVideoListener
#endif
{
public:
    VideoClipUI(VideoClip* clip);
    ~VideoClipUI();

    VideoClip* videoClip;

    void setupThumbnail() override;

#if FILMSTRO_USE_FFMPEG
    void videoFileChanged(const File& f) override;
    void videoSizeChanged(int w, int h, AVPixelFormat) override;
#endif
};