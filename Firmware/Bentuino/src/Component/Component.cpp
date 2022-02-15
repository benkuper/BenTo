bool Component::init(JsonObject o)
{
    enabled->set(Settings::getVal(o, "enabled", enabled->boolValue()));

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
                NDBG("Set Parameter " + parameters[i]->name + " : " + data[0].stringValue() + " >> " + parameters[i]->stringValue());
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

// Save / Load

void Component::fillSettingsData(JsonObject o, bool configOnly)
{
    for (int i = 0; i < numParameters; i++)
    {
        Parameter *p = parameters[i];
        if (!p->isConfig && configOnly)
            continue;

        p->fillSettingsData(configOnly ? o : o.createNestedObject(p->name), configOnly);
    }

    if (numComponents > 0)
    {
        JsonObject comps = o.createNestedObject("components");
        for (int i = 0; i < numComponents; i++)
        {
            Component *c = components[i];
            JsonObject co = comps.createNestedObject(c->name);
            c->fillSettingsData(co, configOnly);
        }
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
        if (p->isConfig && !includeConfig)
            continue;
        JsonObject po = contents.createNestedObject(p->name);
        p->fillOSCQueryData(po);
        po["FULL_PATH"] = fullPath + "/" + p->name;
    }

    for (int i = 0; i < numComponents; i++)
    {
        if (components[i] == nullptr)
            continue;

        Component *c = components[i];
        JsonObject co = contents.createNestedObject(c->name);
        c->fillOSCQueryData(co);
    }
}

String Component::getFullPath(bool includeRoot, bool scriptMode)
{
    Component *pc = parentComponent;
    String s = name;

    char separator = scriptMode ? '_' : '/';

    while (pc != NULL)
    {
        if (pc == RootComponent::instance && !includeRoot)
            break;
        s = pc->name + separator + s;
        pc = pc->parentComponent;
    }

    if (!scriptMode)
        s = "/" + s;

    return s;
}

// Scripts
void Component::linkScriptFunctions(Script *script, bool isLocal)
{
    IM3Module module = script->runtime->modules;
    const char *tName = isLocal ? "local" : getFullPath(false, true).c_str();

    //m3_LinkRawFunctionEx(module, tName, "setEnabled", "v(i)", &Component::m3_setEnabled, this);

    linkScriptFunctionsInternal(script, module, tName);

    for (int i = 0; i < numComponents; i++)
    {
        if (components[i] == nullptr)
            continue;
        components[i]->linkScriptFunctions(script);
    }
}

// Script functions

// void Component::setEnabledFromScript(uint32_t val)
// {
//     enabled->set((bool)val);
// }