bool ScriptComponent::initInternal(JsonObject o)
{
    script.init();
    return true;
}

void ScriptComponent::updateInternal()
{
    script.update();
}

void ScriptComponent::clearInternal()
{
    script.shutdown();
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
    }else if(CheckCommand("setScriptParam", 2)){
        script.setScriptParam(data[0].intValue(), (float)data[1]);
        return true;
    }

    return false;
}