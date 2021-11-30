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
}