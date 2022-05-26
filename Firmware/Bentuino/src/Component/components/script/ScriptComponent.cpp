bool ScriptComponent::initInternal(JsonObject o)
{
    return true;
}

void ScriptComponent::updateInternal()
{
    script.update();
}

void ScriptComponent::clearInternal()
{
}

bool ScriptComponent::handleCommandInternal(const String &command, var *data, int numData)
{
    if (CheckCommand("load", 1))
    {
        script.load(data->stringValue());
        return true;
    }
    else if (CheckCommand("stop", 0))
    {
        script.stop();
        return true;
    }

    return false;
}