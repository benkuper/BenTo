/*
  ==============================================================================

    LightBlockFilter.h
    Created: 18 May 2020 3:27:11pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class LightBlockFilter :
    public LightBlockModel
{
public:
    LightBlockFilter(const String& name = "LightBlockFilterModel", var params = var());
    ~LightBlockFilter();

    void filterColors(Array<Colour>* result, Prop* p, double time, var params);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LightBlockFilter)
};