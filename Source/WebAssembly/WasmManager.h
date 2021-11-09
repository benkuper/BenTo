/*
  ==============================================================================

    WasmManager.h
    Created: 7 Nov 2021 11:15:18am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "WasmScript.h"

class WasmManager :
    public BaseManager<WasmScript>,
    public Thread,
    public URL::DownloadTask::Listener,
    public Timer
{
public:
    juce_DeclareSingleton(WasmManager, true);
    WasmManager();
    ~WasmManager();

    Trigger* downloadCompiler;
    std::unique_ptr<URL::DownloadTask> downloadTask;

    void run() override;
    void onContainerTriggerTriggered(Trigger* t) override;

    void timerCallback() override;

    // Inherited via Listener
    virtual void progress(URL::DownloadTask* task, int64 downloaded, int64 total) override;
    virtual void finished(URL::DownloadTask* task, bool success) override;
};