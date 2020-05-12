/*
  ==============================================================================

    PropEditor.h
    Created: 12 May 2020 7:17:02pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../Prop.h"

class PropEditor :
    public BaseItemEditor
{
public:
    PropEditor(Prop* p, bool isRoot = false);
    ~PropEditor();

    WeakReference<Prop> prop;

    std::unique_ptr<BoolToggleUI> logInUI;
    std::unique_ptr<BoolToggleUI> logOutUI;

    void resizedInternalHeaderItemInternal(Rectangle<int>& r) override;
};