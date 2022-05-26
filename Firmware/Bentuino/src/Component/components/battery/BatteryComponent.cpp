ImplementSingleton(BatteryComponent);

bool BatteryComponent::initInternal(JsonObject o)
{
    sendFeedback = AddParameter("sendFeedback", true);
    return true;
}

void BatteryComponent::updateInternal()
{
}

void BatteryComponent::clearInternal()
{
}