//#define NUM_SYSTEM_COMPONENTS 6

DeclareComponentSingleton(Root, "root", )

String deviceID;

// system
SettingsComponent * settings;
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

void shutdown();
void restart();

void onChildComponentEvent(const ComponentEvent &e) override;

bool handleCommandInternal(const String & command, var * data, int numData) override;

//const Component *systemComponents[NUM_SYSTEM_COMPONENTS]{comm, wifi, files, server, sequence, battery};

EndDeclareComponent