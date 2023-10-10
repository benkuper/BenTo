bool DummyComponent::initInternal(JsonObject o)
{
    AddBoolParam(param1);
    AddBoolParam(param2);
    AddIntParam(param3);
    AddFloatParam(param4);
    AddStringParam(param5);

    return true;
}

ImplementSingleton(DummyManagerComponent);

bool DummyManagerComponent::initInternal(JsonObject o)
{
    AddIntParam(numItems);

    for (int i = 0; i < numItems; i++)
    {
        NDBG("Add item " + String(i + 1));
        items[i].name = "item" + String(i + 1);
        AddOwnedComponent(&items[i]);
    }

    return true;
}