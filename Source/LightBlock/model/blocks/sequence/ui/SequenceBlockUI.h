/*
  ==============================================================================

    SequenceBlockUI.h
    Created: 23 Apr 2018 11:36:00am
    Author:  Ben

  ==============================================================================
*/

#pragma once

class BentoSequenceBlockUI :
	public LightBlockModelUI
{
public:
	BentoSequenceBlockUI(BentoSequenceBlock* block);
	~BentoSequenceBlockUI();

	BentoSequenceBlock * tb;

	void editBlock() override;
};