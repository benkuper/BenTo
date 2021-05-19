/*
  ==============================================================================

    VideoClip.h
    Created: 4 Jan 2021 10:34:30pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class VideoClip :
    public AudioLayerClip
#if FILMSTRO_USE_FFMPEG
    , public FFmpegVideoListener
#endif
{
public:
    VideoClip();
    ~VideoClip();

#if FILMSTRO_USE_FFMPEG
    FFmpegVideoReader videoReader;
#endif

    void setupFromSource() override;


    virtual void prepareToPlay(int blockSize, int sampleRate) override;

#if FILMSTRO_USE_FFMPEG
    void videoFileChanged(const File& f) override;
    void videoSizeChanged(int w, int h, AVPixelFormat) override;
#endif

    void onContainerParameterChangedInternal(Parameter* p) override;
};