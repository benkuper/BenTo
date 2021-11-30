ImplementSingleton(CommunicationComponent)

bool CommunicationComponent::initInternal()
{
    AddComponent(serial, Serial)
    AddComponent(osc, OSC)

    return true;
}