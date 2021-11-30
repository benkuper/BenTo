ImplementSingleton(CommunicationComponent)

void CommunicationComponent::initInternal()
{
    AddComponent(serial, Serial)
    AddComponent(osc, OSC)
}