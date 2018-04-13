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
	PropManager::deleteInstance();
	LightBlockModelLibrary::deleteInstance();
}

void BentoEngine::clearInternal()
{
	PropManager::getInstance()->clear();
	LightBlockModelLibrary::getInstance()->clear();
}

var BentoEngine::getJSONData()
{
	var data = Engine::getJSONData();

	var mData = LightBlockModelLibrary::getInstance()->getJSONData();
	if (!mData.isVoid() && mData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("models", mData);

	var propData = PropManager::getInstance()->getJSONData();
	if (!propData.isVoid() && propData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("props", propData);

	return data;
}

void BentoEngine::loadJSONDataInternalEngine(var data, ProgressTask * loadingTask)
{
	//ProgressTask * projectTask = loadingTask->addTask("Project");
	ProgressTask * modelsTask = loadingTask->addTask("Models");
	ProgressTask * propTask = loadingTask->addTask("Props");


	//load here
	//projectTask->start();
	//ProjectSettings::getInstance()->loadJSONData(data.getProperty("projectSettings", var()));
	//projectTask->setProgress(1);
	//projectTask->end();

	modelsTask->start();
	LightBlockModelLibrary::getInstance()->loadJSONData(data.getProperty("models", var()));
	modelsTask->setProgress(1);
	modelsTask->end();

	propTask->start();
	PropManager::getInstance()->loadJSONData(data.getProperty("props", var()));
	propTask->setProgress(1);
	propTask->end();
}
