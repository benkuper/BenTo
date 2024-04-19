/*
  ==============================================================================

    DMXBlock.h
    Created: 10 Apr 2018 6:58:42pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class DMXBlock :
	public LightBlockModel
{
public:
	DMXBlock(var params = var());
	~DMXBlock();

	DECLARE_TYPE("DMX")

};
