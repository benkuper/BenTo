/*
  ==============================================================================

    VideoBlock.cpp
    Created: 10 Apr 2018 6:58:49pm
    Author:  Ben

  ==============================================================================
*/

VideoBlock::VideoBlock(var params) :
	LightBlockModel(getTypeString(), params),
	receiver(nullptr)
{

	spat.reset(new Spatializer());
	addChildControllableContainer(spat.get());
	spat->addBaseManagerListener(this);

	textureName = addStringParameter("Texture Name", "The Spout / Syphon name of the texture","");
	inputIsLive = addBoolParameter("Input is Live", "If there is something coming, you'll know it here", false);
	inputIsLive->setControllableFeedbackOnly(true);

	currentLayout = paramsContainer->addEnumParameter("Layout", "The Layout to use. You can edit them by clicking inside the Spatializer window");
	updateLayoutOptions();

	saveAndLoadName = false;
	saveType = false;
	
	setupReceiver();
	
	
}

VideoBlock::~VideoBlock()
{

	if (SharedTextureManager::getInstanceWithoutCreating() != nullptr)
	{
		if (receiver != nullptr)
		{
			receiver->removeListener(this);
			SharedTextureManager::getInstance()->removeReceiver(receiver);
		}
	}

	spat->removeBaseManagerListener(this);
}

void VideoBlock::setupReceiver()
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

Image VideoBlock::getImage()
{
	if (receiver == nullptr) return Image();
	else  return receiver->getImage();
}

void VideoBlock::updateLayoutOptions()
{
	String layoutName = currentLayout->getValueData().toString();
	currentLayout->clearOptions();
	for (auto &layout : spat->items)
	{
		currentLayout->addOption(layout->niceName, layout->shortName, false);
	}

	currentLayout->setValueWithData(layoutName);
}

Array<Colour> VideoBlock::getColors(Prop * p, double time, var params)
{
	if (!inputIsLive->boolValue()) return LightBlockModel::getColors(p, time, params);
	SpatLayout * layout = spat->getItemWithName(getParamValue<var>(currentLayout, params).toString());
	Array<SpatItem *> spatItems = spat->getItemsForProp(p, layout);

	if(spatItems.isEmpty())  return LightBlockModel::getColors(p, time, params);


	int resolution = p->resolution->intValue();
	Array<Colour> result;
	result.resize(resolution);
	result.fill(Colours::black);

	for (auto& si : spatItems)
	{
		int firstSpatIndex = si->startIndex->intValue() -1 ;//to zero based
		int numSpatColors = si->resolution->intValue();

		for (int i = firstSpatIndex; (i < firstSpatIndex + numSpatColors) && i < resolution; i++) result.setUnchecked(i, si->colors[i-firstSpatIndex]);

	}
	
	return result;
}

void VideoBlock::onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c)
{
	LightBlockModel::onControllableFeedbackUpdateInternal(cc, c);

	if (c == currentLayout)
	{
		spat->setCurrentLayout(spat->getItemWithName(currentLayout->getValueData().toString()));
	}
}

void VideoBlock::textureUpdated(SharedTextureReceiver *)
{
	spat->computeSpat(receiver->getImage());
	videoListeners.call(&VideoListener::textureUpdated, this);
}

void VideoBlock::connectionChanged(SharedTextureReceiver *)
{
	inputIsLive->setValue(receiver->isConnected);
}

void VideoBlock::itemAdded(SpatLayout *)
{
	updateLayoutOptions();
	if (spat->items.size() == 1) currentLayout->setValueWithData(spat->items[0]->shortName);
}

void VideoBlock::itemRemoved(SpatLayout *)
{
	updateLayoutOptions();
}

void VideoBlock::clear()
{
	LightBlockModel::clear();
	spat->clear();
}

var VideoBlock::getJSONData()
{
	var data = LightBlockModel::getJSONData();
	data.getDynamicObject()->setProperty("spat", spat->getJSONData());
	return data;
}

void VideoBlock::loadJSONDataInternal(var data)
{
	LightBlockModel::loadJSONDataInternal(data);
	spat->loadJSONData(data.getProperty("spat", var()));
	spat->setCurrentLayout(spat->getItemWithName(currentLayout->getValueData().toString()));
}


void VideoBlock::onContainerParameterChangedInternal(Parameter * p)
{
	if (p == textureName) setupReceiver();
}

LightBlockModelUI * VideoBlock::createUI()
{
	return new VideoBlockUI(this);
}
