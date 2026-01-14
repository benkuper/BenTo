/*
  ==============================================================================

	EmbeddedScriptBlock.cpp
	Created: 9 Nov 2021 6:06:13pm
	Author:  bkupe

  ==============================================================================
*/

#include "Prop/PropIncludes.h"
#include "EmbeddedScriptBlock.h"

EmbeddedScriptBlock::EmbeddedScriptBlock(var params) :
	LightBlockModel(getTypeString(), params),
	Thread("WasmCompile"),
	wasmEngine(this)
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

	addChildControllableContainer(&wasmEngine);
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
		if (!isThreadRunning())
		{
			stopThread(100);
			startThread();
		}
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

	File nodeF = getNodeExecutable();
	if (!nodeF.exists())
	{
		NLOGERROR(niceName, "Node executable not found at " + nodeF.getFullPathName() + ", cannot compile script.");
		return;
	}


	String modifiedFileString = generateParams();

	File metaDataFile = getMetadataFile();
	if (metaDataFile.existsAsFile()) metaDataFile.deleteFile();
	FileOutputStream metaDataFOS(metaDataFile);
	metaDataFOS.writeString(JSON::toString(scriptMetaData, true));
	metaDataFOS.flush();


	File toCompileF = folder.getChildFile("compile_temp.ts");
	if (toCompileF.existsAsFile()) toCompileF.deleteFile();

	FileOutputStream toCompileFOS(toCompileF);
	toCompileFOS.writeText(modifiedFileString, false, false, "\n");
	toCompileFOS.flush();

	File compiledFile = folder.getChildFile(shortName + ".wasm");

	StringArray args;
	args.add(nodeF.getFullPathName());
	args.add(getCompilerFile().getFullPathName());
	args.add(toCompileF.getFullPathName());
	args.add("-o");
	args.add(compiledFile.getFullPathName());
	args.add(options);

	NLOG(niceName, args.joinIntoString(" "));
	ChildProcess p;
	p.start(args);
	String pResult = p.readAllProcessOutput().replace("\r", "");

	//if (silentMode) WinExec(command.getCharPointer(), SW_HIDE);
	//else result = system(command.getCharPointer());

	bool success = false;

	if (!pResult.toLowerCase().contains("error"))
	{
		File nwf = getWasmFile();
		NLOG(niceName, "Script has been compiled successfully to " + nwf.getFileName());


		if (autoUpload->boolValue())
		{
			uploadToPropsTrigger->trigger();
			if (autoLaunch->boolValue())
			{
				Timer::callAfterDelay(200, [this]() { this->loadOnPropsTrigger->trigger(); });
			}
		}

		success = true;
	}
	else
	{
		NLOGERROR(niceName, "Error compiling script : \n" << pResult);
	}

	if (success)
	{
		if (wasmEngine.enabled->boolValue()) wasmEngine.init(compiledFile);
	}

}

