#ifdef UNITY_COMPILE

String ComponentEvent::getName() const
{
    return component->getEventName(type);
}

#endif