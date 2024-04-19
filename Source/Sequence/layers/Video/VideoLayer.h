/*
  ==============================================================================

    VideoLayer.h
    Created: 4 Jan 2021 10:34:27pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class VideoLayer :
    public AudioLayer,
    public Timer
{
public:
    VideoLayer(Sequence* sequence, var params);
    ~VideoLayer();

    AudioLayerClip* createAudioClip() override;


    virtual void updateCurrentClip() override;

    virtual SequenceLayerTimeline* getTimelineUI() override;

    static VideoLayer* create(Sequence* sequence, var params) { return new VideoLayer(sequence, params); }
    virtual String getTypeString() const override { return "Video"; }

    void timerCallback() override;
};