/*
  ==============================================================================

	EmbeddedScriptBlock.cpp
	Created: 9 Nov 2021 6:06:13pm
	Author:  bkupe

  ==============================================================================
*/

#include "Prop/PropIncludes.h"

EmbeddedScriptBlock::EmbeddedScriptBlock(var params) :
	LightBlockModel(getTypeString(), params),
	Thread("WasmCompile")
{
	scriptFile = addFileParameter("Script", "Path to the script");
	scriptFile->fileTypeFilter = "*.ts";

	compileType = addEnumParameter("Compile Type", "");
	compileType->addOption("Optimized", COMPILE_OPTIMIZED)->addOption("Debug", COMPILE_DEBUG)->addOption("Tiny", COMPILE_TINY);

	lowMemory = addBoolParameter("Low Memory", "", false);
	autoCompile = addBoolParameter("Auto Compile", "", true);
	compileTrigger = addTrigger("Compile", "Compiles the script");
	uploadToPropsTrigger = addTrigger("Upload to Props", "");
	autoUpload = addBoolParameter("Auto Upload", "", true);
	loadOnPropsTrigger = addTrigger("Load on Props", "");
	autoLaunch = addBoolParameter("Auto Launch", "", true);
	stopOnPropsTrigger = addTrigger("Stop on Props", "");
}

EmbeddedScriptBlock::~EmbeddedScriptBlock()
{
}

void EmbeddedScriptBlock::checkAutoCompile()
{
	if (!autoCompile->boolValue()) return;
	File f = scriptFile->getFile();
	if (!f.exists()) return;
	Time t = f.getLastModificationTime();
	if (t > lastModTime)
	{
		stopThread(100);
		startThread();
	}
	lastModTime = t;

}

void EmbeddedScriptBlock::compile()
{
	File f = scriptFile->getFile();
	if (!f.exists()) return;
	File folder = f.getParentDirectory();

	File wf = getWasmFile();
	if (wf.exists()) wf.deleteFile();

	//bool silentMode = false;
	//bool result = true;

	CompileType t = compileType->getValueDataAsEnum<CompileType>();
	String options = "";
	switch (t)
	{
	case COMPILE_DEBUG:
		options = "--debug";
		break;

	case COMPILE_OPTIMIZED:
		options = "-O3s --noAssert";
		break;

	case COMPILE_TINY:
		options = "-O3z --noAssert --runtime stub --use abort =";
		break;
	}

	if (lowMemory->boolValue())
	{
		options += " --lowMemoryLimit";
	}

	//String buildCommand = "npm run build";
	String arguments = "\"" + f.getFullPathName() + "\" -b \"" + folder.getChildFile(shortName + ".wasm").getFullPathName() + "\" " + /*" -t app.wat " +*/ options;

	String command = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("Bento/wasm/wasmcompiler.exe").getFullPathName() + " " + arguments;

	NLOG(niceName, command);
	ChildProcess p;
	p.start(command);
	String pResult = p.readAllProcessOutput().replace("\r", "");

	//if (silentMode) WinExec(command.getCharPointer(), SW_HIDE);
	//else result = system(command.getCharPointer());

	if (pResult.isEmpty())
	{
		File nwf = getWasmFile();
		NLOG(niceName, "Script has been compiled successfully to " + nwf.getFileName());

		generateParams();

		if (autoUpload->boolValue())
		{
			uploadToPropsTrigger->trigger();
			if (autoLaunch->boolValue())
			{
				Timer::callAfterDelay(200, [this]() {this->loadOnPropsTrigger->trigger(); });
			}
		}
	}
	else
	{
		NLOGERROR(niceName, "Error compiling script : \n" << pResult);
	}
}

