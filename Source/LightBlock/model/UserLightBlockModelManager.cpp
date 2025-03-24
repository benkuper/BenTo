/*
  ==============================================================================

    UserLightBlockModelManager.cpp
    Created: 10 Apr 2018 7:48:55pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlock/LightBlockIncludes.h"
#include "Sequence/SequenceIncludes.h"

UserLightBlockModelManager::UserLightBlockModelManager(const String &name, BlockType type) :
	BaseManager(name),
	type(type)
{
	itemDataType = name;
	libFolder = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(String(ProjectInfo::projectName)+"/" + name);
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
	case STREAMING_SCRIPT: return new StreamingScriptBlock();
	case SEQUENCE:
	{
		BentoSequenceBlock * t = new BentoSequenceBlock();

		if (!Engine::mainEngine->isLoadingFile)
		{
			ShapeShifterManager::getInstance()->showContent(BentoSequenceEditor::getTypeStringStatic());
			BentoSequenceEditor * te = (BentoSequenceEditor*)ShapeShifterManager::getInstance()->getContentForName(BentoSequenceEditor::getTypeStringStatic());
			if (te != nullptr) te->setSequence(t->sequence.get());
		}
		
		return t;
	}

	case LIVE_FEED: return new SharedTextureBlock();
	case VIDEO: return new VideoFileBlock();

	case EMBEDDED_SCRIPT: return new EmbeddedScriptBlock();

	default:
		break;
	}
	jassertfalse;
	return nullptr;
}

var UserLightBlockModelManager::getJSONData(bool includeNonOverriden)
{
	var data = BaseManager::getJSONData(includeNonOverriden);

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
		}else if(SequenceBlock * t = dynamic_cast<SequenceBlock *>(m))
		{
			DBG("Edit sequence block"); 
			TimelineEditor * te = (TimelineEditor *)ShapeShifterManager::getInstance()->getContentForName(TimelineEditor::getTypeStringStatic());
			if (te != nullptr) te->setSequence(&t->sequence);
		}
		
	}*/
}
