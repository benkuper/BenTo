/*
  ==============================================================================

    Node.h
    Created: 14 Apr 2018 2:11:31am
    Author:  Ben

  ==============================================================================
*/

#pragma once

class NodeViewUI;
class NodeConnection;
class ParameterSlot;
class ColorSlot;

class Node :
	public BaseItem,
	public NodeConnectionSlot::SlotListener
{
public:
	Node(const String &name = "Node", var params = var());
	virtual ~Node();

	OwnedArray<NodeConnectionSlot, CriticalSection> inSlots;
	OwnedArray<NodeConnectionSlot, CriticalSection> outSlots;

	ParameterSlot * addParameterSlot(bool isInput, Parameter * p);
	ColorSlot * addColorSlot(bool isInput, const String &name);
	void removeSlot(bool isInput, const String &name);
	void removeSlot(bool isInput, NodeConnectionSlot * s);

	NodeConnectionSlot * getSlotWithName(bool isInput, const String &name);
	Array<NodeConnectionSlot *> getSlotsWithType(bool isInput, ConnectionType  type);
	ParameterSlot * getParameterSlot(Parameter * p, bool isInput);

	var getParameterValue(Parameter * p, var params);

	var getLocalParams(var params);
	
	virtual NodeViewUI * createUI();


	//Listener
	class  NodeListener
	{
	public:
		/** Destructor. */
		virtual ~NodeListener() {}
		virtual void slotAdded(Node *, NodeConnectionSlot *, bool /*isInput*/) {}
		virtual void slotRemoved(Node *, NodeConnectionSlot *, bool /*isInput*/) {}
	};

	ListenerList<NodeListener> nodeListeners;
	void addNodeListener(NodeListener* newListener) { nodeListeners.add(newListener); }
	void removeNodeListener(NodeListener* listener) { nodeListeners.remove(listener); }


	// ASYNC
	class  NodeEvent
	{
	public:
		enum Type { INSLOT_ADDED, OUTSLOT_ADDED, INSLOT_REMOVED, OUTSLOT_REMOVED };

		NodeEvent(Type t, Node * n, NodeConnectionSlot * slot, var params = var()) :
			type(t), node(n), slot(slot), params(params) {}

		Type type;
		Node * node;
		NodeConnectionSlot * slot;
		var params;
	};

	QueuedNotifier<NodeEvent> nodeNotifier;
	typedef QueuedNotifier<NodeEvent>::Listener AsyncListener;

	void addAsynNodeListener(AsyncListener* newListener) { nodeNotifier.addListener(newListener); }
	void addAsyncCoalescedNodeListener(AsyncListener* newListener) { nodeNotifier.addAsyncCoalescedListener(newListener); }
	void removeAsyncNodeListener(AsyncListener* listener) { nodeNotifier.removeListener(listener); }

private:
	WeakReference<Node>::Master masterReference;
};