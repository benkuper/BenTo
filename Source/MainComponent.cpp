/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

#include "LightBlock/LightBlockIncludes.h"
#include "Prop/PropIncludes.h"
#include "Video/VideoIncludes.h"
#include "Node/NodeIncludes.h"
#include "Sequence/SequenceIncludes.h"
#include "VideoPreview/VideoPreviewPanel.h"
#include "Common/CommonIncludes.h"

//==============================================================================
MainComponent::MainComponent() :
	OrganicMainContentComponent()
{
	getCommandManager().registerAllCommandsForTarget(this);
	SharedTextureManager::getInstance(); //create the main instance
}

MainComponent::~MainComponent()
{
	SharedTextureManager::deleteInstance();
	VizTimer::deleteInstance();
}

void MainComponent::init()
{
	ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition(PropFlasherPanel::getTypeStringStatic(), &PropFlasherPanel::create));
	ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Props", &PropManagerUI::create));
	ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Blocks", &LightBlockModelLibraryUI::create));
	ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Block Visualizer", &BlockViz::create));
	ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition(NodeBlockEditor::getTypeStringStatic(), &NodeBlockEditor::create));
	ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition(BentoSequenceEditor::getTypeStringStatic(), &BentoSequenceEditor::create));
	ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition(SpatializerPanel::getTypeStringStatic(), &SpatializerPanel::create));
	ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition(PropShapePanel::getTypeStringStatic(), &PropShapePanel::create));
	ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Video Preview", &VideoPreviewPanel::create));
	ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("WASM Scripts", &EmbeddedScriptManagerUI::create));

	ShapeShifterManager::getInstance()->setDefaultFileData(BinaryData::default_btlayout);
	ShapeShifterManager::getInstance()->setLayoutInformations("btlayout", "Bento/layouts");

	OrganicMainContentComponent::init();
}