/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

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

private:
    //==============================================================================
    // Your private member variables go here...


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)


	virtual void newOpenGLContextCreated() override;

	virtual void renderOpenGL() override;

	virtual void openGLContextClosing() override;

};
