/*
  ==============================================================================

    LightBlockEffect.h
    Created: 13 Jan 2021 11:27:40pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class LightBlock;
class LightBlockColorProvider;

class LightBlockEffect :
    public BaseItem
{
public:
    LightBlockEffect();
    ~LightBlockEffect();

    std::unique_ptr<LightBlock> lightBlock;

    void setProvider(LightBlockColorProvider* provider);

    void onContainerParameterChangedInternal(Parameter* p) override;

    var getJSONData(bool includeNonOverriden = false) override;
    void loadJSONDataItemInternal(var data) override;

    String getTypeString() const override { return "Effect"; }
};