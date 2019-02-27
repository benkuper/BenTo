// Test Firmware for Creators Club REV B3
//#define MOCKUP // comment out this line for production build

#include <Bounce2.h>
#include "FastLED.h"
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "crc16.h"

// preferences
const char *ssid = "creators-club_test";
const char *password = "wifipassword";
const char *host = "192.168.4.1";
const int port = 80;
#define BAUD_RATE 115200
#define IR_BAUD_RATE 57600
#define LED_BRIGHTNESS 25

const char *test_phrase = "feel the flow - ";
char phrase[20];

// main leds config
#ifdef MOCKUP
#define LED_EN_PIN 0
#define DATA_PIN 18
#define CLK_PIN 5
#define NUM_LEDS    72
#define LED_TYPE    APA102
#define COLOR_ORDER BGR
#else
#define LED_EN_PIN 26
#define DATA_PIN 5
#define CLK_PIN 18
#define NUM_LEDS    32
#define LED_TYPE    SK9822
#define COLOR_ORDER BGR
#endif
#define FRAMES_PER_SECOND 25
CRGB leds[NUM_LEDS];
#define FRAMES_PER_SECOND  120

#define BUTTON_PIN 23
Bounce debouncer = Bounce();

#define BATTERY_PIN 32

// imu config
#define I2C_CLOCK 1000
#ifdef MOCKUP
#define SDA_PIN 25
#define SCL_PIN 26
#else
#define SDA_PIN 2
#define SCL_PIN 4
#endif
#define INT_PIN 15
#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 bno = Adafruit_BNO055();

// ir config
#define IR_TX_PIN 14
#define IR_RX_PIN 34

WiFiClient *client;

boolean imu_start;
void setup() {
  Serial.begin(BAUD_RATE);
  
#ifdef MOCKUP
  pinMode(BUTTON_PIN, INPUT_PULLUP);
#else
  pinMode(BUTTON_PIN, INPUT);
#endif
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(80);

  pinMode(LED_EN_PIN, OUTPUT);
  digitalWrite(LED_EN_PIN, HIGH);
  FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(LED_BRIGHTNESS);
  for (int i; i < NUM_LEDS; i++) {
    leds[i] = CRGB::White;
    LEDS.setBrightness(LED_BRIGHTNESS);
  }
  FastLED.show();

  pinMode(IR_TX_PIN, OUTPUT);
  digitalWrite(IR_TX_PIN, LOW);
  pinMode(IR_RX_PIN, INPUT);

  Serial.println("creators club REV B1");
  Serial.println("test firmware version 0.0");

  delay(2000);

  pinMode(BATTERY_PIN, INPUT);

  pinMode(INT_PIN, INPUT);
//  pinMode(SDA_PIN, OUTPUT);
//  pinMode(SCL_PIN, OUTPUT);
//  digitalWrite(SDA_PIN, LOW);
//  digitalWrite(SCL_PIN, LOW);
  Wire.begin(SDA_PIN,SCL_PIN);
  Wire.setClock(I2C_CLOCK);
  imu_start = bno.begin();
  if(imu_start) {
    int8_t temp = bno.getTemp();
    Serial.print("temperature ");
    Serial.print(temp);
    Serial.println("C");
  } else {
    Serial.println("imu did not start");
  }
/*
  if(WiFi.softAP(ssid, password)) {
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("ip address ");
    Serial.println(myIP);
  } else {
    Serial.println("wifi did not start");
  }
*/
  Serial.println();
  Serial.println("beginning tests");
  Serial.println();
}

