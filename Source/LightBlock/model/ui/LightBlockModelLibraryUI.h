/*
  ==============================================================================

    LightBlockLibraryUI.h
    Created: 10 Apr 2018 10:58:08pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class LightBlockModelLibraryUI :
	public ShapeShifterContentComponent,
	public ComponentListener,
	public ContainerAsyncListener
{
public:
	LightBlockModelLibraryUI(const String &contentName, LightBlockModelLibrary * library);
	~LightBlockModelLibraryUI();

	Viewport viewport;
	Component container;

	std::unique_ptr<IntSliderUI> iconSizeUI;

	LightBlockModelLibrary * library;
	LightBlockModelGroupUI patternGroupUI;
	LightBlockModelManagerUI nodeBlocksManagerUI;
	LightBlockModelManagerUI sequenceBlocksManagerUI;
	LightBlockModelManagerUI pictureBlocksManagerUI;
	LightBlockModelManagerUI streamingScriptBlocksManagerUI;
	LightBlockModelManagerUI embeddedScriptBlocksManagerUI;
	LightBlockModelManagerUI videoManagerUI;

	//LightBlockModelGroupUI genericFilterGroupUI;


	void paint(Graphics &g) override;
	void resized() override;

	void newMessage(const ContainerAsyncEvent &e) override;

	void componentMovedOrResized(Component &, bool, bool) override;

	static LightBlockModelLibraryUI * create(const String &contentName) { return new LightBlockModelLibraryUI(contentName, LightBlockModelLibrary::getInstance()); }

	
};