String EmbeddedScriptBlock::generateParams()
{
	File f = scriptFile->getFile();
	if (!f.existsAsFile()) return "";

	paramsContainer->clear();

	scriptMetaData = var(new DynamicObject());
	var scriptVariablesData;
	var scriptFunctionsData;



	String fileData = f.loadFileAsString();

	StringArray variablesMatch = RegexFunctions::getFirstMatch(R"(\/\*\s*public\s+variables\s*\*\/([\s\S]*?)\/\*\s*end\s+public\s+variables\s*\*\/)", fileData);

	if (!variablesMatch.isEmpty())
	{
		Array<StringArray> varsLines = RegexFunctions::getAllMatches(R"(let\s+([a-zA-Z0-9_]+)\s*:\s*([a-zA-Z0-9_]+)\s*=\s*([^;]+);\s*(?:\/\/s*(.*))?)", variablesMatch[1]);

		for (auto varMatch : varsLines)
		{
			String name = varMatch[1];
			String type = varMatch[2];
			String valueStr = varMatch[3].trim();
			String metadata = varMatch[4].trim();

			String niceName = StringUtil::toNiceName(name);

			Parameter* p = nullptr;

			if (type == "f32" || type == "f64" || type == "float")
			{
				p = new FloatParameter(niceName, "", valueStr.getFloatValue());
			}
			else if (type == "boolean" || type == "bool")
			{
				p = new BoolParameter(niceName, "", valueStr == "true");
			}
			else if (type == "u32" || type == "u64")
			{
				p = new IntParameter(niceName, "", valueStr.getIntValue(), 0);
			}
			else if (type == "i32" || type == "i64")
			{
				p = new IntParameter(niceName, "", valueStr.getIntValue());
			}

			if (p != nullptr)
			{
				var varData = var(new DynamicObject());
				varData.getDynamicObject()->setProperty("name", name);
				varData.getDynamicObject()->setProperty("type", type);

				if (metadata.isNotEmpty())
				{
					StringArray metaParts = StringArray::fromTokens(metadata, ";", "");
					for (auto& part : metaParts)
					{
						part = part.trim();
						if (part == "feedback" || part == "readonly")
						{
							p->setControllableFeedbackOnly(true);
							varData.getDynamicObject()->setProperty("feedback", true);
						}
						else if (part.contains(":")) // Range
						{
							float min = part.upToFirstOccurrenceOf(":", false, false).getFloatValue();
							float max = part.fromFirstOccurrenceOf(":", false, false).getFloatValue();
							p->setRange(min, max);
							
							varData.getDynamicObject()->setProperty("min", min);
							varData.getDynamicObject()->setProperty("max", max);
						}
					}
				}

				p->setCustomShortName(name);
				paramsContainer->addParameter(p);
				scriptVariablesData.append(varData);
			}
		}

		scriptMetaData.getDynamicObject()->setProperty("variables", scriptVariablesData);

	}

	StringArray functionsMatch = RegexFunctions::getFirstMatch(R"(\/\*\s*public\s+functions\s*\*\/([\s\S]*?)\/\*\s*end\s+public\s+functions\s*\*\/)", fileData);

	if (!functionsMatch.isEmpty())
	{
		Array<StringArray> functions = RegexFunctions::getAllMatches(R"(export\s+function\s+([a-zA-Z0-9_]+)\s*\()", functionsMatch[1]);

		for (auto fMatch : functions)
		{
			String name = fMatch[1];
			String niceName = StringUtil::toNiceName(name);
			paramsContainer->addTrigger(niceName, "Calls " + name + "() in the script");
			var funcData = var(new DynamicObject());
			funcData.getDynamicObject()->setProperty("name", name);
			funcData.getDynamicObject()->setProperty("niceName", niceName);
			scriptFunctionsData.append(funcData);
		}

		scriptMetaData.getDynamicObject()->setProperty("functions", scriptFunctionsData);
	}


	StringArray eventsMatch = RegexFunctions::getFirstMatch(R"(const\s+enum\s+Events\s*\{\s*([a-zA-Z0-9_,\s]+)\s*\};)", fileData);

	if (!eventsMatch.isEmpty())
	{
		StringArray events = StringArray::fromTokens(eventsMatch[1], ",", "");
		var eventsData;
		for (auto& eventName : events) eventsData.append(eventName.trim());

		scriptMetaData.getDynamicObject()->setProperty("events", eventsData);
	}

	String generatedCode;

	if (scriptVariablesData.size() > 0)
	{
		String setParamFunc = "export function setParam(paramIndex: i32, value: f32): void {\n \
			switch (paramIndex) { \n";


		for (int i = 0; i < scriptVariablesData.size(); i++)
		{
			var varData = scriptVariablesData[i];
			String name = varData["name"].toString();
			String type = varData["type"].toString();

			//replace sendFeedback
			fileData = fileData.replace("sendFeedback(" + name + ")", "dev.sendParamFeedback(" + String(i) + ", " + name + ")");

			String caseStr = "case " + String(i) + ": " + name + " = ";

			if (type == "boolean" || type == "bool")
			{
				caseStr += "value > 0.5; break;\n";
			}
			else if (type == "u32" || type == "u64" || type == "i32" || type == "i64")
			{
				caseStr += "value as " + type + "; break;\n";
			}
			else if (type == "f32" || type == "f64" || type == "float")
			{
				caseStr += "value as " + type + "; break;\n";
			}
			else {
				continue;
			}
			setParamFunc += caseStr;
		}



		setParamFunc += "default: break;\n} \n}\n";
		generatedCode += setParamFunc;

		//NLOG(niceName, "Set Param Func : " << setParamFunc);
	}

	if (scriptFunctionsData.size() > 0)
	{
		String triggerSetFunc = "export function triggerFunction(funcIndex: i32): void {\n \
				switch (funcIndex) { \n";

		for (int i = 0; i < scriptFunctionsData.size(); i++)
		{
			var funcData = scriptFunctionsData[i];
			String name = funcData["name"].toString();
			triggerSetFunc += "case " + String(i) + ": " + name + "(); break;\n";
		}

		triggerSetFunc += "default: break;\n} \n}\n";

		generatedCode += triggerSetFunc;
	}


	if (generatedCode.isNotEmpty())
	{
		generatedCode = "\n// Auto-generated parameter setting functions\n" + generatedCode;
	}

	return fileData + generatedCode;

}


