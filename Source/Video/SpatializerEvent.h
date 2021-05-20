/*
  ==============================================================================

    SpatializerEvent.h
    Created: 5 Feb 2019 8:23:23pm
    Author:  jonglissimo

  ==============================================================================
*/

#pragma once

class  SpatializerEvent
{
public:
	enum Type { LAYOUT_CHANGED };

	SpatializerEvent(Type t) : type(t) {}
	Type type;
};

typedef QueuedNotifier<SpatializerEvent>::Listener SpatializerAsyncListener;
