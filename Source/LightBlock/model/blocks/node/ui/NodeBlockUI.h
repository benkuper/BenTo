/*
  ==============================================================================

    NodeBlockUI.h
    Created: 23 Apr 2018 11:34:54am
    Author:  Ben

  ==============================================================================
*/

#pragma once


class NodeBlockUI :
	public LightBlockModelUI
{
public:
	NodeBlockUI(NodeBlock * block);
	~NodeBlockUI();

	NodeBlock * nb;

	void editBlock() override;
};