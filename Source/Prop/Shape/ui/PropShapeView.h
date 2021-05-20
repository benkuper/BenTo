/*
  ==============================================================================

    PropShapeView.h
    Created: 26 Jun 2020 12:04:08pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class PropShapeView :
    public Curve2DUI
{
public:
    PropShapeView(PropShape* shape);
    ~PropShapeView();

    PropShape* shape;
};