File EmbeddedScriptBlock::getToolsFolder() const
{
	File f = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("BenTo/tools");
	if (!f.exists()) f.createDirectory();
	return f;
}

File EmbeddedScriptBlock::getCompilerFile()
{
	File f = getToolsFolder().getChildFile("node_modules/assemblyscript/bin/asc.js");
	if (!f.exists())
	{
		NLOGWARNING(niceName, "AssemblyScript compiler not found at " + f.getFullPathName() + ", downloading it now...");
		downloadWasmCompiler();
	}

	return f;
}

File EmbeddedScriptBlock::getNodeExecutable()
{
#if JUCE_WINDOWS
	String nodeFolder = "node-win";
	String nodeExecutableName = "node.exe";
#elif JUCE_LINUX
	String nodeFolder = "node-linux";
	String nodeExecutableName = "bin/node";
#elif JUCE_MAC
	String nodeFolder = "node-mac";
	String nodeExecutableName = "bin/node";
#endif

	File f = getToolsFolder().getChildFile(nodeFolder);
	if (!f.exists()) downloadWasmCompiler();

	File nodeExecutable = f.getChildFile(nodeExecutableName);

	if (!nodeExecutable.exists())
	{
		NLOGWARNING(niceName, "Node executable not found at " + nodeExecutable.getFullPathName() + ", downloading it now...");
		downloadWasmCompiler();
	}

	return nodeExecutable;
}

void EmbeddedScriptBlock::downloadWasmCompiler()
{
#if JUCE_WINDOWS
	String zipName = "wasm-win";
#elif JUCE_LINUX
	String zipName = "wasm-linux";
#elif JUCE_MAC
	String zipName = "wasm-mac";
#endif

	String url = "https://benjamin.kuperberg.fr/bento/download/" + zipName + ".zip";
	File toolsFolder = getToolsFolder();

	//download and extract
	URL downloadUrl(url);
	std::unique_ptr<InputStream> stream(downloadUrl.createInputStream(URL::InputStreamOptions(URL::ParameterHandling::inPostData).withConnectionTimeoutMs(10000).withProgressCallback(std::bind(&EmbeddedScriptBlock::wasmDownloadProgressCallback, this, std::placeholders::_1, std::placeholders::_2))));

	NLOG(niceName, "Downloading wasm compiler from " + url);

	if (stream != nullptr)
	{
		File downloadedFile = toolsFolder.getChildFile(zipName + ".zip");
		if (downloadedFile.existsAsFile()) downloadedFile.deleteFile();
		FileOutputStream fos(downloadedFile);
		fos.writeFromInputStream(*stream, -1);
		fos.flush();

		ZipFile zipFile(downloadedFile);
		if (zipFile.uncompressTo(toolsFolder))
		{
			NLOG(niceName, "Wasm compiler downloaded and extracted to " + toolsFolder.getFullPathName());
			downloadedFile.deleteFile();
		}
		else
		{
			NLOGERROR(niceName, "Error extracting wasm compiler from " + downloadedFile.getFullPathName());
			return;
		}
	}
	else
	{
		NLOGERROR(niceName, "Error downloading wasm compiler from " + url);
		return;
	}

	if (getCompilerFile().exists() && getNodeExecutable().exists())
	{
		NLOG(niceName, "Wasm compiler and Node.js executable are ready to use.");
	}
	else
	{
		NLOGERROR(niceName, "Wasm compiler or Node.js executable not found after download.");
		return;
	}

#if JUCE_MAC || JUCE_LINUX
	// Make the node executable executable
	File nodeExecutable = getNodeExecutable();
	if (nodeExecutable.setExecutePermission(true))
	{
		NLOG(niceName, "Node executable permissions set to executable.");
	}
	else
	{
		NLOGERROR(niceName, "Node executable couldn't be set to executable");
		return;
	}

#endif


}

bool EmbeddedScriptBlock::wasmDownloadProgressCallback(int bytesSent, int bytesTotal)
{
	if (threadShouldExit()) return false;
	if (bytesTotal == 0) return true;
	float p = bytesSent * 1.0f / bytesTotal;
	NLOG(niceName, "Downloading wasm compiler...  " << String(p * 100, 2) + "%");
	return true;
}

File EmbeddedScriptBlock::getWasmFile()
{
	File f = scriptFile->getFile();
	if (f.exists()) return f.getParentDirectory().getChildFile(shortName + ".wasm");
	return File();
}

File EmbeddedScriptBlock::getMetadataFile()
{
	File f = scriptFile->getFile();
	if (f.exists()) return f.getParentDirectory().getChildFile(shortName + "_metadata.wmeta");
	return File();
}

