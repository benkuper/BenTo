ImplementSingleton(CommunicationComponent);

bool CommunicationComponent::initInternal(JsonObject o)
{
#ifdef USE_SERIAL
    AddOwnedComponent(&serial);
#endif

#ifdef USE_OSC
    AddOwnedComponent(&osc);
#endif

    return true;
}

void CommunicationComponent::onChildComponentEvent(const ComponentEvent &e)
{
#ifdef USE_SERIAL
    if (e.component == &serial && e.type == SerialComponent::MessageReceived)
    {
        sendEvent(MessageReceived, e.data, e.numData);
    }
#endif

#if defined USER_SERIAL && defined USE_OSC
    else
#endif

#ifdef USE_OSC
        if (e.component == &osc && e.type == OSCComponent::MessageReceived)
    {
        sendEvent(MessageReceived, e.data, e.numData);
    }
#endif
}

void CommunicationComponent::sendParameterFeedback(Component *c, Parameter *param)
{
    var data[1]{param->val};

#ifdef USE_SERIAL
    serial.sendMessage(c->name, param->name, data, 1);
#endif

#ifdef USE_OSC
    osc.sendMessage("/" + c->name, param->name, data, 1);
#endif

#ifdef USE_SERVER
    // maybe should find away so calls are not crossed with websocket ?
    WebServerComponent::instance->sendParameterFeedback(c, param);
#endif
}

void CommunicationComponent::sendEventFeedback(const ComponentEvent &e)
{
#ifdef USE_SERIAL
    serial.sendMessage(e.component->name, e.getName(), e.data, e.numData);
#endif

#ifdef USE_OSC
    osc.sendMessage(e.component->name, e.getName(), e.data, e.numData);
#endif
}

void CommunicationComponent::sendMessage(Component *c, const String &mName, const String &val)
{
    var data[1]{val};
#ifdef USE_SERIAL
    serial.sendMessage(c->name, mName, data, 1);
#endif

#ifdef USE_OSC
    osc.sendMessage(c->name, mName, data, 1);
#endif
}
