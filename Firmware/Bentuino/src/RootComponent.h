//#define NUM_SYSTEM_COMPONENTS 6

DeclareComponentSingleton(Root, "root", )

String deviceID;

// system
CommunicationComponent * comm;
WifiComponent * wifi;
BatteryComponent * battery;
FilesComponent * files;
WebServerComponent * server;
SequenceComponent * sequence;

//users
ButtonComponent * button;
IMUComponent * imu;
LedStripComponent *strip;

//Needs a single structure
LedStreamReceiverComponent * streamReceiver;

//Behaviour
Timer<5> timer;
long timeAtShutdown;

bool initInternal(JsonObject o) override;
void updateInternal() override;

void shutdown();
void restart();

void powerdown();

void saveSettings();

void onChildComponentEvent(const ComponentEvent &e) override;

bool handleCommandInternal(const String & command, var * data, int numData) override;

bool isShuttingDown() const { return timeAtShutdown > 0; }
//const Component *systemComponents[NUM_SYSTEM_COMPONENTS]{comm, wifi, files, server, sequence, battery};

EndDeclareComponent