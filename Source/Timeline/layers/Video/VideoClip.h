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
    public AudioLayerClip,
    public FFmpegVideoListener
{
public:
    VideoClip();
    ~VideoClip();

   FFmpegVideoReader videoReader;

    void setupFromSource() override;
    
    void setIsCurrent(bool value) override;

    virtual void prepareToPlay(int blockSize, int sampleRate) override;
    
    void videoFileChanged(const File& f) override;
    void videoSizeChanged(int w, int h, AVPixelFormat) override;

    void onContainerParameterChangedInternal(Parameter* p) override;
};