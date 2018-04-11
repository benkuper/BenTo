/*
  ==============================================================================

    BentoEngine.cpp
    Created: 10 Apr 2018 5:14:40pm
    Author:  Ben

  ==============================================================================
*/

#include "BentoEngine.h"

#include "LightBlock/model/LightBlockModelLibrary.h"
#include "Prop/PropManager.h"

BentoEngine::BentoEngine(ApplicationProperties * appProperties, const String &appVersion) :
	Engine("BenTo",".bento", appProperties, appVersion)
{
	Engine::mainEngine = this;

	addChildControllableContainer(LightBlockModelLibrary::getInstance());
	addChildControllableContainer(PropManager::getInstance());
}

BentoEngine::~BentoEngine()
{
	LightBlockModelLibrary::deleteInstance();
	PropManager::deleteInstance();
}
