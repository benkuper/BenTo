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
#include "Video/Spatializer.h"
#include "Audio/AudioManager.h"

BentoEngine::BentoEngine() :
	Engine("BenTo", ".bento")
{
	Engine::mainEngine = this;

	addChildControllableContainer(LightBlockModelLibrary::getInstance());
	addChildControllableContainer(PropManager::getInstance());

	ProjectSettings::getInstance()->addChildControllableContainer(AudioManager::getInstance());


	OSCRemoteControl::getInstance()->addRemoteControlListener(this);
}

BentoEngine::~BentoEngine()
{
	PropManager::deleteInstance();
	LightBlockModelLibrary::deleteInstance();
	Spatializer::deleteInstance();
	AudioManager::deleteInstance();
}

void BentoEngine::clearInternal()
{
	Spatializer::getInstance()->clear();
	PropManager::getInstance()->clear();
	LightBlockModelLibrary::getInstance()->clear();
}


void BentoEngine::processMessage(const OSCMessage & m)
{
	StringArray aList;
	aList.addTokens(m.getAddressPattern().toString(), "/", "\"");

	if (aList.size() < 2) return;

	if (aList[1] == "model")
	{
		String modelName = OSCHelpers::getStringArg(m[0]);
		LightBlockModel * lm = LightBlockModelLibrary::getInstance()->getModelWithName(modelName);

		if (lm != nullptr)
		{
			if (aList[2] == "assign")
			{
				if (m.size() >= 2)
				{
					int id = OSCHelpers::getIntArg(m[1]);

					LightBlockModelPreset * mp = nullptr;
					if (m.size() >= 3)
					{
						String presetName = OSCHelpers::getStringArg(m[2]);
						mp = lm->presetManager.getItemWithName(presetName);
					}

					LightBlockColorProvider * providerToAssign = mp != nullptr ? mp : (LightBlockColorProvider *)lm;
					Array<Prop *> propsToAssign = PropManager::getInstance()->getPropsWithId(id);
					for (auto &p : propsToAssign) p->activeProvider->setValueFromTarget(providerToAssign);
				}


			}
		}

	} else if (aList[1] == "prop")
	{
		int id = OSCHelpers::getIntArg(m[0]);
		Array<Prop *> props = PropManager::getInstance()->getPropsWithId(id);

		if (aList[2] == "enable")
		{
			bool active = OSCHelpers::getIntArg(m[1]) > 0;
			for (auto & p : props) p->enabled->setValue(active);
		}
	}
}


var BentoEngine::getJSONData()
{
	var data = Engine::getJSONData();

	var mData = LightBlockModelLibrary::getInstance()->getJSONData();
	if (!mData.isVoid() && mData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("models", mData);

	var propData = PropManager::getInstance()->getJSONData();
	if (!propData.isVoid() && propData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("props", propData);


	var spatData = Spatializer::getInstance()->getJSONData();
	if (!spatData.isVoid() && spatData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("spatializer", spatData);

	return data;
}

void BentoEngine::loadJSONDataInternalEngine(var data, ProgressTask * loadingTask)
{
	//ProgressTask * projectTask = loadingTask->addTask("Project");
	ProgressTask * modelsTask = loadingTask->addTask("Models");
	ProgressTask * propTask = loadingTask->addTask("Props");
	ProgressTask * spatTask = loadingTask->addTask("Spatializer");


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

	spatTask->start();
	Spatializer::getInstance()->loadJSONData(data.getProperty("spatializer", var()));
	spatTask->setProgress(1);
	spatTask->end();
}
