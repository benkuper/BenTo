/*
  ==============================================================================

    VideoBlock.h
    Created: 21 May 2021 8:01:27am
    Author:  bkupe

  ==============================================================================
*/

#pragma once


class VideoBlock :
	public TextureBlock,
	public SharedTextureReceiver::Listener
{
public:
	VideoBlock(var params = var());
	~VideoBlock();

	FileParameter * videoPath;
	Image getImage() override;

	void setupVideo();

	void onContainerParameterChangedInternal(Parameter* p) override;

	String getTypeString() const override { return "Video"; }

	static VideoBlock* create(var params) { return new VideoBlock(params); }
};