void EmbeddedScriptBlock::generateParams()
{
	File f = scriptFile->getFile();
	if (!f.existsAsFile()) return;
	FileInputStream fis(f);

	paramsContainer->clear();

	int paramsState = -1;
	String s = fis.readNextLine();
	while (s.isNotEmpty())
	{
		switch (paramsState)
		{
		case -1:
			if (s.startsWith("/*Parameters")) paramsState = 0;
			break;

		case 0:
			if (s.startsWith("*/")) paramsState = 1;
			else
			{
				StringArray pArr;
				pArr.addTokens(s, ",", "\"");
				if (pArr.size() >= 2)
				{
					paramsContainer->addFloatParameter(pArr[0], pArr[0], pArr[1].getFloatValue(), pArr.size() > 2 ? pArr[2].getFloatValue() : (float)INT32_MIN, pArr.size() > 3 ? pArr[3].getFloatValue() : (float)INT32_MAX);
				}
			}
			break;

		}

		if (paramsState == 1) break;

		s = fis.readNextLine();

	}

}

File EmbeddedScriptBlock::getWasmFile()
{
	File f = scriptFile->getFile();
	if (!f.exists()) return File();
	return f.getParentDirectory().getChildFile(shortName + ".wasm");
}

void EmbeddedScriptBlock::loadScriptOnProp(Prop* p)
{
	if (BentoProp* bp = dynamic_cast<BentoProp*>(p))
	{
		bp->sendControlToProp("script.load", shortName);
	}
}

void EmbeddedScriptBlock::stopScriptOnProp(Prop* p)
{
	if (BentoProp* bp = dynamic_cast<BentoProp*>(p))
	{
		bp->sendControlToProp("script.stop", shortName);
	}
}

void EmbeddedScriptBlock::sendParamControlToProp(Prop* p, int index, float val)
{
	if (BentoProp* bp = dynamic_cast<BentoProp*>(p))
	{
		var args;
		args.append(index);
		args.append(val);
		bp->sendControlToProp("script.setScriptParam", args);
	}
}


void EmbeddedScriptBlock::run()
{
	compile();
}

void EmbeddedScriptBlock::onContainerParameterChangedInternal(Parameter* p)
{
	LightBlockModel::onContainerParameterChangedInternal(p);
	if (p == scriptFile)
	{
		File f = scriptFile->getFile();
		if (f.existsAsFile())
		{
			lastModTime = f.getLastModificationTime();

			if (!isCurrentlyLoadingData) {
				setNiceName(f.getFileNameWithoutExtension());
			}
		}
	}
}

void EmbeddedScriptBlock::onContainerTriggerTriggered(Trigger* t)
{
	if (t == compileTrigger) startThread();
	else if (t == uploadToPropsTrigger)
	{
		File f = getWasmFile();
		if (!f.existsAsFile())
		{
			NLOGWARNING(niceName, "Script " << f.getFileName() << "doesn't exists");
			return;
		}

		for (auto& p : PropManager::getInstance()->items)
		{
			if (BentoProp* bp = dynamic_cast<BentoProp*>(p)) bp->addFileToUpload({ f });
		}
	}
	else if (t == loadOnPropsTrigger)
	{
		for (auto& p : PropManager::getInstance()->items)
		{
			if (p->currentBlock != nullptr && p->currentBlock->provider == this) loadScriptOnProp(p);
		}
	}
	else if (t == stopOnPropsTrigger)
	{
		for (auto& p : PropManager::getInstance()->items)
		{
			if (p->currentBlock != nullptr && p->currentBlock->provider == this) stopScriptOnProp(p);
		}
	}
}

void EmbeddedScriptBlock::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	LightBlockModel::onControllableFeedbackUpdateInternal(cc, c);
	if (cc == paramsContainer.get())
	{
		for (auto& p : PropManager::getInstance()->items)
		{
			int index = paramsContainer->controllables.indexOf(c);
			if (p->currentBlock != nullptr && p->currentBlock->provider == this) sendParamControlToProp(p, index, ((Parameter*)c)->floatValue());
		}
	}
}

void EmbeddedScriptBlock::handleEnterExit(bool enter, Array<Prop*> props)
{
	for (auto& p : props)
	{
		if (enter) loadScriptOnProp(p);
		else stopScriptOnProp(p);
	}
}

void EmbeddedScriptBlock::afterLoadJSONDataInternal()
{
	generateParams();
}


void EmbeddedScriptBlock::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{
	result->fill(Colours::black.withAlpha(.2f));
}

void EmbeddedScriptBlockManager::timerCallback()
{
	for (auto& i : items) ((EmbeddedScriptBlock*)i)->checkAutoCompile();
}
