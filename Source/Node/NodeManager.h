/*
  ==============================================================================

    NodeManager.h
    Created: 13 Apr 2018 11:25:07pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "Node.h"

class NodeManager :
	public BaseManager<Node>
{
public:
	NodeManager();
	~NodeManager();
};