int state = 0;
int retries = 0;
int count = 0;
int err = 0;
boolean ir_tx_polarity;
unsigned long timeout;
void loop() {
  sensors_event_t event;
  char d, crc = 0;
  int adc;
  
  FastLED.show();
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  switch(stateCounter(9)) {
    case 0:
      Serial.print("test0: battery ");
      adc = analogRead(BATTERY_PIN);
      Serial.println(adc);
      if ((adc > 2300 && adc < 2800) || adc > 4090) {
        for (int i; i < NUM_LEDS; i++) {
          leds[i] = CRGB::Green;
          LEDS.setBrightness(LED_BRIGHTNESS);
        }
      } else {
        for (int i; i < NUM_LEDS; i++) {
          leds[i] = CRGB::Red;
          LEDS.setBrightness(LED_BRIGHTNESS);
        }
      }
      digitalWrite(IR_TX_PIN, LOW);
      break;

    case 1:
      Serial.println("test1: white");
      for (int i; i < NUM_LEDS; i++) {
        leds[i] = CRGB::White;
        LEDS.setBrightness(LED_BRIGHTNESS);
      }
      break;

    case 2:
      Serial.println("test2: accel");
/*
      bno.getEvent(&event);
      Serial.print("X: ");
      Serial.print(event.orientation.x, 4);
      Serial.print("\tY: ");
      Serial.print(event.orientation.y, 4);
      Serial.print("\tZ: ");
      Serial.print(event.orientation.z, 4);
      Serial.println("");
*/
      if (imu_start) {
        for (int i; i < NUM_LEDS; i++) {
          leds[i] = CRGB::Green;
          LEDS.setBrightness(LED_BRIGHTNESS);
        }
      } else {
        for (int i; i < NUM_LEDS; i++) {
          leds[i] = CRGB::Red;
          LEDS.setBrightness(LED_BRIGHTNESS);
        }
      }
      break;

    case 3:
      Serial.print("test3: radio tx/rx ");
      Serial.println(state, DEC);
      switch(state) {
        case 0: // START
          for (int i; i < NUM_LEDS; i++) {
            leds[i] = CRGB::Yellow;
            LEDS.setBrightness(LED_BRIGHTNESS);
          }

          Serial.println("connecting to wifi");
          WiFi.begin(ssid, password);
          timeout = millis();
          retries = 0;
          count = 0;
          state++;
          break;
        case 1: // CONNECTING TO AP
          if (WiFi.status() == WL_CONNECTED) {
            state++;
          } else {
            if (millis() - timeout > 5000)
              state = 300; // FAIL
          }
          break;
        case 2: // CONNECT TO SERVER
          Serial.print("ip address ");
          Serial.println(WiFi.localIP());

          Serial.println("connecting to server");
          if (client) {
            client->stop();
            delete client;
          }
          client = new WiFiClient();
          if (client->connect(host, port)) {
            String url = "/";
            client->print(String("GET ") + url + " HTTP/1.1\r\n" +
                "Host: " + host + "\r\n" +
                "Connection: close\r\n\r\n");
            timeout = millis();
            state++;
          } else {
            Serial.println("connection failed");
            retries++;
            if (retries < 5) {
              //state = 2; // RETRY
            } else {
              state = 300; // FAIL
            }
          }
          break;
        case 3: // READ HEADER
          if (client->available()) {
            while (client->available()) {
              String line = client->readStringUntil('\r');
              if (line == "\n") break;
              Serial.print(line);
            }
            //client->read();
            client->read();
            Serial.println();
            state++;
          } else {
            if (millis() - timeout > 1000) {
              retries++;
              if (retries < 5) {
                state = 2; // RETRY
              } else {
                state = 300; // FAIL
              }
            }
          }
          break;
        case 4: // READ RESPONSE
          for(int i=0;i<64;i++) {
            if (!client->available()) {
              err++;
              Serial.println();
              Serial.println(i);
              break;
            }
            client->readBytes(phrase, 16);
            if (strcmp(phrase, test_phrase) != 0) {
              err++;
            }
            //Serial.println(strcmp(phrase, test_phrase));
            //d = client->read();
            //Serial.write(d);
            //crc = _crc8_ccitt_update(crc, d);
          }
          //Serial.println();
          //d = client->read();
          //Serial.print(d, HEX);
          //Serial.println();
          //Serial.print(crc, HEX);
          //Serial.println();

          client->stop();
          delete client;
          client = NULL;
          Serial.println("closing connection");
          //Serial.println();
          if (err < 40) {
            count++;
            if (count < 100) {
              retries = 0;
              state = 2; // RECONNECT
            } else {
              Serial.print("total errors ");
              Serial.println(err);
              state = 100; // SUCCESS
            }
          } else {
            state = 300; // FAIL
          }
          break;
        case 100: // SUCCESS
          for (int i; i < NUM_LEDS; i++) {
            leds[i] = CRGB::Green;
            LEDS.setBrightness(LED_BRIGHTNESS);
          }
          WiFi.disconnect();
          break;
        case 300: // FAIL
          for (int i; i < NUM_LEDS; i++) {
            leds[i] = CRGB::Red;
            LEDS.setBrightness(LED_BRIGHTNESS);
          }
          WiFi.disconnect();
          break;
        default:
          break;
      }
      break;

    case 4:
      Serial.println("test4: IR LEDs");
      digitalWrite(IR_TX_PIN, HIGH);
      for (int i; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
        //LEDS.setBrightness(LED_BRIGHTNESS/2);
      }
      Serial.println(digitalRead(IR_RX_PIN));
      break;

    case 5:
      Serial.println("test5: IR Phototransistors");
//      pinMode(IR_TX_PIN, OUTPUT);
//      digitalWrite(IR_TX_PIN, LOW);
      for (int i; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Yellow;
        LEDS.setBrightness(LED_BRIGHTNESS);
      }
      if (state < 100) {
        switch (state % 4) {
          case 0:
            digitalWrite(IR_TX_PIN, LOW);
            state++;
            break;
          case 1:
            if (state == 1) {
              ir_tx_polarity = digitalRead(IR_RX_PIN);
              state++;
            } else {
              if (ir_tx_polarity == digitalRead(IR_RX_PIN)) {
                state++;
              } else {
                state = 300; // FAIL
              }
            }
            break;
          case 2:
            digitalWrite(IR_TX_PIN, HIGH);
            state++;
            break;
          case 3:
            if (!ir_tx_polarity == digitalRead(IR_RX_PIN)) {
              state++;
            } else {
              state = 300; // FAIL
            }
            break;
        }
      } else if (state == 100) {
        for (int i; i < NUM_LEDS; i++) {
          leds[i] = CRGB::Green;
          LEDS.setBrightness(LED_BRIGHTNESS);
        }
      } else {
        for (int i; i < NUM_LEDS; i++) {
          leds[i] = CRGB::Red;
          LEDS.setBrightness(LED_BRIGHTNESS);
        }
      }
//      Serial.println(digitalRead(IR_RX_PIN));
      //digitalWrite(IR_TX_PIN, HIGH);
//      Serial1.begin(IR_BAUD_RATE,SERIAL_8N1,IR_RX_PIN,IR_TX_PIN,false);
//      Serial1.print("feeltheflow");
//      Serial1.end();
      break;

    case 6:
      Serial.println("test6: USB Charge");
      for (int i; i < NUM_LEDS; i++) {
        leds[i] = CRGB::White;
        LEDS.setBrightness(LED_BRIGHTNESS);
      }
      break;

    case 7:
      Serial.println("test7: rainbow");
      static byte gHue;
      gHue++;
      fill_rainbow( leds, NUM_LEDS, gHue, 3);
      LEDS.setBrightness(LED_BRIGHTNESS);
      break;

    case 8:
      Serial.println("test8: burn in");
      for (int i; i < NUM_LEDS; i++) {
        leds[i] = CRGB::White;
        LEDS.setBrightness(192);
      }
      break;

    case 9:
      Serial.println("test9: power off");
      for (int i; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
      }
      // TODO: implement wakeup sources
      //esp_sleep_enable_gpio_wakeup(BUTTON_PIN,0);
      //esp_sleep_enable_ext0_wakeup(GPIO_NUM_32,1);
      // TODO: implement sleep
      //esp_light_sleep_start();
      Serial.println("Daisy, Daisy, give me your answer, do.");
      break;
  }
}

byte stateCounter(byte limit) {
  static byte counter = 0;

  // Update the Bounce instance :
  debouncer.update();

  // Call code if Bounce fell (transition from HIGH to LOW) :
  if ( debouncer.fell() ) {

    // incriment counter.
    if (counter < limit)
      counter++;
    else
      counter = 0;

    state = 0;
  }
  return counter;
}
