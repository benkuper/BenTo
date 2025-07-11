/*
==============================================================================

  BentoEngine.cpp
  Created: 10 Apr 2018 5:14:40pm
  Author:  Ben

==============================================================================
*/

#include "BentoEngine.h"

#include "LightBlock/LightBlockIncludes.h"
#include "Prop/PropIncludes.h"
#include "Video/VideoIncludes.h"
#include "Node/NodeIncludes.h"

#include "Audio/AudioManager.h"
#include "BentoSettings.h"
#include "Common/CommonIncludes.h"
#include "Sequence/SequenceIncludes.h"

BentoEngine::BentoEngine() :
	Engine("BenTo", ".bento"),
	customParams("Custom parameters", false, false, false, false)
{
	Engine::mainEngine = this;

	convertURL = "https://www.jonglissimo.com/bento-converter/convert.php";
	breakingChangesVersions.add("2.0.0b1");

	addChildControllableContainer(LightBlockModelLibrary::getInstance());
	addChildControllableContainer(PropManager::getInstance());
	addChildControllableContainer(Spatializer::getInstance());

	projectName = ProjectSettings::getInstance()->addStringParameter("Project name", "This name will be used to identify the project when uploaded to the props", "project", true);

	//Communication
	OSCRemoteControl::getInstance()->localPort->defaultValue = 43000;
	OSCRemoteControl::getInstance()->localPort->resetValue();

	OSCRemoteControl::getInstance()->addRemoteControlListener(this);
	SerialManager::getInstance(); // init


	GlobalSettings::getInstance()->addChildControllableContainer(AudioManager::getInstance());
	GlobalSettings::getInstance()->addChildControllableContainer(BentoSettings::getInstance());

	ProjectSettings::getInstance()->addChildControllableContainer(&customParams);

}

BentoEngine::~BentoEngine()
{
	PropManager::deleteInstance();
	PropShapeLibrary::deleteInstance();
	SerialManager::deleteInstance();
	DMXManager::deleteInstance();

	NodeFactory::deleteInstance();
	LightBlockModelLibrary::deleteInstance();

	AudioManager::deleteInstance();

	//BentoWebServer::deleteInstance();

	BentoSettings::deleteInstance();

	Spatializer::deleteInstance();

	ZeroconfManager::deleteInstance();

	PropFlasher::deleteInstance();
	LightBlockPreviewDispatcher::deleteInstance();
}

void BentoEngine::clearInternal()
{
	PropManager::getInstance()->clear();
	LightBlockModelLibrary::getInstance()->clear();
	Spatializer::getInstance()->clear();

	projectName->resetValue();
}

juce::Result BentoEngine::saveDocument(const File& file)
{
	juce::Result r = Engine::saveDocument(file);

	if (r.wasOk()) {
		if (projectName->getValue() == "project") {
			projectName->setValue(file.getFileName().replace(".bento", ""));
		}
	}

	return r;
}


void BentoEngine::processMessage(const OSCMessage& m, const String& clientId)
{
	StringArray aList;
	aList.addTokens(m.getAddressPattern().toString(), "/", "\"");

	if (aList.size() < 2) return;

	if (aList[1] == "model")
	{
		String modelName = OSCHelpers::getStringArg(m[0]);
		LightBlockModel* lm = LightBlockModelLibrary::getInstance()->getModelWithName(modelName);

		if (lm != nullptr)
		{
			if (aList[2] == "assign")
			{
				if (m.size() >= 2)
				{
					int id = OSCHelpers::getIntArg(m[1]);

					LightBlockModelPreset* mp = nullptr;
					if (m.size() >= 3)
					{
						String presetName = OSCHelpers::getStringArg(m[2]);
						mp = lm->presetManager.getItemWithName(presetName);
					}

					LightBlockColorProvider* providerToAssign = mp != nullptr ? mp : (LightBlockColorProvider*)lm;
					if (id == -1)
					{
						for (auto& p : PropManager::getInstance()->items)  p->activeProvider->setValueFromTarget(providerToAssign);
					}
					else
					{
						Prop* p = PropManager::getInstance()->getPropWithId(id);
						if (p != nullptr) p->activeProvider->setValueFromTarget(providerToAssign);
					}

				}


			}
		}

	}
	else if (aList[1] == "prop")
	{
		//int id = aList[2] == "all" ? -1 : aList[2].getIntValue();

		//String localAddress = "/" + aList.joinIntoString("/", 3);
		//OSCMessage lm(localAddress);
		//lm.addString(""); //fake ID
		//for (auto& a : m) lm.addArgument(a);
		//lm.setAddressPattern(localAddress);

		//if (id == -1)
		//{
		//	for (auto& p : PropManager::getInstance()->items)  p->handleOSCMessage(lm);
		//}
		//else
		//{
		//	if (Prop* p = PropManager::getInstance()->getPropWithId(id)) p->handleOSCMessage(lm);
		//}
	}
	else if (aList[1] == "enableList")
	{
		Array<int> ids;
		for (int i = 0; i < m.size(); i++)
		{
			if (m[i].isInt32()) ids.add(m[i].getInt32());
		}

		for (auto& p : PropManager::getInstance()->items) p->enabled->setValue(ids.contains(p->globalID->intValue()));
	}
	else if (aList[1] == "assignAndPlay")
	{
		String modelName = OSCHelpers::getStringArg(m[0]);
		LightBlockModel* lm = LightBlockModelLibrary::getInstance()->getModelWithName(modelName);

		if (BentoSequenceBlock* b = dynamic_cast<BentoSequenceBlock*>(lm))
		{
			Array<Prop*> props;
			if (m.size() == 1) props.addArray(PropManager::getInstance()->items);
			else
			{
				for (int i = 1; i < m.size(); i++) props.add(PropManager::getInstance()->getPropWithId(m[i].getInt32()));
			}

			for (auto& p : props)
			{
				p->activeProvider->setValueFromTarget(b);
				p->enabled->setValue(true);
				b->sequence->stopTrigger->trigger();
				b->sequence->playTrigger->trigger();
			}
		}
	}
	else if (aList[1] == "stopAllSequences")
	{
		for (auto& b : LightBlockModelLibrary::getInstance()->sequenceBlocks.items)
		{
			if (BentoSequenceBlock* tb = dynamic_cast<BentoSequenceBlock*>(b)) tb->sequence->stopTrigger->trigger();
		}
	}
}


var BentoEngine::getJSONData(bool includeNonOverriden)
{
	var data = Engine::getJSONData(includeNonOverriden);

	var mData = LightBlockModelLibrary::getInstance()->getJSONData();
	if (!mData.isVoid() && mData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("models", mData);


	var propData = PropManager::getInstance()->getJSONData();
	if (!propData.isVoid() && propData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("props", propData);

	return data;
}

void BentoEngine::loadJSONDataInternalEngine(var data, ProgressTask* loadingTask)
{
	//ProgressTask * projectTask = loadingTask->addTask("Project"
	ProgressTask* modelsTask = loadingTask->addTask("Models");
	ProgressTask* propTask = loadingTask->addTask("Props");


	modelsTask->start();
	LightBlockModelLibrary::getInstance()->loadJSONData(data.getProperty("models", var()));
	modelsTask->setProgress(1);
	modelsTask->end();


	propTask->start();
	PropManager::getInstance()->loadJSONData(data.getProperty("props", var()));
	propTask->setProgress(1);
	propTask->end();
}
