String ComponentEvent::getName() const
{
    return component->getEventName(type);
}