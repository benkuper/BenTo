/*
  ==============================================================================

    UserLightBlockModelManager.h
    Created: 10 Apr 2018 7:48:55pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class UserLightBlockModelManager :
	public BaseManager<LightBlockModel>
{
public:
	enum BlockType { PICTURE, NODE, SCRIPT, TIMELINE, LIVE_FEED, VIDEO, WASM };

	UserLightBlockModelManager(const String &name, BlockType type);
	virtual ~UserLightBlockModelManager();

	BlockType type;
	File libFolder;

	LightBlockModel * createItem() override;


	var getJSONData() override;
	void loadJSONDataInternal(var data) override;
};
