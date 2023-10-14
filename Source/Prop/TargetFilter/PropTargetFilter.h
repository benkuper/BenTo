/*
  ==============================================================================

    PropTargetFilter.h
    Created: 5 Feb 2019 11:13:17am
    Author:  jonglissimo

  ==============================================================================
*/

#pragma once

class PropClusterGroupManager;
class Prop;

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
	public PropTargetFilter
{
public:
	PropFilterGlobalID(var params = var());
	~PropFilterGlobalID();

	IntParameter * id;

	int getTargetIDForProp(Prop * p) override;

	DECLARE_TYPE("ID")

};

class PropFilterPropShape :
	public PropTargetFilter
{
public:
	PropFilterPropShape(var params = var());
	~PropFilterPropShape();
	
	EnumParameter * shape;

	int getTargetIDForProp(Prop * p) override;

	DECLARE_TYPE("Shape")
};

class PropFilterCluster :
	public PropTargetFilter
{
public:
	PropFilterCluster(PropClusterGroupManager * manager = nullptr);
	~PropFilterCluster();
	 
	PropClusterGroupManager* manager;

	BoolParameter* useLocalID;

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
	public PropTargetFilter
{
public:
	PropFilterScript(var params = var());
	~PropFilterScript();

	Script script;

	int getTargetIDForProp(Prop * p) override;

	var getJSONData() override;
	void loadJSONDataItemInternal(var data) override;

	DECLARE_TYPE("Script")

};