/*
  ==============================================================================

    RemapBlockFilter.h
    Created: 18 May 2020 3:27:43pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../../LightBlockFilter.h"

class RemapBlockFilter :
    public LightBlockFilter
{
public:
    RemapBlockFilter(var params = var());
    ~RemapBlockFilter();

    enum RemapMode { STRETCH, CROP };

    EnumParameter* remapMode;
    FloatParameter* sourceStart;
    FloatParameter* sourceEnd;
    FloatParameter* targetStart;
    FloatParameter* targetEnd;

    virtual void getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params) override;

    String getTypeString() const override { return "Remap"; }
};