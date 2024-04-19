/*
  ==============================================================================

    VideoFileBlock.h
    Created: 21 May 2021 8:01:27am
    Author:  bkupe

  ==============================================================================
*/

#pragma once


class VideoFileBlock :
	public VideoBlock,
	public SharedTextureReceiver::Listener
{
public:
	VideoFileBlock(var params = var());
	~VideoFileBlock();

	FileParameter * videoPath;
	Image getImage() override;

	void setupVideo();

	void onContainerParameterChangedInternal(Parameter* p) override;

	DECLARE_TYPE("Video File")
};
