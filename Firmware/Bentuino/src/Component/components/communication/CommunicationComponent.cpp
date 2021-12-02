ImplementSingleton(CommunicationComponent)

    bool CommunicationComponent::initInternal()
{
    AddComponent(serial, Serial);
    AddComponent(osc, OSC);

    return true;
}

void CommunicationComponent::onChildComponentEvent(const ComponentEvent &e)
{
    if (e.component == serial && e.type == SerialComponent::MessageReceived)
    {
        sendEvent(MessageReceived, e.data, e.numData);
    }
    else if (e.component == osc && e.type == OSCComponent::MessageReceived)
    {
        sendEvent(MessageReceived, e.data, e.numData);
    }
}

void CommunicationComponent::sendParameterFeedback(Component *c, Parameter *param)
{
    var data[1]{param->val};
    serial->sendMessage(c->name, param->name, data, 1);
}

void CommunicationComponent::sendConfigFeedback(Component *c, const String &configName, const String &val)
{
    var data[1]{val};
    serial->sendMessage(c->name, configName, data, 1);
}