bool Component::init(JsonObject o)
{
    //NDBG(name+" init, o = "+String(o.size()));
    //for (JsonPair kv : o)  NDBG(String(kv.key().c_str()) +" > "+kv.value().as<String>());

    isInit = initInternal(o);
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

Parameter *Component::addParameter(const String &name, var val, var minVal, var maxVal, bool isConfig)
{
    Parameter *p = new Parameter(name, val, minVal, maxVal, isConfig);
    parameters[numParameters] = p;
    numParameters++;
    AddDefaultParameterListener(Component, p) return p;
}

Parameter *Component::addConfigParameter(const String &name, var val, var minVal, var maxVal)
{
    return addParameter(name, val, minVal, maxVal, true);
}

void Component::onParameterEvent(const ParameterEvent &e)
{
    if (e.parameter == enabled)
        onEnabledChanged();

    onParameterEventInternal(e);

    SendParameterFeedback(e.parameter);
}

bool Component::handleCommand(const String &command, var *data, int numData)
{
    // if (CheckCommand("setConfig", 1))
    // {
    //     if (numData >= 2)
    //     {
    //         SetConfig(data[0].stringValue(), data[1]);
    //         return true;
    //     }
    //     else
    //     {
    //         SendConfigFeedback(data[0].stringValue());
    //         return true;
    //     }
    // }

    if (handleCommandInternal(command, data, numData))
        return true;

    for (int i = 0; i < numParameters; i++)
    {
        if (parameters[i]->name == command)
        {
            if (parameters[i]->readOnly)
            {
                NDBG("Parameter " + parameters[i]->name + "is read only !");
                return true;
            }

            if (numData > 0) // query for feedback
            {
                parameters[i]->set(data[0]);
                NDBG("Set Parameter " + parameters[i]->name + " : " +data[0].stringValue()+" >> "+parameters[i]->stringValue());
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

bool Component::checkCommand(const String &command, const String &ref, int numData, int expectedData)
{
    if (command != ref)
        return false;
    if (numData < expectedData)
    {
        NDBG("Command " + command + " expects at least " + expectedData + " arguments");
        return false;
    }
    return true;
}

void Component::fillSettingsData(JsonObject o)
{
    for (int i = 0; i < numParameters; i++)
    {
        Parameter *p = parameters[i];
        JsonObject po = o.createNestedObject(p->name);
        p->fillSettingsData(po);
    }

    JsonObject comps = o.createNestedObject("components");
    for (int i = 0; i < numComponents; i++)
    {
        Component *c = components[i];
        JsonObject co = comps.createNestedObject(c->name);
        c->fillSettingsData(co);
    }
}

void Component::fillOSCQueryData(JsonObject o, bool includeConfig)
{
    String fullPath = getFullPath();
    o["DESCRIPTION"] = name;
    o["FULL_PATH"] = fullPath;
    o["ACCESS"] = 0;

    JsonObject contents = o.createNestedObject("CONTENTS");

    for (int i = 0; i < numParameters; i++)
    {
        Parameter *p = parameters[i];
        if(p->isConfig && !includeConfig) continue;
        JsonObject po = contents.createNestedObject(p->name);
        p->fillOSCQueryData(po);
        po["FULL_PATH"] = fullPath + "/" + p->name;
    }


    for (int i = 0; i < numComponents; i++)
    {
        Component *c = components[i];
        JsonObject co = contents.createNestedObject(c->name);
        c->fillOSCQueryData(co);
    }
}

String Component::getFullPath()
{
    Component *pc = parentComponent;
    String s = name;
    while (pc != NULL)
    {
        s = pc->name + "/" + s;
        pc = pc->parentComponent;
    }

    return "/" + s;
}