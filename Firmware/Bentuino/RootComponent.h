#include "UnityIncludes.h"

//#define NUM_SYSTEM_COMPONENTS 6

DeclareComponent(Root, "root", )

// system
CommunicationComponent * comm;
WifiComponent * wifi;
BatteryComponent * battery;
FilesComponent * files;
WebServerComponent * server;
SequenceComponent * sequence;

//users
IMUComponent * imu;
LedStripComponent *strip;

void initInternal() override
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
}

void onChildComponentEvent(const ComponentEvent &e) override;

//const Component *systemComponents[NUM_SYSTEM_COMPONENTS]{comm, wifi, files, server, sequence, battery};

EndDeclareComponent