void EmbeddedScriptBlock::loadScriptOnProps(Prop* p)
{
	Array<Prop*> props;
	if (p != nullptr) props.add(p);
	else props = getAssignedProps();

	for (auto* p : props)
	{
		if (BentoProp* bp = dynamic_cast<BentoProp*>(p))
		{
			bp->sendControlToProp("script.load", shortName);
		}
	}
}

void EmbeddedScriptBlock::stopScriptOnProps(Prop* p)
{
	Array<Prop*> props;
	if (p != nullptr) props.add(p);
	else props = getAssignedProps();

	for (auto* p : props)
	{
		if (BentoProp* bp = dynamic_cast<BentoProp*>(p))
		{
			bp->sendControlToProp("script.stop", shortName);
		}
	}
}

void EmbeddedScriptBlock::sendParamControlToProps(const String& paramName, float val, Prop* p)
{
	Array<Prop*> props;
	if (p != nullptr) props.add(p);
	else props = getAssignedProps();
	for (auto* p : props)
	{
		if (BentoProp* bp = dynamic_cast<BentoProp*>(p))
		{
			var args;
			args.append(paramName);
			args.append(val);
			bp->sendControlToProp("script.setParam", args);
		}
	}
}

void EmbeddedScriptBlock::sendTriggerFunctionToProps(const String& functionName, Prop* p)
{
	Array<Prop*> props;
	if (p != nullptr) props.add(p);
	else props = getAssignedProps();

	for (auto* p : props)
	{
		if (BentoProp* bp = dynamic_cast<BentoProp*>(p))
		{
			var args;
			args.append(functionName);
			bp->sendControlToProp("script.trigger", args);
		}
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
			generateParams();

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
		File mf = getMetadataFile();

		if (!f.existsAsFile())
		{
			NLOGWARNING(niceName, "Script " << f.getFileName() << "doesn't exists");
			return;
		}

		for (auto& p : PropManager::getInstance()->items)
		{
			if (BentoProp* bp = dynamic_cast<BentoProp*>(p))
			{
				bp->addFileToUpload({ f });
				bp->addFileToUpload({ mf });
			}
		}
	}
	else if (t == loadOnPropsTrigger)
	{
		loadScriptOnProps();
	}
	else if (t == stopOnPropsTrigger)
	{
		stopScriptOnProps();
	}
}

void EmbeddedScriptBlock::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	LightBlockModel::onControllableFeedbackUpdateInternal(cc, c);
	if (cc == paramsContainer.get())
	{
		if (c->type == Controllable::TRIGGER)
		{
			if (wasmEngine.enabled->boolValue())
			{
				wasmEngine.triggerFunction(c->shortName);
			}
			sendTriggerFunctionToProps(c->shortName);
		}
		else
		{
			if (wasmEngine.enabled->boolValue())
			{
				wasmEngine.setScriptParam(c->shortName, ((Parameter*)c)->floatValue());
			}
			sendParamControlToProps(c->shortName, ((Parameter*)c)->floatValue());
		}

	}
}

void EmbeddedScriptBlock::handleEnterExit(bool enter, Array<Prop*> props)
{
	for (auto& p : props)
	{
		if (enter) loadScriptOnProps(p);
		else stopScriptOnProps(p);
	}
}

var EmbeddedScriptBlock::getJSONData(bool includeNonOverriden)
{
	var data = LightBlockModel::getJSONData(includeNonOverriden);
	data.getDynamicObject()->setProperty(wasmEngine.shortName, wasmEngine.getJSONData(includeNonOverriden));
	return data;
}

void EmbeddedScriptBlock::loadJSONDataItemInternal(var data)
{
	LightBlockModel::loadJSONDataItemInternal(data);
	wasmEngine.loadJSONData(data.getProperty(wasmEngine.shortName, var()));
}

void EmbeddedScriptBlock::afterLoadJSONDataInternal()
{
	LightBlockModel::afterLoadJSONDataInternal();
	if (autoCompile->boolValue())
	{
		compile();
	}
}

void EmbeddedScriptBlock::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{
	if (!p->isFake || !wasmEngine.enabled->boolValue()) //only send to fake props with wasmEngine enabled
	{
		return;
	}

	GenericScopedLock lock(wasmEngine.ledColors.getLock());
	for (int i = 0; i < jmin(resolution, wasmEngine.ledColors.size()); i++)
		result->set(i, wasmEngine.ledColors[i]);
}

void EmbeddedScriptBlockManager::timerCallback()
{
	for (auto& i : items) ((EmbeddedScriptBlock*)i)->checkAutoCompile();
}
