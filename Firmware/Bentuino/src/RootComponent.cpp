bool RootComponent::initInternal()
{
    BoardInit;

    AddComponent(settings, Settings);
    AddComponent(comm, Communication);
    AddComponent(wifi, Wifi);
    AddComponent(battery, Battery);
    AddComponent(files, Files);
    AddComponent(server, WebServer);
    AddComponent(sequence, Sequence);

    
    AddComponent(strip, LedStrip);
    AddComponent(imu, IMU);

    return true;
}

void RootComponent::onChildComponentEvent(const ComponentEvent &e)
{
    NDBG("Child Component event : " + e.component->name+" : "+ e.getName());
}