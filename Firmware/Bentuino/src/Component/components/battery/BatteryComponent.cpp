ImplementSingleton(BatteryComponent);

bool BatteryComponent::initInternal(JsonObject o)
{
    AddAndSetParameter(sendFeedback);
    return true;
}

void BatteryComponent::updateInternal()
{
}

void BatteryComponent::clearInternal()
{
}