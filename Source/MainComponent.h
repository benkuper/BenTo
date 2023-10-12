/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
using namespace juce;

ApplicationProperties& getAppProperties();
ApplicationCommandManager& getCommandManager();

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent :
	public OrganicMainContentComponent,
	public OpenGLRenderer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

	void init() override;
	
	void setupOpenGLInternal() override;
private:
    //==============================================================================
    // Your private member variables go here...


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)


	virtual void newOpenGLContextCreated() override;

	virtual void renderOpenGL() override;

	virtual void openGLContextClosing() override;


public:
	void getAllCommands(Array<CommandID>& commands) override;
	virtual void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
	virtual bool perform(const InvocationInfo& info) override;
	StringArray getMenuBarNames() override;
	virtual PopupMenu getMenuForIndex(int topLevelMenuIndex, const String& menuName) override;
	void fillFileMenuInternal(PopupMenu& menu) override;
};
