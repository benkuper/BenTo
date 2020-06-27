/*
  ==============================================================================

    PropShapeBlock.h
    Created: 26 Jun 2020 6:43:45pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once


#include "../../LightBlockModel.h"
#include "Prop/Shape/PropShape.h"

class ShapeEditorBlock :
	public LightBlockModel
{
public:
	ShapeEditorBlock(var params = var());
	~ShapeEditorBlock();

	TargetParameter* targetShape;

	void getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params);

	String getTypeString() const override { return "Shape Editor"; }

	static ShapeEditorBlock* create(var params) { return new ShapeEditorBlock(params); }
};
