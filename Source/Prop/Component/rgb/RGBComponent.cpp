/*
  ==============================================================================

    RGBComponent.cpp
    Created: 8 May 2020 3:09:02pm
    Author:  bkupe

  ==============================================================================
*/

RGBPropComponent::RGBPropComponent(Prop* prop, var params) :
    PropComponent(prop, "RGB"),
    updateRate(params.getProperty("updateRate", 50)),
    resolution(params.getProperty("resolution", 32)),
    invertDirection(params.getProperty("invertDirection", false)),
    useLayout(params.hasProperty("layout"))
{

    brightness = addFloatParameter("Brightness", "Brightness", params.getProperty("defaultBrightness",.5f), 0, 1);

    if (useLayout)
    {
        var layoutData = params.getProperty("layout", var());
        if (layoutData.size() == resolution)
        {
            ledIndexMap.resize(layoutData.size());
            for (int i = 0; i < layoutData.size(); i++) ledIndexMap.set((int)layoutData[i], i);
        }
        else
        {
            LOGWARNING("Layout has not the same size as resolution " << layoutData.size() << " < > " << resolution);
        }
    }
}

RGBPropComponent::~RGBPropComponent()
{
}

void RGBPropComponent::handePropConnected()
{
    PropComponent::handePropConnected();
    sendControl("enabled", prop->enabled->boolValue());
}
