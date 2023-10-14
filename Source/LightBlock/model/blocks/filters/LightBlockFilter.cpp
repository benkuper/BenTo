/*
  ==============================================================================

    LightBlockFilter.cpp
    Created: 18 May 2020 3:27:11pm
    Author:  bkupe

  ==============================================================================
*/

LightBlockFilter::LightBlockFilter(const String& name, var params) :
    LightBlockModel(name, params, ProviderType::FILTER)
{
    assignToAll->hideInEditor = true;
    setCanBeDisabled(true);
}

LightBlockFilter::~LightBlockFilter()
{
}

void LightBlockFilter::filterColors(Array<Colour>* result, Prop* p, double time, var params)
{
    if (!enabled->boolValue()) return;
    int id = params.getProperty("forceID", p->globalID->intValue());
    getColorsInternal(result, p, time, id, p->getResolution(), params);
}
