/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

#include "LightBlock/model/ui/LightBlockModelLibraryUI.h"
#include "Prop/ui/PropManagerUI.h"
#include "BlockViz/BlockVizPanel.h"
#include "Node/ui/NodeBlockEditor.h"
#include "Timeline/TimelineEditor.h"
#include "Video/ui/SpatializerPanel.h"
#include "Prop/Cluster/ui/PropClusterGroupManagerUI.h"

//==============================================================================
MainComponent::MainComponent()
{
	SharedTextureManager::getInstance(); //create the main instance
	openGLContext.setRenderer(this);
}

MainComponent::~MainComponent()
{
	SharedTextureManager::deleteInstance();
}

void MainComponent::init()
{
	ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Props", &PropManagerUI::create));
	ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Blocks", &LightBlockModelLibraryUI::create));
	ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Block Visualizer", &BlockViz::create));
	ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition(NodeBlockEditor::getTypeStringStatic(), &NodeBlockEditor::create));
	ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition(TimelineEditor::getTypeStringStatic(), &TimelineEditor::create));
	ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition(SpatializerPanel::getTypeStringStatic(), &SpatializerPanel::create));
	//ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Clusters", &PropClusterGroupManagerUI::create));

	ShapeShifterManager::getInstance()->setDefaultFileData(BinaryData::default_btlayout);
	ShapeShifterManager::getInstance()->setLayoutInformations("btlayout", "Bento/layouts");

	OrganicMainContentComponent::init();
}

void MainComponent::newOpenGLContextCreated()
{
}

void MainComponent::renderOpenGL()
{
	if (SharedTextureManager::getInstanceWithoutCreating() != nullptr) SharedTextureManager::getInstance()->renderGL();
}

void MainComponent::openGLContextClosing()
{
}
