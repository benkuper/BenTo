/*
  ==============================================================================

    UserLightBlockModelManager.cpp
    Created: 10 Apr 2018 7:48:55pm
    Author:  Ben

  ==============================================================================
*/

#include "UserLightBlockModelManager.h"
#include "blocks/node/NodeBlock.h"
#include "blocks/script/ScriptBlock.h"
#include "blocks/timeline/TimelineBlock.h"
#include "blocks/picture/PictureBlock.h"

#include "Timeline/TimelineEditor.h"
#include "Node/ui/NodeBlockEditor.h"

UserLightBlockModelManager::UserLightBlockModelManager(const String &name, BlockType type) :
	BaseManager(name),
	type(type)
{
	itemDataType = name;
	libFolder = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("Bento/" + name);
	libFolder.createDirectory();
}

UserLightBlockModelManager::~UserLightBlockModelManager()
{
	
}

LightBlockModel * UserLightBlockModelManager::createItem()
{
	switch (type)
	{
	case PICTURE: return new PictureBlock();
	case NODE: 
	{
		NodeBlock * n = new NodeBlock();

		if (!Engine::mainEngine->isLoadingFile)
		{
			ShapeShifterManager::getInstance()->showContent(NodeBlockEditor::getTypeStringStatic());
			
			NodeBlockEditor * ne = (NodeBlockEditor *)ShapeShifterManager::getInstance()->getContentForName(NodeBlockEditor::getTypeStringStatic());
			if (ne != nullptr)
			{
				ne->setNodeBlock(n);
			}
		}
		
		return n;
	}
	case SCRIPT: return new ScriptBlock();
	case TIMELINE:
	{
		TimelineBlock * t = new TimelineBlock();

		if (!Engine::mainEngine->isLoadingFile)
		{
			ShapeShifterManager::getInstance()->showContent(TimelineEditor::getTypeStringStatic());
			TimelineEditor * te = (TimelineEditor *)ShapeShifterManager::getInstance()->getContentForName(TimelineEditor::getTypeStringStatic());
			if (te != nullptr) te->setSequence(&t->sequence);
		}
		
		return t;
	}

	default:
		break;
	}
	jassertfalse;
	return nullptr;
}

var UserLightBlockModelManager::getJSONData()
{
	var data = BaseManager::getJSONData();

	/*
	LightBlockModel * editingBlock = nullptr;
	for (auto & i : items)
	{
		if (i->isBeingEdited)
		{
			editingBlock = i;
			break;
		}
	}
	*/

	//if (editingBlock != nullptr) data.getDynamicObject()->setProperty("editingBlock", editingBlock->shortName);
	return data;
}

void UserLightBlockModelManager::loadJSONDataInternal(var data)
{
	BaseManager::loadJSONDataInternal(data);
	/*if (data.hasProperty("editingBlock"))
	{
		
		LightBlockModel * m = getItemWithName(data.getProperty("editingBlock", ""));

		if (NodeBlock * nb = dynamic_cast<NodeBlock *>(m))
		{
			DBG("Edit node block");
			NodeBlockEditor * ne = (NodeBlockEditor *)ShapeShifterManager::getInstance()->getContentForName(NodeBlockEditor::getTypeStringStatic());
			if (ne != nullptr) ne->setNodeBlock(nb);
		}else if(TimelineBlock * t = dynamic_cast<TimelineBlock *>(m))
		{
			DBG("Edit timeline block"); 
			TimelineEditor * te = (TimelineEditor *)ShapeShifterManager::getInstance()->getContentForName(TimelineEditor::getTypeStringStatic());
			if (te != nullptr) te->setSequence(&t->sequence);
		}
		
	}*/
}
