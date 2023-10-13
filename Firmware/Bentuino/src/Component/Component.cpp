#include "Component.h"
bool Component::init(JsonObject o)
{

    AddBoolParam(enabled);

    isInit = initInternal(o);

    if (!isInit)
        NDBG(F("Init Error."));
    // else
    //     NDBG(F("Init OK"));

    return isInit;
}

void Component::update()
{
    if (!enabled)
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
    numParams = 0;
}

// void Component::addParameter(Parameter *p)
// {
//     if (numParameters >= MAX_CHILD_PARAMETERS)
//     {
//         NDBG("Parameter limit reached ! Trying to add " + p->name);
//         return;
//     }

//     parameters[numParameters] = p;
//     numParameters++;
//     AddDefaultParameterListener(Component, p);
// }

// void Component::onParameterEvent(const ParameterEvent &e)
// {
//     // if (e.parameter == &enabled)
//     // onEnabledChanged();

//     onParameterEventInternal(e);

//     SendParameterFeedback(e.parameter);
// }

bool Component::handleCommand(const String &command, var *data, int numData)
{
    if (handleCommandInternal(command, data, numData))
        return true;

    // if (Parameter *p = getParameterWithName(command))
    // {
    //     if (p->readOnly)
    //     {
    //         NDBG("Parameter " + p->name + "is read only !");
    //         return true;
    //     }

    //     if (numData > 0) // query for feedback
    //     {
    //         p->set(data[0]);
    //         // NDBG("Set Parameter " + p->name + " : " + data[0].stringValue() + " >> " + p->stringValue());
    //     }
    //     else
    //     {
    //         SendParameterFeedback(p);
    //     }

    //     return true;
    // }

    return handleSetParam(command, data, numData);
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
    // for (int i = 0; i < numParams; i++)
    // {
    //     // Parameter *p = parameters[i];
    //     // if (!p->isConfig && configOnly)
    //     //     continue;

    //     p->fillSettingsData(configOnly ? o : o.createNestedObject(p->name), configOnly);
    // }

    FillSettingsParam(enabled);
    fillSettingsParamsInternal(o, configOnly);

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

    FillOSCQueryBoolParam(enabled);
    fillOSCQueryParamsInternal(o, fullPath);

    // for (int i = 0; i < numParams; i++)
    // {
    //     fillOSCQueryParam(o, fullPath, paramNames[i], paramTypes[i], params[i]);
    // }

    JsonObject contents = o.createNestedObject("CONTENTS");

    // for (int i = 0; i < numParams; i++)
    // {
    //     Parameter *p = parameters[i];
    //     if (p->isConfig && !includeConfig)
    //         continue;
    //     JsonObject po = contents.createNestedObject(p->name);
    //     p->fillOSCQueryData(po);
    //     po["FULL_PATH"] = fullPath + "/" + p->name;
    // }

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
                      "\"CONTENTS\":";

        if (numParams > 0)
        {

            StaticJsonDocument<6000> doc;
            JsonObject o = doc.to<JsonObject>();

            FillOSCQueryBoolParam(enabled);
            fillOSCQueryParamsInternal(o, fullPath);

            String str;
            serializeJson(o, str);
            str.remove(str.length() - 1);
            chunk->data += str;
        }
        else
        {
            chunk->data += "{";
        }

        // for (int i = 0; i < numParams; i++)
        // {

        //     fillOSCQueryParam(o, pNames[i], paramTypes[i], &params[i]);
        //     // Parameter *p = parameters[i];
        //     // p->fillOSCQueryData(o);

        //     chunk->data += "\"" + pNames[i] + "\":" + str;
        //     if (i < numParams - 1)
        //         chunk->data += ",";
        // }

        if (numComponents > 0)
        {
            chunk->nextType = Start;
            chunk->nextComponent = components[0];
            if (numParams > 0)
                chunk->data += ",";
            chunk->data += "\"" + components[0]->name + "\":";
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

void Component::fillOSCQueryParam(JsonObject o, const String &fullPath, const String &pName, ParamType t, void *param, bool readOnly, const String *options, int numOptions, float vMin, float vMax)
{
    JsonObject po = o.createNestedObject(pName);
    po["DESCRIPTION"] = pName;
    po["ACCESS"] = readOnly ? 1 : 3;
    const String pType = t == Bool ? (*(bool *)param) ? "T" : "F" : typeNames[t];
    po["TYPE"] = pType;
    po["FULL_PATH"] = fullPath + "/" + pName;

    JsonArray vArr = po.createNestedArray("VALUE");

    if (options != nullptr && numOptions > 0)
    {
        JsonArray rArr = po.createNestedArray("RANGE");
        JsonObject vals = rArr.createNestedObject();
        JsonArray opt = vals.createNestedArray("VALS");

        for (int i = 0; i < numOptions; i++)
        {
            opt.add(options[i]);
        }

        po["TYPE"] = "s"; // force string type
        int index = *(int *)param;
        if (index >= 0 && index < numOptions)
            vArr.add(options[index]);
    }
    else
    {
        switch (t)
        {
        case ParamType::Bool:
            vArr.add((*(bool *)param));
            break;

        case ParamType::Int:
            vArr.add((*(int *)param));
            break;

        case ParamType::Float:
            vArr.add((*(float *)param));
            break;

        case ParamType::Str:
            vArr.add((*(String *)param));
            break;

        default:
            break;
        }
    }

    if (vMin != 0 || vMax != 0)
    {
        JsonArray rArr = po.createNestedArray("RANGE");
        JsonObject ro = rArr.createNestedObject();

        ro["MIN"] = vMin;
        ro["MAX"] = vMax;
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
#ifdef USE_SCRIPT
void Component::linkScriptFunctions(IM3Module module, bool isLocal)
{
    const char *tName = isLocal ? "local" : name.c_str(); // getFullPath(false, true).c_str();

    // m3_LinkRawFunctionEx(module, tName, "setEnabled", "v(i)", &Component::m3_setEnabled, this);
    linkScriptFunctionsInternal(module, tName);

    for (int i = 0; i < numComponents; i++)
    {
        if (components[i] == nullptr)
            continue;
        components[i]->linkScriptFunctions(module);
    }
}
#endif

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

// Parameter *Component::getParameterWithName(const String &name)
// {
//     for (int i = 0; i < numParameters; i++)
//         if (parameters[i]->name == name)
//             return parameters[i];

//     return NULL;
// }

void Component::addParam(void *param, ParamType type)
{
    if (numParams >= MAX_CHILD_PARAMS)
    {
        NDBG("Param limit reached !");
        return;
    }

    params[numParams] = param;
    paramTypes[numParams] = type;
    numParams++;
}

void Component::setParam(void *param, var *value, int numData)
{
    bool hasChanged = false;
    ParamType t = getParamType(param);

    // NDBG("Set Param " + String(t));

    if (numData == 0 && t != Trigger)
    {
        NDBG("Expecting at least 1 parameter");
        return;
    }

    if (numData < 2 && t == P2D)
    {
        NDBG("Expecting at least 2 parameters");
        return;
    }

    if (numData < 3 && t == P3D)
    {
        NDBG("Expecting at least 3 parameters");
        return;
    }

    switch (t)
    {
    case ParamType::Trigger:
        break;

    case ParamType::Bool:
        // NDBG("Bool : " + String(*((bool *)param)) + "  <> " + String(value[0].boolValue()));

        hasChanged = *((bool *)param) != value[0].boolValue();
        if (hasChanged)
            *((bool *)param) = value[0].boolValue();
        break;

    case ParamType::Int:
        hasChanged = *((int *)param) != value[0].intValue();
        if (hasChanged)
            *((int *)param) = value[0].intValue();
        break;

    case ParamType::Float:
        hasChanged = *((float *)param) != value[0].floatValue();
        if (hasChanged)
            *((float *)param) = value[0].floatValue();
        break;

    case ParamType::Str:
        hasChanged = *((String *)param) != value[0].stringValue();
        if (hasChanged)
            *((String *)param) = value[0].stringValue();
        break;

    default:
        // not handle
        break;
    }

    if (hasChanged)
    {

        // notify here
        paramValueChanged(param);
    }
}

bool Component::handleSetParam(const String &paramName, var *data, int numData)
{
    // NDBG("Handle Set Param " + paramName + " : ");
    // for (int i = 0; i < numData; i++)
    // {
    //     DBG("> " + data[i].stringValue());
    // }

    CheckAndSetParam(enabled);

    // NDBG("Handle Param Internal");
    return handleSetParamInternal(paramName, data, numData);
}

void Component::paramValueChanged(void *param)
{
    // DBG("Param value changed " + getParamString(param));

    if (param == &enabled)
        onEnabledChanged();

    paramValueChangedInternal(param);
    checkParamsFeedback(param);

    if (parentComponent != nullptr)
        parentComponent->childParamValueChanged(this, this, param);
}

void Component::childParamValueChanged(Component *caller, Component *comp, void *param)
{
    // NDBG("Child param value changed : "+caller->name + " > " + comp->name);
    if (parentComponent != nullptr)
        parentComponent->childParamValueChanged(this, comp, param);
}

bool Component::checkParamsFeedback(void *param)
{
    CheckAndSendParamFeedback(enabled);
    return checkParamsFeedbackInternal(param);
}

Component::ParamType Component::getParamType(void *param) const
{
    for (int i = 0; i < numParams; i++)
        if (params[i] == param)
            return paramTypes[i];

    return ParamType::ParamTypeMax;
}

String Component::getParamString(void *param) const
{
    ParamType t = getParamType(param);
    switch (t)
    {
    case ParamType::Bool:
        return String(*((bool *)param));

    case ParamType::Int:
        return String(*((int *)param));

    case ParamType::Float:
        return String(*((float *)param));

    case ParamType::Str:
        return *((String *)param);

    default:
        break;
    }

    return String("[unknown]");
}
