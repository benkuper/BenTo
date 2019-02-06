#include <Arduino.h>

#define SERIAL_DEBUG 1
#include <Wire.h>

#define USE_BUTTON 1
#define USE_WIFI 1

#define USE_IR 1

#if USE_WIFI
#define USE_OSC 1
#endif

#define USE_TOUCH 0
#define USE_IMU 0
#define USE_LEDSTRIP 1
#define USE_BATTERY 1

#define POWER_ENABLE_PIN 19

using namespace std;

#include "DeviceSettings.h"
DeviceSettings settings;

#include "ButtonManager.h"
ButtonManager btManager;

#if USE_WIFI
#include "WiFiManager.h"
WiFiManager wifiManager;
#endif

#if USE_OSC
#include "OSCManager.h"
OSCManager oscManager(wifiManager.oscUDP);
#endif

#if USE_IMU
#include "IMUManager.h"
IMUManager imuManager;
#endif

#if USE_LEDSTRIP
#include "FastLED.h"

#include "LedStripManager.h"
#if USE_WIFI
LedStripManager stripManager(wifiManager.streamingUDP);
#else
LedStripManager stripManager;
#endif
#endif

#if USE_TOUCH
#include "TouchManager.h"
TouchManager touchManager;
#endif

#if USE_BATTERY
#include "BatteryManager.h"
BatteryManager batteryManager;
#endif

#if USE_IR
#include "IRManager.h"
IRManager irManager;
#endif

long orientationSendRateMS = 20;
long lastOrientationSendTime = 0;

//Callbacks
void onButtonEvent(int type)
{
  Serial.print("Button Event : ");
  Serial.println(type);

  switch (type)
  {
    case BT_PRESSED:
      {
#if USE_LEDSTRIP
        if (stripManager.currentMode == LedStripManager::Mode::Pattern) stripManager.pm.nextPattern();
#endif

#if USE_OSC
        OSCMessage msg("/touch/pressed");
        msg.add(DeviceSettings::deviceID.c_str());
        msg.add(1);
        oscManager.sendMessage(msg);
#endif
      }
      break;

    case BT_RELEASED:
      {
#if USE_OSC
        OSCMessage msg("/touch/pressed");
        msg.add(DeviceSettings::deviceID.c_str());
        msg.add(0);
        oscManager.sendMessage(msg);

#endif
      }
      break;

    case BT_SHORTPRESS:
      {
#if USE_OSC
        OSCMessage msg("/touch/shortPress");
        msg.add(DeviceSettings::deviceID.c_str());
        oscManager.sendMessage(msg);
#endif
      }
      break;

    case BT_LONGPRESS:
      sleep();
      break;
  }
}

void onMultipress(int count)
{
#if SERIAL_DEBUG
  Serial.print("Multipress : ");
  Serial.println(count);
#endif

  if (count == 2)
  {
#if USE_LEDSTRIP
    stripManager.nextMode();
#endif
  } else if (count == 3)
  {
#if USE_LEDSTRIP
    stripManager.pm.setPattern(stripManager.pm.Fish);
#endif
  }
}

#if USE_WIFI
void wifiConnectingUpdate(int curTry)
{
#if USE_LEDSTRIP
  setRange(0, curTry % NUM_LEDS, CRGB(0,20,20), true);
#endif

  if (btManager.buttonIsPressed()) wifiManager.cancelConnection();

}

void wifiConfigSaved()
{
  resetESP(false);
}
#endif

#if USE_TOUCH
void touchUpdate(int touchID, bool touched)
{

#if SERIAL_DEBUG
  Serial.print("t\t");
  Serial.print(touchID);
  Serial.print("\t");
  Serial.println(touched);
#endif

#if USE_OSC
  OSCMessage msg("/touch");
  msg.add(DeviceSettings::deviceID);
  msg.add(touchID);
  msg.add(touched);
  oscManager.sendMessage(msg);
#endif
}
#endif

#if USE_IMU
void orientationUpdate(float yaw, float pitch, float roll)
{
  if (millis() - lastOrientationSendTime > orientationSendRateMS)
  {
#if USE_OSC
/*
    OSCMessage msg("/orientation");
    msg.add(DeviceSettings::deviceID.c_str());
    msg.add(yaw);
    msg.add(pitch);
    msg.add(roll);
    oscManager.sendMessage(msg);
    */
#endif

    lastOrientationSendTime = millis();
  }
}
#endif


#if USE_OSC
void messageReceived(OSCMessage &msg)
{
#if SERIAL_DEBUG
  Serial.print("OSC Message received : ");
  char address[256];
  msg.getAddress(address);
  Serial.println(address);
#endif

  if (msg.match("/reset"))
  {
    resetESP(false);
  } else if (msg.match("/bootloader"))
  {
    resetESP(true);
  } else if (msg.match("/sleep"))
  {
    sleep();
  } else if(msg.match("/battery"))
  {
      batteryLevelUpdate();
  }else
  {
    if(settings.handleMessage(msg)) return;
    
    if (wifiManager.handleMessage(msg)) return;
#if USE_TOUCH
    if (touchManager.handleMessage(msg)) return;
#endif

#if USE_LEDSTRIP
    if (stripManager.handleMessage(msg)) return;
#endif

#if USE_IR
    if (irManager.handleMessage(msg)) return;
#endif

#if SERIAL_DEBUG
    Serial.println("...message not handled");
#endif
  }

}
#endif


