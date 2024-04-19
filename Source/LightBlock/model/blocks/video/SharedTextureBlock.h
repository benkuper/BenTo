/*
  ==============================================================================

    SharedTextureBlock.h
    Created: 10 Apr 2018 6:58:49pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class SharedTextureBlock :
	public VideoBlock,
	public SharedTextureReceiver::Listener,
	public SharedTextureManager::Listener
{
public:
	SharedTextureBlock(var params = var());
	~SharedTextureBlock();

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

	DECLARE_TYPE("Shared Texture")
};
