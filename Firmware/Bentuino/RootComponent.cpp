#ifdef UNITY_COMPILE

void RootComponent::onChildComponentEvent(const ComponentEvent &e)
{
    NDBG("Child Component event : " + e.component->name+" : "+ e.getName());
}

#endif