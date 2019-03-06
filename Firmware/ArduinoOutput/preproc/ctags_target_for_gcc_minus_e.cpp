# 1 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino"
# 1 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino"
# 2 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino" 2

#define SERIAL_DEBUG 1


#define DBG(textToWrite) Serial.println(textToWrite)




# 12 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino" 2

#define USE_BUTTON 1
#define USE_WIFI 1
#define USE_IR 1


#define USE_OSC 1


#define USE_SERVER 1
#define USE_FILES 1

#define USE_TOUCH 0
#define USE_IMU 0
#define USE_LEDSTRIP 1
#define USE_BATTERY 1

#define POWER_ENABLE_PIN 19


using namespace std;

# 35 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino" 2
DeviceSettings settings;

# 38 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino" 2
ButtonManager btManager;


# 42 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino" 2
FileManager fileManager;



# 47 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino" 2
WiFiManager wifiManager;




# 53 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino" 2
BWebServer webServer;




# 59 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino" 2
OSCManager oscManager(wifiManager.oscUDP);
# 68 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino"
# 69 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino" 2
# 70 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino" 2


LedStripManager stripManager(wifiManager.streamingUDP);
# 86 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino"
BatteryManager batteryManager;



# 91 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino" 2
IRManager irManager;


long orientationSendRateMS = 20;
long lastOrientationSendTime = 0;

//Callbacks
void onButtonEvent(int type)
{
  Serial.print("Button Event : ");
  Serial.println(type);

  switch (type)
  {
  case 0:
  {

    if (stripManager.currentMode == LedStripManager::Mode::Pattern)
      stripManager.pm.nextPattern();



    OSCMessage msg("/touch/pressed");
    msg.add(DeviceSettings::deviceID.c_str());
    msg.add(1);
    oscManager.sendMessage(msg);

  }
  break;

  case 1:
  {

    OSCMessage msg("/touch/pressed");
    msg.add(DeviceSettings::deviceID.c_str());
    msg.add(0);
    oscManager.sendMessage(msg);


  }
  break;

  case 2:
  {

    OSCMessage msg("/touch/shortPress");
    msg.add(DeviceSettings::deviceID.c_str());
    oscManager.sendMessage(msg);

  }
  break;

  case 3:
    sleep();
    break;
  }
}

void onMultipress(int count)
{
  Serial.println("Multipress : "+String(count));

  if (count == 2)
  {

    stripManager.nextMode();

  }
  else if (count == 3)
  {

    stripManager.pm.setPattern(stripManager.pm.Fish);

  }
}


void wifiConnectingUpdate(int curTry)
{

  setRange(0, curTry % 32 /*need that global*/, CRGB(0, 20, 20), true);


  if (btManager.buttonIsPressed())
    wifiManager.cancelConnection();
}

void wifiConfigSaved()
{
  resetESP(false);
}
# 222 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino"
void messageReceived(OSCMessage &msg)
{
  char address[256];
  msg.getAddress(address);
  Serial.println("OSC Message received : "+String(address));

  if (msg.match("/reset"))
  {
    resetESP(false);
  }
  else if (msg.match("/bootloader"))
  {
    resetESP(true);
  }
  else if (msg.match("/sleep"))
  {
    sleep();
  }
  else if (msg.match("/battery"))
  {
    batteryLevelUpdate();
  }
  else
  {
    if (settings.handleMessage(msg))
      return;

    if (wifiManager.handleMessage(msg))
      return;






    if (stripManager.handleMessage(msg))
      return;



    if (irManager.handleMessage(msg))
      return;


    Serial.println("...message not handled");
  }
}



void batteryCriticalLevel()
{
  Serial.println("CRITICAL LEVEL, DEEP SLEEP !");


  for (int i = 0; i < 5; i++)
  {
    setFullColor(CRGB::Red);
    delay(200);
    setFullColor(CRGB::Black);
    delay(200);
  }


  delay(100);
  sleep();
}