#if USE_BATTERY
void batteryCriticalLevel()
{
#if DEBUG_SERIAL
  Serial.println("CRITICAL LEVEL, DEEP SLEEP !");
#endif

#if USE_LEDSTRIP
  for (int i = 0; i < 5; i++)
  {
    setFullColor(CRGB::Red);
    delay(200);
    setFullColor(CRGB::Black);
    delay(200);
  }
#endif

  delay(100);
  sleep();
}

void batteryChargingStateChanged()
{
#if USE_OSC
/*
  OSCMessage msg("/battery/charging");
  msg.add(DeviceSettings::deviceID.c_str());
  msg.add(batteryManager.isCharging ? 1 : 0);
  oscManager.sendMessage(msg);
  */
#endif
}

void batteryLevelUpdate()
{
#if USE_OSC
/*
  OSCMessage msg("/battery/level");
  msg.add(DeviceSettings::deviceID.c_str());
  msg.add(batteryManager.normalizedVoltage);
  msg.add(batteryManager.voltage);
  msg.add(batteryManager.rawData);
  oscManager.sendMessage(msg);
  */
#endif
}
#endif


void resetESP(bool toBootloader)
{
  if (toBootloader)
  {
    pinMode(0, OUTPUT);
    digitalWrite(0, LOW);
  }

  ESP.restart();
}


void sleep()
{
#if USE_LEDSTRIP

  int h = batteryManager.normalizedVoltage * 96;

  for (int i = 0; i < 5; i++)
  {
    setFullColor(CHSV(h, 255, 60));
    delay(20);
    setFullColor(CRGB::Black);
    delay(20);
  }


  for (int i = NUM_LEDS; i > 0; i--)
  {
    setRange(0, i, CHSV(h, 255, 60), true);
    delay(10);
  }
  FastLED.clear();
#endif

  delay(1000);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, HIGH);
  esp_deep_sleep_start();
}

//Setup & loop
void setup() {
  Serial.begin(115200);
  Serial.println("");

  btManager.init();
  btManager.addButtonCallback(&onButtonEvent);
  btManager.addMultipressCallback(&onMultipress);

  //Keep the power
  pinMode(POWER_ENABLE_PIN, OUTPUT);
  digitalWrite(POWER_ENABLE_PIN, HIGH);

  Wire.begin(2, 4);
  settings.init();

#if SERIAL_DEBUG
  Serial.println("BentoV4 Initialization. (DeviceID : "+String(settings.deviceID)+")");
  //Serial.println(ESP.getResetReason());
  Serial.println("Will init all modules");
#endif

#if USE_IR
  irManager.init();
#endif

#if USE_BATTERY
  batteryManager.init();
  batteryManager.addBatteryCriticalLevelCallback(&batteryCriticalLevel);
  batteryManager.addChargingStateChangedCallback(&batteryChargingStateChanged);
  batteryManager.addBatteryLevelUpdateCallback(&batteryLevelUpdate);
#endif

#if USE_LEDSTRIP
  stripManager.init();

#if USE_BATTERY
  batteryManager.checkBattery(true);
  int h = batteryManager.normalizedVoltage * 96;
#else
  int h = 150;
#endif
  
  for (int i = 0; i < NUM_LEDS; i++)
  {
    setRange(0, i, CHSV(h, 255, 60) , true);
    delay(10);
  }

  delay(50);
#endif


#if USE_TOUCH
  touchManager.init();
  touchManager.addCallbackTouchUpdate(&touchUpdate);
#endif

#if USE_IMU
  imuManager.init();
  imuManager.addCallbackOrientationUpdate(&orientationUpdate);
#endif


  //WiFi & OSC
#if USE_WIFI
  wifiManager.addCallbackConnectingUpdate(&wifiConnectingUpdate);
  wifiManager.init();

#if USE_LEDSTRIP
  if (wifiManager.isConnected)
  {
#if USE_OSC
    oscManager.isReadyToSend = true;
#endif
    if(!wifiManager.apMode) setFullColor(CRGB(5,20,0));
    else setFullColor(CRGB(15,15,0));
    
    delay(300);
  } else
  {
    if (wifiManager.turnOffWiFi)
    {
      for (int i = 0; i < 3; i++)
      {
        setFullColor(CRGB(20,0,20));
        delay(100);
      }
    } else
    {
      setFullColor(CRGB(20,0,0));
      delay(300);
    }
  }


  setFullColor(CRGB::Black);
#endif

#if USE_OSC
  oscManager.init();
  oscManager.addCallbackMessageReceived(&messageReceived);
#endif

#endif


#if USE_LEDSTRIP
  setFullColor(CRGB::Black);
#endif

#if SERIAL_DEBUG
  Serial.println("Bento is READY :)");
#endif
}


void loop() {

  btManager.update();

#if USE_WIFI
  wifiManager.update();

#if USE_OSC
  oscManager.update();
#endif

#endif


#if USE_TOUCH
  touchManager.update();
#endif

#if USE_IMU
  imuManager.update();
#endif

#if USE_LEDSTRIP
  stripManager.update();
#endif

#if USE_BATTERY
  batteryManager.update();
#endif

   
  //processSerial();
}

void processSerial()
{
  if (Serial.available()  == 0) return;

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
