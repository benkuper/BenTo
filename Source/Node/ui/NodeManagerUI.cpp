/*
  ==============================================================================

	NodeManagerUI.cpp
	Created: 14 Apr 2018 2:10:12am
	Author:  Ben

  ==============================================================================
*/

NodeManagerUI::NodeManagerUI(NodeManager * manager) :
	BaseManagerViewUI("Nodes", manager),
	startConnector(nullptr),
	dropConnector(nullptr)
{
	animateItemOnAdd = false;
	canZoom = true;

	removeMouseListener(this);
	addMouseListener(this, true);

	acceptedDropTypes.add("NodeTool");
	acceptedDropTypes.add("LightBlockModel");
	acceptedDropTypes.add("Script");
	acceptedDropTypes.add("Node");
	acceptedDropTypes.add("Timeline");
	acceptedDropTypes.add("Picture");
	
	addExistingItems(false);
	
	connectionsUI.reset(new NodeConnectionManagerUI(&manager->connectionManager, this));
	addAndMakeVisible(connectionsUI.get());
	connectionsUI->toBack();
	connectionsUI->autoFilterHitTestOnItems = true;
	connectionsUI->validateHitTestOnNoItem = false;

	startTimerHz(30);

	resized();
}

NodeManagerUI::~NodeManagerUI()
{
}

void NodeManagerUI::paintOverChildren(Graphics & g)
{
	BaseManagerUI::paintOverChildren(g);
	if (startConnector != nullptr) drawConnectionCreation(g);

	/*if (!dragPosition.isOrigin())
	{
		Colour c = BLUE_COLOR;
		if (dragType == "NodeTool") c = Colours::pink;
		g.setColour(c.withAlpha(.3f));
		g.drawRoundedRectangle(Rectangle<float>(0,0, 200, 200).withCentre(dragPosition+getViewCenter().toFloat()), 2, 4);
	}*/
}

void NodeManagerUI::drawConnectionCreation(Graphics &g)
{
	Point<float> sp = getLocalPoint(startConnector, startConnector->getLocalBounds().getCentre().toFloat());
	Point<float> dp = dropConnector == nullptr ? getMouseXYRelative().toFloat() : getLocalPoint(dropConnector, dropConnector->getLocalBounds().getCentre().toFloat());
	g.setColour(dropConnector == nullptr ? NodeConnectionUI::getColorForType(startConnector->slot->type) : Colours::yellow);

	Point<float> p1 = startConnector->slot->isInput ? dp : sp;
	Point<float> p2 = startConnector->slot->isInput ? sp : dp;
	Path p;
	p.startNewSubPath(p1);
	p.cubicTo(p1.translated(60, 0), p2.translated(-60, 0), p2);
	g.strokePath(p, PathStrokeType(3));
}

void NodeManagerUI::resized()
{
	if (connectionsUI == nullptr) return;
	connectionsUI->setBounds(getLocalBounds());
	BaseManagerViewUI::resized();
}


NodeViewUI::Connector * NodeManagerUI::getDropCandidate()
{
	for (auto & nui : itemsUI)
	{
		if (nui->item == startConnector->slot->node) continue;
		
		OwnedArray<NodeViewUI::Connector> * arr = startConnector->slot->isInput ? &nui->outConnectors : &nui->inConnectors;
		for (auto &con : *arr)
		{
			if (con->slot->type != startConnector->slot->type) continue;
			if (con->getMouseXYRelative().getDistanceFromOrigin() < dropDistance) return con;
		}
	}

	return nullptr;

}

NodeViewUI::Connector * NodeManagerUI::getConnectorForSlot(NodeConnectionSlot * s)
{
	if (s == nullptr) return nullptr;

	NodeViewUI * nui = getUIForItem(s->node);
	
	if (nui == nullptr) return nullptr;

	OwnedArray<NodeViewUI::Connector> * arr = s->isInput ? &nui->inConnectors : &nui->outConnectors;
	for (auto &con : *arr)
	{
		if (con->slot == s) return con;
	}

	return nullptr;
}

NodeViewUI * NodeManagerUI::createUIForItem(Node * item)
{
	NodeViewUI * nui = item->createUI();
	ColorNodeViewUI * cnui = dynamic_cast<ColorNodeViewUI *>(nui);
	if (cnui != nullptr) cnui->propToPreview = manager->prop.get();
	return nui;
}

