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

bool initInternal() override;
void onChildComponentEvent(const ComponentEvent &e) override;

//const Component *systemComponents[NUM_SYSTEM_COMPONENTS]{comm, wifi, files, server, sequence, battery};

EndDeclareComponent