/*
  ==============================================================================

    WasmManager.cpp
    Created: 7 Nov 2021 11:15:18am
    Author:  bkupe

  ==============================================================================
*/

#include "WasmManager.h"

juce_ImplementSingleton(WasmManager)

WasmManager::WasmManager() :
    BaseManager("WASM Scripts"),
    Thread("DownloadCompiler")
{
    downloadCompiler = addTrigger("Download compiler", "Download the embedded compiler");
    startTimerHz(1);
}

WasmManager::~WasmManager()
{
}

void WasmManager::run()
{
    File f = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("Bento/wasm/wasmcompiler.exe");

    if (f.existsAsFile())
    {
        bool result = AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon, "Compiler already exists", "There is already a compiler found. Do you want to replace it ?", "Go ahead", "No way");
        if (result) f.deleteFile();
        else return;
    }

    URL url("http://benjamin.kuperberg.fr/bento/download/wasmcompiler.exe");
    LOG("Downloading compiler...");
    downloadTask = url.downloadToFile(f, "", this);
    if (downloadTask == nullptr)
    {
        LOGERROR("Error downloading compiler");
    }
}

void WasmManager::onContainerTriggerTriggered(Trigger* t)
{
    if (t == downloadCompiler)
    {
        startThread();
    }
}

void WasmManager::timerCallback()
{
    for (auto& i : items) i->checkAutoCompile();
}

void WasmManager::progress(URL::DownloadTask* task, int64 downloaded, int64 total)
{
    int percent = downloaded *100/ total;
    LOG(percent << "%");
}

void WasmManager::finished(URL::DownloadTask* task, bool success)
{
    LOG("Compiler is downloaded !");

}
