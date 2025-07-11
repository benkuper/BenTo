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
 
    File nodeF = getNodeExecutable();
    if (!nodeF.exists())
    {
        NLOGERROR(niceName, "Node executable not found at " + nodeF.getFullPathName() + ", cannot compile script.");
        return;
    }
    
    StringArray args;
    args.add(nodeF.getFullPathName());
    args.add(getCompilerFile().getFullPathName());
    args.add(f.getFullPathName());
    args.add("-o");
    args.add(folder.getChildFile(shortName + ".wasm").getFullPathName());
    args.add(options);
    
    NLOG(niceName, args.joinIntoString(" "));
    ChildProcess p;
    p.start(args);
    String pResult = p.readAllProcessOutput().replace("\r", "");

    //if (silentMode) WinExec(command.getCharPointer(), SW_HIDE);
    //else result = system(command.getCharPointer());

    if (!pResult.toLowerCase().contains("error"))
    {
        File nwf = getWasmFile();
        NLOG(niceName, "Script has been compiled successfully to " + nwf.getFileName());

        generateParams();

        if (autoUpload->boolValue())
        {
            uploadToPropsTrigger->trigger();
            if (autoLaunch->boolValue())
            {
                Timer::callAfterDelay(200, [this]() { this->loadOnPropsTrigger->trigger(); });
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
    if(bytesTotal == 0) return true;
    float p = bytesSent * 1.0f / bytesTotal;
    NLOG(niceName, "Downloading wasm compiler...  " << String(p * 100, 2) + "%");
    return true;
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
