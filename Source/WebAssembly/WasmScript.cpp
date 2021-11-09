/*
  ==============================================================================

	WasmScript.cpp
	Created: 7 Nov 2021 11:15:33am
	Author:  bkupe

  ==============================================================================
*/

#include "WasmScript.h"
#include "Prop/PropIncludes.h"

WasmScript::WasmScript() :
	BaseItem("Script"),
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
	autoCompile = addBoolParameter("Auto Upload", "", true);
	launchOnPropsTrigger = addTrigger("Launch on Props", "");
	autoLaunch = addBoolParameter("Auto Launch", "", true);

}

WasmScript::~WasmScript()
{
}

void WasmScript::checkAutoCompile()
{
	if (!autoCompile->boolValue()) return;
	File f = scriptFile->getFile();
	if (!f.exists()) return;
	Time t = f.getLastModificationTime();
	if (t > lastModTime)
	{
		compile();
	}
}

void WasmScript::compile()
{
	File f = scriptFile->getFile();
	if (!f.exists()) return;
	File folder = f.getParentDirectory();

	File wf = getWasmFile();
	if (wf.exists()) wf.deleteFile();

	bool silentMode = false;
	bool result = true;

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
	String arguments = f.getFullPathName() + " -b " + folder.getChildFile(shortName + ".wasm").getFullPathName() + " " + /*" -t app.wat " +*/ options;

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
		if (autoUpload->boolValue())
		{
			uploadToPropsTrigger->trigger();
			if (autoLaunch->boolValue())
			{
				std::function<void()> func = std::bind(&Trigger::trigger, launchOnPropsTrigger);
				Timer::callAfterDelay(500, func);
			}
		}
	}
	else
	{
		NLOGERROR(niceName, "Error compiling script : \n" << pResult);
	}
}

File WasmScript::getWasmFile()
{
	File f = scriptFile->getFile();
	if (!f.exists()) return File();
	return f.getParentDirectory().getChildFile(shortName + ".wasm");
}

void WasmScript::run()
{
	compile();
}

void WasmScript::onContainerParameterChangedInternal(Parameter* p)
{
	if (p == scriptFile)
	{
		File f = scriptFile->getFile();
		if (f.existsAsFile()) lastModTime = f.getLastModificationTime();
	}
}

void WasmScript::onContainerTriggerTriggered(Trigger* t)
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
			p->addFileToUpload(f);
		}
	}
	else if (t == launchOnPropsTrigger)
	{
		for (auto& p : PropManager::getInstance()->items)
		{
			p->sendControlToProp("scripts.load", shortName);
		}
	}
}
