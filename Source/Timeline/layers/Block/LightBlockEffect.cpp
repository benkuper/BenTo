/*
  ==============================================================================

    LightBlockEffect.cpp
    Created: 13 Jan 2021 11:27:40pm
    Author:  bkupe

  ==============================================================================
*/

LightBlockEffect::LightBlockEffect() :
    BaseItem("Effect"),
    lightBlock(nullptr)
{
}

LightBlockEffect::~LightBlockEffect()
{
}

void LightBlockEffect::setProvider(LightBlockColorProvider* provider)
{
    if (lightBlock != nullptr) return;

    if (provider == nullptr) return;

    lightBlock.reset(new LightBlock(provider));
    addChildControllableContainer(lightBlock.get());
}

void LightBlockEffect::onContainerParameterChangedInternal(Parameter* p)
{
    //
}

var LightBlockEffect::getJSONData()
{
    var data = BaseItem::getJSONData();
    if (lightBlock != nullptr)
    {
        data.getDynamicObject()->setProperty("provider", lightBlock->provider->getControlAddress(LightBlockModelLibrary::getInstance()));
        data.getDynamicObject()->setProperty("block", lightBlock->getJSONData());
    }
   
    return data;
}

void LightBlockEffect::loadJSONDataItemInternal(var data)
{
    setProvider((LightBlockColorProvider *)LightBlockModelLibrary::getInstance()->getControllableContainerForAddress(data.getProperty("provider", "")));
    if (lightBlock != nullptr)
    {
        lightBlock->loadJSONData(data.getProperty("providerData", var()));
    }
}
