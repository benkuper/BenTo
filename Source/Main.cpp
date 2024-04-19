/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include "JuceHeader.h"
#include "BentoEngine.h"
#include "MainComponent.h"
#include "Common/CommonIncludes.h"

//==============================================================================
class BenToApplication  :
	public OrganicApplication
{
public:
    //==============================================================================
    BenToApplication() : OrganicApplication("BenTo", true, ImageCache::getFromMemory(BinaryData::icon_png, BinaryData::icon_pngSize)) 
    {
    }

    bool moreThanOneInstanceAllowed() { return false; }

    //==============================================================================
    void initialiseInternal (const String& commandLine) override
    {
		AppUpdater::getInstance()->setURLs("http://benjamin.kuperberg.fr/bento/update.json", "http://benjamin.kuperberg.fr/bento/download/", "BenTo");
		engine.reset(new BentoEngine());
		mainComponent.reset(new MainComponent());
    }
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (BenToApplication)
