
ImplementSingleton(BehaviourManagerComponent);

bool BehaviourManagerComponent::initInternal(JsonObject o)
{
    AddAndSetParameter(numBehaviours);

    for (int i = 0; i < numBehaviours.intValue(); i++)
    {
        DBG("Add behaviour " + String(i + 1) + " of " + String(numBehaviours.intValue()));
        behaviours[i].name = "behaviour" + String(i + 1);
        AddOwnedComponent(&behaviours[i]);
    }

    return true;
}

void BehaviourManagerComponent::onChildComponentEvent(const ComponentEvent &e)
{
    if (e.type == BehaviourComponent::CommandLaunched)
    {
        sendEvent(CommandLaunched, e.data, e.numData);
    }
}