void batteryChargingStateChanged()
{

/*

  OSCMessage msg("/battery/charging");

  msg.add(DeviceSettings::deviceID.c_str());

  msg.add(batteryManager.isCharging ? 1 : 0);

  oscManager.sendMessage(msg);

  */
# 300 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino"
}

void batteryLevelUpdate()
{

/*

  OSCMessage msg("/battery/level");

  msg.add(DeviceSettings::deviceID.c_str());

  msg.add(batteryManager.normalizedVoltage);

  msg.add(batteryManager.voltage);

  msg.add(batteryManager.rawData);

  oscManager.sendMessage(msg);

  */
# 314 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino"
}


void resetESP(bool toBootloader)
{
  if (toBootloader)
  {
    pinMode(0, 0x02);
    digitalWrite(0, 0x0);
  }

  ESP.restart();
}

void sleep()
{


  int h = batteryManager.normalizedVoltage * 96;

  for (int i = 0; i < 5; i++)
  {
    setFullColor(CHSV(h, 255, 60));
    delay(20);
    setFullColor(CRGB::Black);
    delay(20);
  }

  for (int i = 32 /*need that global*/; i > 0; i--)
  {
    setRange(0, i, CHSV(h, 255, 60), true);
    delay(10);
  }
  FastLED.clear();


  delay(1000);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0x1);
  esp_deep_sleep_start();
}

//Setup & loop
void setup()
{
  Serial.begin(115200);
  Serial.println("");

  btManager.init();
  btManager.addButtonCallback(&onButtonEvent);
  btManager.addMultipressCallback(&onMultipress);

  //Keep the power
  pinMode(19, 0x02);
  digitalWrite(19, 0x1);

  Wire.begin(2, 4);
  settings.init();

  Serial.println("BentoV4 Initialization. (DeviceID : " + String(settings.deviceID) + ")");
  //Serial.println(ESP.getResetReason());
  Serial.println("Will init all modules");


  irManager.init();



  batteryManager.init();
  batteryManager.addBatteryCriticalLevelCallback(&batteryCriticalLevel);
  batteryManager.addChargingStateChangedCallback(&batteryChargingStateChanged);
  batteryManager.addBatteryLevelUpdateCallback(&batteryLevelUpdate);



  stripManager.init();


  batteryManager.checkBattery(true);
  int h = batteryManager.normalizedVoltage * 96;




  for (int i = 0; i < 32 /*need that global*/; i++)
  {
    setRange(0, i, CHSV(h, 255, 60), true);
    delay(10);
  }

  delay(50);
# 416 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino"
  //WiFi & OSC

  wifiManager.addCallbackConnectingUpdate(&wifiConnectingUpdate);
  wifiManager.init();


  if (wifiManager.isConnected)
  {

    oscManager.isReadyToSend = true;

    if (!wifiManager.apMode)
      setFullColor(CRGB(5, 20, 0));
    else
      setFullColor(CRGB(15, 15, 0));

    delay(300);
  }
  else
  {
    if (wifiManager.turnOffWiFi)
    {
      for (int i = 0; i < 3; i++)
      {
        setFullColor(CRGB(20, 0, 20));
        delay(100);
      }
    }
    else
    {
      setFullColor(CRGB(20, 0, 0));
      delay(300);
    }
  }

  setFullColor(CRGB::Black);



  oscManager.init();
  oscManager.addCallbackMessageReceived(&messageReceived);





  setFullColor(CRGB::Black);


  DDBG("Bento is READY :)");
}

void loop()
{

  btManager.update();


  wifiManager.update();


  oscManager.update();
# 491 "d:\\Documents\\Perso\\Projects\\BenTo\\Firmware\\BentoFlowCreator\\BentoFlowCreator.ino"
  stripManager.update();



  batteryManager.update();


  //processSerial();
}

void processSerial()
{
  if (Serial.available() == 0)
    return;

  while (Serial.available() > 0)
  {
    char c = Serial.read();
    switch (c)
    {
    case 'r':
      resetESP(false);
      break;

    case 'b':
      resetESP(true);
      break;
    }
  }
}
