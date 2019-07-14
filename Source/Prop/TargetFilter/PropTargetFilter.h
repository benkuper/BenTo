/*
  ==============================================================================

    PropTargetFilter.h
    Created: 5 Feb 2019 11:13:17am
    Author:  jonglissimo

  ==============================================================================
*/

#pragma once
#include "Prop/Prop.h"
class PropClusterGroupManager;


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

	String getTypeString() const override { return "Global ID"; }
	static PropTargetFilter * create(var) { return new PropFilterGlobalID(); }
};

class PropFilterPropFamily :
	PropTargetFilter
{
public:
	PropFilterPropFamily();
	~PropFilterPropFamily();

	TargetParameter * family;

	int getTargetIDForProp(Prop * p) override;

	String getTypeString() const override { return "Family"; }
	static PropTargetFilter * create(var) { return new PropFilterPropFamily(); }
};

class PropFilterPropType :
	PropTargetFilter
{
public:
	PropFilterPropType();
	~PropFilterPropType();
	
	EnumParameter * type;

	int getTargetIDForProp(Prop * p) override;

	String getTypeString() const override { return "Type"; }
	static PropTargetFilter * create(var) { return new PropFilterPropType(); }
};

class PropFilterCluster :
	PropTargetFilter
{
public:
	PropFilterCluster(PropClusterGroupManager * manager = nullptr);
	~PropFilterCluster();
	 
	PropClusterGroupManager* manager;

	BoolParameter * specificClusterGroup;
	TargetParameter * clusterGroup;
	BoolParameter * specificCluster;
	TargetParameter * cluster;
	BoolParameter * specificID;
	IntParameter * id;

	int getTargetIDForProp(Prop * p) override;

	void onContainerParameterChangedInternal(Parameter * p) override;

	static bool targetIsCluster(ControllableContainer * cc);

	String getTypeString() const override { return "Cluster"; }
	static PropTargetFilter * create(PropClusterGroupManager * m) { return new PropFilterCluster(m); }

	class PropFilterClusterDefinition :
		public FactoryDefinition<PropTargetFilter, std::function<PropTargetFilter* (PropClusterGroupManager*)>>
	{
	public:
		PropFilterClusterDefinition(StringRef menu, StringRef type, std::function<PropTargetFilter* (PropClusterGroupManager*)> createFunc, PropClusterGroupManager * pcgm) :
			FactoryDefinition(menu, type, createFunc),
			pcgm(pcgm)
		{}
		~PropFilterClusterDefinition() {}

		PropClusterGroupManager* pcgm;
		virtual PropTargetFilter * create() { return createFunc(pcgm); }
	};
};


class PropFilterScript :
	PropTargetFilter
{
public:
	PropFilterScript();
	~PropFilterScript();

	Script script;

	int getTargetIDForProp(Prop * p) override;

	String getTypeString() const override { return "Script"; }
	static PropTargetFilter * create(var) { return new PropFilterScript(); }
};