/*
  ==============================================================================

	PropViz.cpp
	Created: 11 Apr 2018 10:17:51pm
	Author:  Ben

  ==============================================================================
*/

juce_ImplementSingleton(VizTimer);

PropViz::PropViz(Prop* prop) :
	prop(prop),
	propRef(prop),
	shouldRepaint(true)
{
	prop->addAsyncCoalescedPropListener(this);
	VizTimer::getInstance()->registerViz(this);
}

PropViz::~PropViz()
{
	if (!propRef.wasObjectDeleted()) prop->removeAsyncPropListener(this);
	if (VizTimer* vz = VizTimer::getInstanceWithoutCreating()) vz->unregisterViz(this);
}

void PropViz::paint(Graphics& g)
{
	if (propRef.wasObjectDeleted() || prop->currentBlock == nullptr)
	{
		//g.setColour(TEXT_COLOR.darker());
		//g.drawFittedText("No active block", getLocalBounds().reduced(10), Justification::centred, 5);
		return;
	}

	if (PropManager::getInstance()->disablePreview->boolValue()) return;

	int numLeds = prop->resolution->intValue();

	if (numLeds == 0) return;

	Prop::Shape shape = prop->type->getValueDataAsEnum<Prop::Shape>();

	switch (shape)
	{
	case Prop::Shape::CLUB:
	{
		float ratio = getWidth() * 1.0f / getHeight();
		int ledSize = jmax((ratio > (1.0f / numLeds) ? getHeight() : getWidth()) / numLeds, 2);

		Rectangle<int> lr(getLocalBounds().reduced(0, ledSize));

		Rectangle<int> ls = lr.withCentre(lr.getCentre()).withSizeKeepingCentre(ledSize, ledSize);
		//lr = lr.withSizeKeepingCentre(ledSize, ledSize * numLeds);

		for (int i = 0; i < numLeds; i++)
		{
			float p = i * 1.0f / (numLeds - 1);
			Rectangle<float> ledR = ls.withY(lr.getY() + (1 - p) * lr.getHeight() - ledSize / 2.0f).toFloat();
			g.setColour(Colours::white.withAlpha(.2f));
			g.drawEllipse(ledR, .5f);
			g.setColour(prop->colors[numLeds - 1 - i]);
			g.fillEllipse(ledR);
		}
	}
	break;

	case Prop::Shape::HOOP:
	{
		float size = jmin(getWidth(), getHeight()) - 8;
		Rectangle<int> r = getLocalBounds().withSizeKeepingCentre(size, size);

		float radius = r.getWidth() / 2;
		float angle = MathConstants<float>::pi * 2 / numLeds;

		for (int i = 0; i < numLeds; i++)
		{
			Rectangle<float> lr(cosf(angle * i) * radius, sinf(angle * i) * radius, 4, 4);
			lr.translate(r.getCentreX() - 2, r.getCentreY() - 2);
			g.setColour(Colours::white.withAlpha(.2f));
			g.drawEllipse(lr, .5f);
			g.setColour(prop->colors[i]);
			g.fillEllipse(lr);
		}
	}
	break;

	default:
		break;
	}

}

void PropViz::newMessage(const Prop::PropEvent& e)
{
	if (PropManager::getInstance()->disablePreview->boolValue()) return;
	switch (e.type)
	{
	case Prop::PropEvent::BLOCK_CHANGED:
		//repaint();
		shouldRepaint = true;
		break;

	case Prop::PropEvent::COLORS_UPDATED:
		//repaint();
		shouldRepaint = true;
		break;
	}
}

void PropViz::handleRepaint()
{
	if (shouldRepaint)
	{
		//if (prop->colorLock.tryEnter())
	//	{
		repaint();
		shouldRepaint = false;
		//prop->colorLock.exit();
//	}
	}
}

VizTimer::VizTimer()
{
	startTimerHz(20);
}

void VizTimer::registerViz(PropViz* viz)
{
	vizArray.addIfNotAlreadyThere(viz);
}

void VizTimer::unregisterViz(PropViz* viz)
{
	vizArray.removeAllInstancesOf(viz);
}

void VizTimer::timerCallback()
{
	if (PropManager::getInstance()->disablePreview->boolValue()) return;
	for (auto& v : vizArray) v->handleRepaint();
}
