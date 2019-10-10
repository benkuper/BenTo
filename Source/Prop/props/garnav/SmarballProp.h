/*
  ==============================================================================

    SmarballProp.h
    Created: 21 Sep 2018 8:31:28pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../bento/BentoProp.h"

class SmartballProp :
	public BentoProp
{
public:
	SmartballProp(var params = var());
	~SmartballProp();

	Point3DParameter * accelerometer;

	String getTypeString() const override { return "SmartBall"; }
	static SmartballProp * create(var params) { return new SmartballProp(params); }

};