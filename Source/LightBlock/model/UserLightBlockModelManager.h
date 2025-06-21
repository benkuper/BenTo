/*
  ==============================================================================

    UserLightBlockModelManager.h
    Created: 10 Apr 2018 7:48:55pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class UserLightBlockModelManager :
	public Manager<LightBlockModel>
{
public:
	enum BlockType { PICTURE, NODE, STREAMING_SCRIPT, SEQUENCE, LIVE_FEED, VIDEO, EMBEDDED_SCRIPT};

	UserLightBlockModelManager(const String &name, BlockType type);
	virtual ~UserLightBlockModelManager();

	BlockType type;
	File libFolder;

	LightBlockModel * createItem() override;


	var getJSONData(bool includeNonOverriden = false) override;
	void loadJSONDataInternal(var data) override;
};
