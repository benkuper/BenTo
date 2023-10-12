#include "SharedTextureBlock.h"
/*
  ==============================================================================

    SharedTextureBlock.cpp
    Created: 10 Apr 2018 6:58:49pm
    Author:  Ben

  ==============================================================================
*/

SharedTextureBlock::SharedTextureBlock(var params) :
	VideoBlock(getTypeString(), params),
	receiver(nullptr)
{
	textureName = addStringParameter("Texture Name", "The Spout / Syphon name of the texture","");
	setupReceiver();

	SharedTextureManager::getInstance()->addListener(this);
}

SharedTextureBlock::~SharedTextureBlock()
{

	if (SharedTextureManager::getInstanceWithoutCreating() != nullptr)
	{
		if (receiver != nullptr)
		{
			receiver->removeListener(this);
			SharedTextureManager::getInstance()->removeReceiver(receiver);
			receiver = nullptr;
		}

		SharedTextureManager::getInstance()->removeListener(this);
	}

}

void SharedTextureBlock::setupReceiver()
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
		//receiver->setUseCPUImage(true);
		//receiver->createReceiver();
	}
	
}

Image SharedTextureBlock::getImage()
{
	if (receiver == nullptr || isClearing) return Image();
	else  return receiver->getImage();
}

void SharedTextureBlock::textureUpdated(SharedTextureReceiver*)
{
	Spatializer::getInstance()->computeSpat(getImage());
	//updateColorsFromImage();
}

void SharedTextureBlock::connectionChanged(SharedTextureReceiver*)
{
	inputIsLive->setValue(receiver->isConnected);
}

void SharedTextureBlock::receiverRemoved(SharedTextureReceiver* r)
{
	if(r == receiver) receiver = nullptr;
}

void SharedTextureBlock::onContainerParameterChangedInternal(Parameter * p)
{
	if (p == textureName) setupReceiver();
}

LightBlockModelUI * SharedTextureBlock::createUI()
{
	return new SharedTextureBlockUI(this);
}
