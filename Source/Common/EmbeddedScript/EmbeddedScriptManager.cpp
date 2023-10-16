/*
  ==============================================================================

    EmbeddedScriptManager.cpp
    Created: 7 Nov 2021 11:15:18am
    Author:  bkupe

  ==============================================================================
*/

//#include "Common/CommonIncludes.h"
//
//juce_ImplementSingleton(EmbeddedScriptManager)
//
//EmbeddedScriptManager::EmbeddedScriptManager() :
//    BaseManager("WASM Scripts"),
//    Thread("DownloadCompiler")
//{
//    downloadCompiler = addTrigger("Download compiler", "Download the embedded compiler");
//    startTimerHz(1);
//}
//
//EmbeddedScriptManager::~EmbeddedScriptManager()
//{
//}
//
//void EmbeddedScriptManager::run()
//{
//    File f = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("Bento/wasm/wasmcompiler.exe");
//
//    std::function<void(File& f)> func = [this](File& f)
//    {
//        if (f.existsAsFile()) return;
//
//        URL url("http://benjamin.kuperberg.fr/bento/download/wasmcompiler.exe");
//        LOG("Downloading compiler...");
//        downloadTask = url.downloadToFile(f,
//            URL::DownloadTaskOptions().withListener(this));
//
//        if (downloadTask == nullptr)
//        {
//            LOGERROR("Error downloading compiler");
//        }
//    };
//
//    if (f.existsAsFile())
//    {
//        AlertWindow::showAsync(
//            MessageBoxOptions().withIconType(AlertWindow::QuestionIcon)
//            .withTitle("Compiler already exists")
//            .withMessage("There is already a compiler found. Do you want to replace it ?")
//            .withButton("Go ahead")
//            .withButton("No way"),
//            [&f, func](int result)
//            {
//                if (result) f.deleteFile();
//                else return;
//
//                func(f);
//            }
//        );
//    }
//    else
//    {
//        func(f);
//    }
//}
//
//void EmbeddedScriptManager::onContainerTriggerTriggered(Trigger* t)
//{
//    if (t == downloadCompiler)
//    {
//        startThread();
//    }
//}
//
//void EmbeddedScriptManager::timerCallback()
//{
//    for (auto& i : items) i->checkAutoCompile();
//}
//
//void EmbeddedScriptManager::progress(URL::DownloadTask* task, int64 downloaded, int64 total)
//{
//    int percent = downloaded *100/ total;
//    LOG(percent << "%");
//}
//
//void EmbeddedScriptManager::finished(URL::DownloadTask* task, bool success)
//{
//    LOG("Compiler is downloaded !");
//
//}
