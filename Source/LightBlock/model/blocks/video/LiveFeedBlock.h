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
	public SharedTextureReceiver::Listener,
	public SharedTextureManager::Listener
{
public:
	LiveFeedBlock(var params = var());
	~LiveFeedBlock();

	StringParameter * textureName;
	SharedTextureReceiver * receiver;

	Image tmpImage;

	void setupReceiver();
	Image getImage() override;

	void onContainerParameterChangedInternal(Parameter* p) override;

	// Inherited via Listener
	virtual void textureUpdated(SharedTextureReceiver *) override;
	virtual void connectionChanged(SharedTextureReceiver *) override;
	virtual void receiverRemoved(SharedTextureReceiver* r) override;

	LightBlockModelUI * createUI() override;
	String getTypeString() const override { return "LiveFeed"; }

	static LiveFeedBlock * create(var params) { return new LiveFeedBlock(params); }
};
