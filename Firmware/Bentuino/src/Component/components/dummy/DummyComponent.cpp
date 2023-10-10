bool DummyComponent::initInternal(JsonObject o)
{
    AddAndSetParameter(dummyParam1);
    AddAndSetParameter(dummyParam2);
    AddAndSetParameter(dummyParam3);
    // AddAndSetParameter(dummyParam4);
    // AddAndSetParameter(dummyParam5);
}


ImplementSingleton(DummyManagerComponent);

bool DummyManagerComponent::initInternal(JsonObject o)
{
    AddAndSetParameter(numDummies);

    for (int i = 0; i < numDummies.intValue(); i++)
    {
        DBG("Add dummy " + String(i + 1) + " of " + String(numDummies.intValue()));
        dummies[i].name = "dummy" + String(i + 1);
        AddOwnedComponent(&dummies[i]);
    }

    return true;
}