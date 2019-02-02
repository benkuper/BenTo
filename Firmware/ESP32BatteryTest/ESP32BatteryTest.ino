#define FASTLED_ALLOW_INTERRUPTS 0 //if not working here, modify fastled_config.h to uncomment this same line
#include "FastLED.h"

#define POWER_ENABLE_PIN 19

#define BATTERY_PIN 32
#define BUTTON_PIN 13

#define DATA_PIN 5
#define CLK_PIN 18
#define LED_TYPE    SK9822
#define COLOR_ORDER BGR
#define LED_ENABLE_PIN 26

#define IR_PIN 14
#define IR_RX 34
#define MAX_IR_POWER 22
#define IR_CHANNEL 0
#define IR_PWM_RESOLUTION 8
#define IR_FREQ 5000


#define BT_PRESS_DEBOUNCE 5
int debounceCount = 0;
bool pressed = false;

int rawData;
long lastCheckTime;
#define BATT_CHECK_INTERVAL 100
bool enableCheckBattery = false;

#define NUM_LEDS 32 //need that global
CRGBArray<NUM_LEDS> leds; //need that global

void setup() {
  Serial.begin(115200);
  Serial.println("");

  // put your setup code here, to run once:
  pinMode(BATTERY_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);

  //Keep the power
  pinMode(POWER_ENABLE_PIN, OUTPUT);
  digitalWrite(POWER_ENABLE_PIN, HIGH);

  pinMode(LED_ENABLE_PIN, OUTPUT); //LED's enable
  digitalWrite(LED_ENABLE_PIN, HIGH);

  ledcSetup(IR_CHANNEL, IR_FREQ, IR_PWM_RESOLUTION);
  ledcAttachPin(IR_PIN, IR_CHANNEL);
  pinMode(IR_RX, INPUT);
  ledcWrite(IR_CHANNEL, 0);

  FastLED.addLeds<LED_TYPE, DATA_PIN, CLK_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  setFullColor(CRGB(10, 10, 0));

  delay(1000);
}

void loop() {
  bool btChanged = checkButton();

  if (btChanged && pressed)
  {
    Serial.println("SLEEP !");
    sleep();
  }

  processSerial();

  if (enableCheckBattery) checkBattery();
}

String buffer = "";

void processSerial()
{
  if (Serial.available() > 0)
  {
    char c = Serial.read();
    if (c == '\r') {}
    else if (c == '\n')
    {
      processMessage();
      buffer = "";
    } else buffer += c;
  }
}

void processMessage()
{
  Serial.println("Process : " + buffer);
  String cmd = buffer.substring(0, buffer.indexOf(' '));
  int val = buffer.substring(buffer.indexOf(' ') + 1).toInt();
  val = std::min(std::max(val, 0), 255);

  if (cmd == "led")
  {
    Serial.println("Set RGB Leds to " + String(val));
    setFullColor(CRGB(val, val, val));
  } else if (cmd == "ir")
  {
    Serial.println("Set IR to " + String(val));
    ledcWrite(IR_CHANNEL, val);
  } else if (cmd == "battery")
  {
    enableCheckBattery = val > 0;
  } else if(cmd == "sleep")
  {
   sleep(); 
  }
  {
    Serial.println(cmd + " > " + val);
  }

}

bool checkButton()
{
  bool v = digitalRead(BUTTON_PIN);
  if (v) debounceCount = min(debounceCount + 1, BT_PRESS_DEBOUNCE);
  else debounceCount = max(debounceCount - 1, 0);

  bool newPressed = pressed;
  if (pressed && debounceCount ==  0) newPressed = false;
  if (!pressed && debounceCount == BT_PRESS_DEBOUNCE)  newPressed = true;

  if (newPressed != pressed) //button state changed
  {
    pressed = newPressed;
    return true;
  }

  return false;
}


void checkBattery()
{

  if (millis() - lastCheckTime > BATT_CHECK_INTERVAL)
  {
    lastCheckTime = millis();
    rawData = analogRead(BATTERY_PIN);
    Serial.println("Battery check " + String(rawData));
  }
}



void setFullColor(CRGB color)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = color;
  }

  FastLED.show();
}

void sleep()
{
  setFullColor(CRGB::Black);
    delay(1000);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, HIGH);
    esp_deep_sleep_start();
}
