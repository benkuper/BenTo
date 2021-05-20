/*
 ==============================================================================

 MainComponentMenuBar.cpp
 Created: 25 Mar 2016 6:02:02pm
 Author:  Martin Hermant

 ==============================================================================
 */


#include "MainComponent.h"
#include "Prop/PropIncludes.h"

namespace BentoCommandIDs
{
	static const int updatePropDefinitions = 0x500;
}

void MainComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
	switch (commandID)
	{
	case BentoCommandIDs::updatePropDefinitions:
		result.setInfo("Update Prop definitions", "", "General", result.readOnlyInKeyEditor);
		break;

	default:
		OrganicMainContentComponent::getCommandInfo(commandID, result);
		break;
	}
}



void MainComponent::getAllCommands(Array<CommandID>& commands) {

	OrganicMainContentComponent::getAllCommands(commands);

	const CommandID ids[] = {
		BentoCommandIDs::updatePropDefinitions,
	};

	commands.addArray(ids, numElementsInArray(ids));
}


PopupMenu MainComponent::getMenuForIndex(int topLevelMenuIndex, const String& menuName)
{
	PopupMenu menu = OrganicMainContentComponent::getMenuForIndex(topLevelMenuIndex, menuName);
	return menu;
}

void MainComponent::fillFileMenuInternal(PopupMenu& menu)
{
	menu.addCommandItem(&getCommandManager(), BentoCommandIDs::updatePropDefinitions);
}

bool MainComponent::perform(const InvocationInfo& info)
{

	switch (info.commandID)
	{
	case BentoCommandIDs::updatePropDefinitions:
	{
		PropManager::getInstance()->startThread();
	}
	break;

	default:
		return OrganicMainContentComponent::perform(info);
	}

	return true;
}

StringArray MainComponent::getMenuBarNames()
{
	StringArray names = OrganicMainContentComponent::getMenuBarNames();
	return names;
}
