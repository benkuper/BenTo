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
	BaseItem("Script")
{
	scriptFile = addFileParameter("Script", "Path to the script");
	scriptFile->fileTypeFilter = "*.ts";

	compileType = addEnumParameter("Compile Type", "");
	compileType->addOption("Optimized", COMPILE_OPTIMIZED)->addOption("Debug", COMPILE_DEBUG)->addOption("Tiny", COMPILE_TINY);

	lowMemory = addBoolParameter("Low Memory", "", false);


	compileTrigger = addTrigger("Compile", "Compiles the script");
	uploadToPropsTrigger = addTrigger("Upload to Props", "");
	launchOnPropsTrigger = addTrigger("Launch on Props", "");

}

WasmScript::~WasmScript()
{
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
	String buildCommand = "npx asc app.ts -b " + shortName + ".wasm -t app.wat " + options;
	String command = "cd \"" + folder.getFullPathName() + "\" && " + buildCommand;
	LOG(command);
	if (silentMode) WinExec(command.getCharPointer(), SW_HIDE);
	else result = system(command.getCharPointer());

	File nwf = getWasmFile();
	if (nwf.existsAsFile())
	{
		NLOG(niceName, "Script has been compiled successfully to " + nwf.getFileName());
	}
	else
	{
		NLOGERROR(niceName, "Error compiling script, file " << nwf.getFullPathName() + " doesn't exist");
	}
}

File WasmScript::getWasmFile()
{
	File f = scriptFile->getFile();
	if (!f.exists()) return File();
	return f.getParentDirectory().getChildFile(shortName + ".wasm");
}

void WasmScript::onContainerTriggerTriggered(Trigger* t)
{
	if (t == compileTrigger) compile();
	else if (t == uploadToPropsTrigger)
	{
		File f = getWasmFile();
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
