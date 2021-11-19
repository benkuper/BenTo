#ifdef UNITY_COMPILE
ImplementSingleton(CommunicationComponent)

void CommunicationComponent::initInternal()
{
    AddComponent(serial, Serial)
    AddComponent(osc, OSC)
}

#endif