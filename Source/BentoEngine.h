/*
  ==============================================================================

    BentoEngine.h
    Created: 10 Apr 2018 5:14:40pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class BentoEngine : 
	public Engine
{
public:
	BentoEngine(ApplicationProperties * appProperties, const String &appVersion);
	~BentoEngine();

};