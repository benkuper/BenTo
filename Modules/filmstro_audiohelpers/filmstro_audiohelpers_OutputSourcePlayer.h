/*
 ==============================================================================
 Copyright (c) 2017, Filmstro Ltd. - Daniel Walz
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:
 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 3. Neither the name of the copyright holder nor the names of its contributors
 may be used to endorse or promote products derived from this software without
 specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.
 ==============================================================================
 \class        OutputSourcePlayer
 \file         filmstro_audiobasics_OutputSourcePlayer.h
 \brief        An AudioSourcePlayer which handles changes of the sample rate

 \author       Daniel Walz / Filmstro Ltd.
 \date         December 19th 2016

 \description  An AudioSourcePlayer which handles changes of the sample rate

 ==============================================================================
 */


#ifndef filmstro_audiobasics_OutputSourcePlayer_h
#define filmstro_audiobasics_OutputSourcePlayer_h

/**
    This class provides an AudioSourcePlayer, which will adapt its resampler whenever 
    the device changes the sampling rate
  */
class OutputSourcePlayer : public juce::AudioSourcePlayer
{
public:
    /**
     Constructs an OutputSourcePlayer. This makes the processing independent from the devices playback rate.
     \param sampleRate is the sample rate the playing AudioSource would expect
     \param channels is the number of channels for the resampler to provide. Defaults to 2 (stereo)
     */
    OutputSourcePlayer (double sampleRate, int channels=2)
    : internalSampleRate (sampleRate),
    deviceSampleRate (0.0),
    numChannels (channels),
    outputDevice (nullptr)
    {

    }

    /**
     Set or replace the source of the AudioSourcePlayer
     */
    void setSource (juce::AudioSource* newSource)
    {
        const juce::ScopedLock sl (readLock);
        juce::AudioSourcePlayer::setSource (newSource);
    }

    /**
     Callback from the AudioIODevice. This drives the playback of the AudioSource
     */
    void audioDeviceIOCallback (const float **inputChannelData, int totalNumInputChannels,
                                float **outputChannelData, int totalNumOutputChannels,
                                int numSamples) override
    {
        if (getCurrentSource()) {
            const juce::ScopedLock sl (readLock);

            if (outputDevice != nullptr) {
                if (fabs (deviceSampleRate - outputDevice->getCurrentSampleRate ()) > 1.0) {
                    deviceSampleRate = outputDevice->getCurrentSampleRate();
                    resampler->setResamplingRatio (deviceSampleRate != 0 ?
                                                   internalSampleRate / deviceSampleRate :
                                                   1.0);
                    resampler->flushBuffers ();
                }
            }
            juce::AudioBuffer<float> buffer (outputChannelData,
                                             totalNumOutputChannels,
                                             numSamples);
            juce::AudioSourceChannelInfo bufferToFill (&buffer, 0, numSamples);
            resampler->getNextAudioBlock (bufferToFill);
        }
        else {
            juce::AudioSourcePlayer::audioDeviceIOCallback (inputChannelData,
                                                            totalNumInputChannels,
                                                            outputChannelData,
                                                            totalNumOutputChannels,
                                                            numSamples);
        }
    }

    /**
     callback when the device wants to start using the AudioDeviceIOCallback
     */
    void audioDeviceAboutToStart (juce::AudioIODevice *device) override
    {
        outputDevice = device;
        resampler.reset(new juce::ResamplingAudioSource(getCurrentSource(), false));
        resampler->prepareToPlay (device->getCurrentBufferSizeSamples(),
                                  device->getCurrentSampleRate());
        juce::AudioSourcePlayer::audioDeviceAboutToStart (device);
    }

    /**
     Callback when the AudioIODevice just stopped using the AudioDeviceIOCallback
     */
    void audioDeviceStopped () override
    {
        juce::AudioSourcePlayer::audioDeviceStopped();
        resampler.reset();
        outputDevice = nullptr;
    }



private:
    double  internalSampleRate;

    double  deviceSampleRate;

    int     numChannels;

    juce::AudioIODevice*    outputDevice;
    std::unique_ptr<juce::ResamplingAudioSource> resampler;
    juce::CriticalSection readLock;
};


#endif /* filmstro_audiobasics_OutputSourcePlayer_h */