void NodeManagerUI::mouseDown(const MouseEvent & e)
{
	BaseManagerViewUI::mouseDown(e);

	NodeViewUI::Connector * c = dynamic_cast<NodeViewUI::Connector *>(e.originalComponent);
	if (c != nullptr)
	{
		startConnector = c;
		setRepaintsOnMouseActivity(true);
	}
}

void NodeManagerUI::mouseDrag(const MouseEvent & e)
{
	BaseManagerViewUI::mouseDrag(e);
	
	//Check for drop candidates
	if (startConnector != nullptr)
	{
		dropConnector = getDropCandidate();
		repaint();
	}
}

void NodeManagerUI::mouseUp(const MouseEvent & e)
{
	BaseManagerViewUI::mouseUp(e);

	if (startConnector != nullptr && dropConnector != nullptr)
	{
		NodeConnectionSlot * source = startConnector->slot->isInput ? dropConnector->slot : startConnector->slot;
		NodeConnectionSlot * dest = startConnector->slot->isInput ? startConnector->slot : dropConnector->slot;
		manager->connectionManager.addConnection(source, dest);
	}

	startConnector = nullptr;
	dropConnector = nullptr;
	repaint();

	setRepaintsOnMouseActivity(false);
}

void NodeManagerUI::mouseEnter(const MouseEvent & e)
{
	BaseManagerViewUI::mouseEnter(e);
	/*NodeViewUI::Connector * c = dynamic_cast<NodeViewUI::Connector *>(e.originalComponent);
	if (c != nullptr)
	{
		DBG("Mouse Enter on connector : " << c->slot->node->niceName << ":" << c->slot->name);
	}*/
}


void NodeManagerUI::itemDragEnter(const SourceDetails & source)
{
	dragPosition = getViewMousePosition().toFloat();
	dragType = source.description.getProperty("dataType", "");
	repaint();
}

void NodeManagerUI::itemDragExit(const SourceDetails & source)
{
	dragPosition = Point<float>();
	repaint();
}

void NodeManagerUI::itemDragMove(const SourceDetails & source)
{
	dragPosition = getViewMousePosition().toFloat();
	repaint();
}

void NodeManagerUI::itemDropped(const SourceDetails & source)
{
	BaseManagerViewUI::itemDropped(source); 
	
	dragType = source.description.getProperty("dataType", "");
	
	if (LightBlockModelUI* modelUI = dynamic_cast<LightBlockModelUI*>(source.sourceComponent.get()))
	{
		if (modelUI == nullptr ) return;

		NodeBlock * nb = dynamic_cast<NodeBlock *>(modelUI->item);
		if (nb != nullptr && manager == &nb->manager) return;

		ModelNode * node = (ModelNode *)manager->addItem(NodeFactory::getInstance()->create("Model"));
		if (node == nullptr) return;

		node->viewUIPosition->setPoint(getViewMousePosition().toFloat() - node->viewUISize->getPoint() / 2);


		bool shift = KeyPress::isKeyCurrentlyDown(16);
		if (shift)
		{
			PopupMenu m;
			m.addItem(-1, "Default");
			m.addSeparator();
			int index = 1;
			for (auto &p : modelUI->item->presetManager.items) m.addItem(index++, p->niceName);
			m.showMenuAsync(PopupMenu::Options(), [this, modelUI](int result)
				{
					if (result >= 1)
					{
						LightBlockColorProvider* provider = modelUI->item;
						provider = modelUI->item->presetManager.items[result - 1];
					}
				}
			);
		}
		else
		{
			node->activeProvider->setValueFromTarget(modelUI->item);

		}

	}else if (dragType == "NodeTool")
	{
		Node * node = manager->addItem(NodeFactory::getInstance()->create(source.description.getProperty("nodeType", "")));
		if (node == nullptr)
		{
			dragPosition = Point<float>();
			return;
		}

		node->viewUIPosition->setPoint(dragPosition);
	}
	
	dragPosition = Point<float>();
}


void NodeManagerUI::timerCallback()
{
	for (auto &nui : itemsUI) nui->repaint();
}