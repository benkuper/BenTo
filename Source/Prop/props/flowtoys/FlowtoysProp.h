/*
  ==============================================================================

    FlowtoysProp.h
    Created: 13 Apr 2018 4:22:33pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../bento/BentoProp.h"

class FlowtoysProp :
	public BentoProp
{
public:
	FlowtoysProp(const String &name = "Flowtoys Prop", var params = var());
	~FlowtoysProp();
};
