/*
  ==============================================================================

    FilesComponent.h
    Created: 8 May 2020 3:08:44pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../PropComponent.h"

class FilesPropComponent :
    public PropComponent
{
public:
    FilesPropComponent(Prop * prop, var params);
    ~FilesPropComponent();
};