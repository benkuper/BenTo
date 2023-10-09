ImplementSingleton(CommunicationComponent);

bool CommunicationComponent::initInternal(JsonObject o)
{
    AddOwnedComponent(&serial);
    AddOwnedComponent(&osc);

    return true;
}

void CommunicationComponent::onChildComponentEvent(const ComponentEvent &e)
{
    if (e.component == &serial && e.type == SerialComponent::MessageReceived)
    {
        sendEvent(MessageReceived, e.data, e.numData);
    }
    else if (e.component == &osc && e.type == OSCComponent::MessageReceived)
    {
        sendEvent(MessageReceived, e.data, e.numData);
    }
}

void CommunicationComponent::sendParameterFeedback(Component *c, Parameter *param)
{
    var data[1]{param->val};
    serial.sendMessage(c->name, param->name, data, 1);
    osc.sendMessage("/" + c->name, param->name, data, 1);

    // maybe should find away so calls are not crossed with websocket ?
    WebServerComponent::instance->sendParameterFeedback(c, param);
}

void CommunicationComponent::sendEventFeedback(const ComponentEvent &e)
{
    serial.sendMessage(e.component->name, e.getName(), e.data, e.numData);
    osc.sendMessage(e.component->name, e.getName(), e.data, e.numData);
}

void CommunicationComponent::sendMessage(Component *c, const String &mName, const String &val)
{
    var data[1]{val};
    serial.sendMessage(c->name, mName, data, 1);
    osc.sendMessage(c->name, mName, data, 1);
}
