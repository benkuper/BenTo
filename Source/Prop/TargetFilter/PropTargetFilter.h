/*
  ==============================================================================

    PropTargetFilter.h
    Created: 5 Feb 2019 11:13:17am
    Author:  jonglissimo

  ==============================================================================
*/

#pragma once
#include "Prop/Prop.h"

class PropTargetFilter :
	public BaseItem
{
public:
	PropTargetFilter(StringRef name = "Prop Target");
	virtual ~PropTargetFilter();

	bool isPropValid(Prop * p);
	virtual int getTargetIDForProp(Prop * p);
};


class PropFilterGlobalID :
	PropTargetFilter
{
public:
	PropFilterGlobalID();
	~PropFilterGlobalID();

	IntParameter * id;

	int getTargetIDForProp(Prop * p) override;
};

class PropFilterPropFamily :
	PropTargetFilter
{
public:
	PropFilterPropFamily();
	~PropFilterPropFamily();

	TargetParameter * family;

	int getTargetIDForProp(Prop * p) override;
};

class PropFilterPropType :
	PropTargetFilter
{
public:
	PropFilterPropType();
	~PropFilterPropType();
	
	EnumParameter * type;

	int getTargetIDForProp(Prop * p) override;
};

class PropFilterCluster :
	PropTargetFilter
{
public:
	PropFilterCluster();
	~PropFilterCluster();
	 
	BoolParameter * specificClusterGroup;
	TargetParameter * clusterGroup;
	BoolParameter * specificCluster;
	TargetParameter * cluster;
	BoolParameter * specificID;
	IntParameter * id;

	int getTargetIDForProp(Prop * p) override;
};


class PropFilterScript :
	PropTargetFilter
{
public:
	PropFilterScript();
	~PropFilterScript();

	Script script;

	int getTargetIDForProp(Prop * p) override;
};