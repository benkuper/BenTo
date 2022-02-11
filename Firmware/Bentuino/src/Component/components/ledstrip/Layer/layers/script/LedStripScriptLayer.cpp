bool LedStripScriptLayer::initInternal(JsonObject o)
{
    return true;
}

void LedStripScriptLayer::updateInternal()
{
}

void LedStripScriptLayer::clearInternal()
{
}

bool LedStripScriptLayer::handleCommandInternal(const String &command, var *data, int numData)
{
    if (CheckCommand("load", 1))
    {
        script.load(data->stringValue());
        return true;
    }

    return false;
}