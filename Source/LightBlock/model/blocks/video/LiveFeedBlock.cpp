/*
  ==============================================================================

    LiveFeedBlock.cpp
    Created: 10 Apr 2018 6:58:49pm
    Author:  Ben

  ==============================================================================
*/

LiveFeedBlock::LiveFeedBlock(var params) :
	TextureBlock(getTypeString(), params),
	receiver(nullptr)
{


	textureName = addStringParameter("Texture Name", "The Spout / Syphon name of the texture","");


	setupReceiver();
}

LiveFeedBlock::~LiveFeedBlock()
{

	if (SharedTextureManager::getInstanceWithoutCreating() != nullptr)
	{
		if (receiver != nullptr)
		{
			receiver->removeListener(this);
			SharedTextureManager::getInstance()->removeReceiver(receiver);
		}
	}
}

void LiveFeedBlock::setupReceiver()
{
	if (receiver != nullptr)
	{
		receiver->removeListener(this);
		SharedTextureManager::getInstance()->removeReceiver(receiver);
	}

	receiver = nullptr;

	receiver = SharedTextureManager::getInstance()->addReceiver(textureName->stringValue());

	if (receiver != nullptr)
	{
		receiver->addListener(this);
		receiver->createReceiver();
	}
	
}

Image LiveFeedBlock::getImage()
{
	if (receiver == nullptr) return Image();
	else  return receiver->getImage();
}

void LiveFeedBlock::textureUpdated(SharedTextureReceiver*)
{
	//updateColorsFromImage();
}

void LiveFeedBlock::connectionChanged(SharedTextureReceiver*)
{
	inputIsLive->setValue(receiver->isConnected);
}

void LiveFeedBlock::onContainerParameterChangedInternal(Parameter * p)
{
	if (p == textureName) setupReceiver();
}

LightBlockModelUI * LiveFeedBlock::createUI()
{
	return new LiveFeedBlockUI(this);
}
