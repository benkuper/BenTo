
#pragma once
DeclareComponentSingleton(BehaviourManager, "behaviours", )

    DeclareRangeConfigParameter(numBehaviours, BEHAVIOUR_MAX_COUNT, 0, BEHAVIOUR_MAX_COUNT);

BehaviourComponent behaviours[BEHAVIOUR_MAX_COUNT];

bool initInternal(JsonObject o) override;

void onChildComponentEvent(const ComponentEvent &e) override;

DeclareComponentEventTypes(CommandLaunched);
DeclareComponentEventNames("CommandLaunched");

EndDeclareComponent