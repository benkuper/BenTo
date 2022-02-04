String ComponentEvent::getName() const
{
    return component->getComponentEventName(type);
}