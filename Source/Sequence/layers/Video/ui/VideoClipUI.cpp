/*
  ==============================================================================

    VideoClipUI.cpp
    Created: 5 Jan 2021 9:36:58pm
    Author:  bkupe

  ==============================================================================
*/

VideoClipUI::VideoClipUI(VideoClip* clip) :
    AudioLayerClipUI(clip),
    videoClip(clip)
{
#if FILMSTRO_USE_FFMPEG
    videoClip->videoReader.addVideoListener(this);
#endif
    setupThumbnail();
}

VideoClipUI::~VideoClipUI()
{
#if FILMSTRO_USE_FFMPEG
    if(!inspectable.wasObjectDeleted()) videoClip->videoReader.removeVideoListener(this);
#endif
}

void VideoClipUI::setupThumbnail()
{
#if FILMSTRO_USE_FFMPEG
    thumbnail.clear();
    if (videoClip->readerSource == nullptr) return;
    int numSamples = videoClip->readerSource->getAudioFormatReader()->lengthInSamples;
    AudioSampleBuffer buffer(2, numSamples);
    videoClip->readerSource->getAudioFormatReader()->read(&buffer, numSamples, 0, 0, true, true);
   
    const int samplesPerBlock = 1024;
    int numBlocks = floorf(numSamples / samplesPerBlock);

    for (int i = 0; i < numBlocks; i++) thumbnail.addBlock(i * samplesPerBlock, buffer, i * samplesPerBlock, samplesPerBlock);
#endif

    repaint();
}

#if FILMSTRO_USE_FFMPEG
void VideoClipUI::videoFileChanged(const File& f)
{
    setupThumbnail();
}

void VideoClipUI::videoSizeChanged(int w, int h, AVPixelFormat)
{
}
#endif

