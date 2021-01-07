/*
  ==============================================================================

    VideoClipUI.cpp
    Created: 5 Jan 2021 9:36:58pm
    Author:  bkupe

  ==============================================================================
*/

#include "VideoClipUI.h"

VideoClipUI::VideoClipUI(VideoClip* clip) :
    AudioLayerClipUI(clip),
    videoClip(clip)
{
    videoClip->videoReader.addVideoListener(this);
    setupThumbnail();
}

VideoClipUI::~VideoClipUI()
{
    if(!inspectable.wasObjectDeleted()) videoClip->videoReader.removeVideoListener(this);
}

void VideoClipUI::setupThumbnail()
{
    thumbnail.clear();
    if (videoClip->readerSource == nullptr) return;
    int numSamples = videoClip->readerSource->getAudioFormatReader()->lengthInSamples;
    AudioSampleBuffer buffer(2, numSamples);
    videoClip->readerSource->getAudioFormatReader()->read(&buffer, numSamples, 0, 0, true, true);
   
    const int samplesPerBlock = 1024;
    int numBlocks = floorf(numSamples / samplesPerBlock);

    for (int i = 0; i < numBlocks; i++) thumbnail.addBlock(i * samplesPerBlock, buffer, i * samplesPerBlock, samplesPerBlock);

    repaint();
}

void VideoClipUI::videoFileChanged(const File& f)
{
    setupThumbnail();
}

void VideoClipUI::videoSizeChanged(int w, int h, AVPixelFormat)
{
}

