bool Component::init()
{
    isInit = initInternal();
    if (isInit)
        NDBG(F("Init OK"));
    else
        NDBG(F("Init Error."));

    return isInit;
}

void Component::update()
{
    for (int i = 0; i < numComponents; i++)
        components[i]->update();

    updateInternal();
}

void Component::clear()
{
    clearInternal();

    for (int i = 0; i < numComponents; i++)
    {
        components[i]->clear();
        delete components[i];
    }

    numComponents = 0;

    for (int i = 0; i < numParameters; i++)
        delete parameters[i];
    numParameters = 0;
}

Parameter *Component::addParameter(const String &name, var val)
{
    Parameter *p = new Parameter(name, val);
    parameters[numParameters] = p;
    numParameters++;
    // AddDefaultParameterListener(p)
    return p;
}

bool Component::handleCommand(const String &command, var *data, int numData)
{
    //NDBG("Handle Command : " + command + ", num data : " + String(numData));

    CommandCheck("setConfig", 2)
        SetConfig(data[0].stringValue(), data[1]);
        return true;
    EndCommandCheck

    if (handleCommandInternal(command, data, numData))
        return true;

    for (int i = 0; i < numParameters; i++)
    {
        if (parameters[i]->name == command)
        {
            if (numData > 0) // query for feedback
            {
                parameters[i]->set(data[0]);
                NDBG("Set Parameter " + parameters[i]->name + " : " + parameters[i]->val.stringValue());
            }
            else
            {
                SendParameterFeedback(parameters[i]);
            }

            return true;
        }
    }

    return false;
}