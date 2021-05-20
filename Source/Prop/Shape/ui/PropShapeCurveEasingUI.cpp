/*
  ==============================================================================

    PropShapeCurveEasingUI.cpp
    Created: 26 Jun 2020 2:21:33pm
    Author:  bkupe

  ==============================================================================
*/

PropShapeEasingUIDrawer::PropShapeEasingUIDrawer(Easing2DUI* eui, PropShapeCurveEasing* ce) :
    eui(eui),
    ce(ce)
{
}

void PropShapeEasingUIDrawer::paintPixels(Graphics& g)
{
    int numPixels = ce->numPixels->intValue();
    for (int i = 0; i < numPixels; i++)
    {
        Colour c = Colours::orange;
        g.setColour(c);
        float weight = i * 1.0f / jmax(numPixels - 1, 1);
        Point<float> val = ce->easing2D->getValue(weight);
        Point<int> pos = eui->getUIPosForValuePos(val);

        g.fillEllipse(Rectangle<float>().withSize(4,4).withCentre(pos.toFloat()));
    }
}
