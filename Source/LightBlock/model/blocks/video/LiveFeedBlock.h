/*
  ==============================================================================

    LiveFeedBlock.h
    Created: 10 Apr 2018 6:58:49pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class LiveFeedBlock :
	public TextureBlock,
	public SharedTextureReceiver::Listener
{
public:
	LiveFeedBlock(var params = var());
	~LiveFeedBlock();

	StringParameter * textureName;
	SharedTextureReceiver * receiver;

	void setupReceiver();
	Image getImage() override;

	void onContainerParameterChangedInternal(Parameter* p) override;

	// Inherited via Listener
	virtual void textureUpdated(SharedTextureReceiver *) override;
	virtual void connectionChanged(SharedTextureReceiver *) override;


	LightBlockModelUI * createUI() override;
	String getTypeString() const override { return "LiveFeed"; }

	static LiveFeedBlock * create(var params) { return new LiveFeedBlock(params); }
};
