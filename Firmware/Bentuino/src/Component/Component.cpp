#include "Component.h"
bool Component::init(JsonObject o)
{
    AddAndSetParameter(enabled);

    isInit = initInternal(o);

    if (!isInit)
        NDBG(F("Init Error."));
    // else
    //     NDBG(F("Init OK"));

    return isInit;
}

void Component::update()
{
    if (!enabled.boolValue())
        return;

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
        // delete components[i];
    }

    numComponents = 0;

    // for (int i = 0; i < numParameters; i++)
    // delete parameters[i];
    numParameters = 0;
}

void Component::addParameter(Parameter *p)
{
    if (numParameters >= MAX_CHILD_PARAMETERS)
    {
        NDBG("Parameter limit reached ! Trying to add " + p->name);
        return;
    }

    parameters[numParameters] = p;
    numParameters++;
    AddDefaultParameterListener(Component, p);
    // DBG("Parameter added, size = " + String(sizeof(*p)) + " (" + String(sizeof(Parameter)) + ")");
}

// Parameter *Component::addParameter(const String &name, var val, var minVal, var maxVal, bool isConfig)
// {
//     Parameter *p = new Parameter(name, val, minVal, maxVal, isConfig);
//     addParameter(p);

//     return p;
// }

// Parameter *Component::addConfigParameter(const String &name, var val, var minVal, var maxVal)
// {
//     return addParameter(name, val, minVal, maxVal, true);
// }

void Component::onParameterEvent(const ParameterEvent &e)
{
    if (e.parameter == &enabled)
        onEnabledChanged();

    onParameterEventInternal(e);

    SendParameterFeedback(e.parameter);
}

bool Component::handleCommand(const String &command, var *data, int numData)
{
    if (handleCommandInternal(command, data, numData))
        return true;

    if (Parameter *p = getParameterWithName(command))
    {
        if (p->readOnly)
        {
            NDBG("Parameter " + p->name + "is read only !");
            return true;
        }

        if (numData > 0) // query for feedback
        {
            p->set(data[0]);
            // NDBG("Set Parameter " + p->name + " : " + data[0].stringValue() + " >> " + p->stringValue());
        }
        else
        {
            SendParameterFeedback(p);
        }

        return true;
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

void Component::fillOSCQueryData(JsonObject o, bool includeConfig, bool recursive)
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

    if (recursive)
    {
        for (int i = 0; i < numComponents; i++)
        {
            if (components[i] == nullptr)
                continue;

            Component *c = components[i];
            JsonObject co = contents.createNestedObject(c->name);
            c->fillOSCQueryData(co);
        }
    }
}

void Component::fillChunkedOSCQueryData(OSCQueryChunk *chunk)
{

    const String fullPath = getFullPath();

    switch (chunk->nextType)
    {
    case Start:
    {
        chunk->data = "{\"DESCRIPTION\":\"" + name + "\"," +
                      "\"FULL_PATH\":\"" + fullPath + "\"," +
                      "\"ACCESS\":0," +
                      "\"CONTENTS\":{";

        for (int i = 0; i < numParameters; i++)
        {
            DynamicJsonDocument doc(500);
            JsonObject o = doc.to<JsonObject>();
            Parameter *p = parameters[i];
            p->fillOSCQueryData(o);
            o["FULL_PATH"] = fullPath + "/" + p->name;
            String str;
            serializeJson(o, str);
            chunk->data += "\"" + p->name + "\":" + str;
            if (i < numParameters - 1)
                chunk->data += ",";
        }

        if (numComponents > 0)
        {
            chunk->nextType = Start;
            chunk->nextComponent = components[0];
            if (numParameters > 0)
                chunk->data += ",";
            chunk->data += "\""+components[0]->name + "\":";
        }
        else
        {
            chunk->data += "}";
            chunk->nextComponent = (Component *)this;
            chunk->nextType = End;
        }
    }
    break;

    case End:
    {
        chunk->data = "}";
        if (parentComponent != nullptr)
        {
            parentComponent->setupChunkAfterComponent(chunk, this);
        }
        else
            chunk->nextComponent = nullptr;
    }

    default:
        break;
    }
}

void Component::setupChunkAfterComponent(OSCQueryChunk *chunk, const Component *c)
{
    int index = 0;
    for (int i = 0; i < numComponents; i++)
    {
        if (components[i] == c)
        {
            index = i;
            break;
        }
    }

    if (index < numComponents - 1) // last one
    {
        chunk->data += ",\"" + components[index + 1]->name + "\":";
        chunk->nextComponent = components[index + 1];
        chunk->nextType = Start;
    }
    else
    {
        chunk->data += "}";
        chunk->nextComponent = this;
        chunk->nextType = End;
    }
}

String Component::getFullPath(bool includeRoot, bool scriptMode) const
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
void Component::linkScriptFunctions(IM3Module module, bool isLocal)
{
    const char *tName = isLocal ? "local" : getFullPath(false, true).c_str();

    // m3_LinkRawFunctionEx(module, tName, "setEnabled", "v(i)", &Component::m3_setEnabled, this);

    linkScriptFunctionsInternal(module, tName);

    for (int i = 0; i < numComponents; i++)
    {
        if (components[i] == nullptr)
            continue;
        components[i]->linkScriptFunctions(module);
    }
}

Component *Component::addComponent(Component *c, JsonObject o)
{
    if (numComponents >= MAX_CHILD_COMPONENTS)
    {
        NDBG("Component limit reached ! Trying to add " + c->name);
        return nullptr;
    }

    components[numComponents] = (Component *)c;
    c->parentComponent = this;
    AddDefaultComponentListener(c);
    numComponents++;
    c->init(o);
    // DBG("Component added, size = " + String(sizeof(*c)) + " (" + String(sizeof(Component)) + ")");
    return c;
}

Component *Component::getComponentWithName(const String &name)
{
    if (name == this->name)
        return this;

    int subCompIndex = name.indexOf('.');

    if (subCompIndex > 0)
    {
        String n = name.substring(0, subCompIndex);
        for (int i = 0; i < numComponents; i++)
        {
            if (components[i]->name == n)
                return components[i]->getComponentWithName(name.substring(subCompIndex + 1));
        }
    }
    else
    {
        for (int i = 0; i < numComponents; i++)
        {
            if (components[i]->name == name)
                return components[i];
        }
    }

    return NULL;
}

Parameter *Component::getParameterWithName(const String &name)
{
    for (int i = 0; i < numParameters; i++)
        if (parameters[i]->name == name)
            return parameters[i];

    return NULL;
}
