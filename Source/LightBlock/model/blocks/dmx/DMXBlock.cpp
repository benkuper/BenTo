/*
  ==============================================================================

    DMXBlock.cpp
    Created: 10 Apr 2018 6:58:42pm
    Author:  Ben

  ==============================================================================
*/

DMXBlock::DMXBlock(var params) :
	LightBlockModel(getTypeString(), params)
{
	saveAndLoadName = false;
	saveType = false;
}

DMXBlock::~DMXBlock()
{
}
