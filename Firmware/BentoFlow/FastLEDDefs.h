#ifndef FASTLED_DEFS_H
#define FASTLEF_DEFS_H
#pragma once

#define NUM_LEDS 32 //need that global
CRGBArray<NUM_LEDS> leds; //need that global
CRGBArray<NUM_LEDS> snapshotLeds; //for snapshot

//Helper functions
void setBrightnessFactor(float value)
{
  FastLED.setBrightness((int)(value * 255));
}

void setFullColor(CRGB color)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = color;
  }

  FastLED.show();
}

void setRange(int startIndex, int endIndex, CRGB color, bool unique = false)
{
  if (unique) FastLED.clear();

  if (startIndex > endIndex)
  {
    FastLED.show();
    return;
  }

  if (startIndex < 0) startIndex = 0;
  if (startIndex >= NUM_LEDS) startIndex = NUM_LEDS - 1;
  if (endIndex < 0) endIndex = 0;
  if (endIndex >= NUM_LEDS) endIndex = NUM_LEDS - 1;

  for (int i = startIndex; i <= endIndex; i++)
  {
    leds[i] = color;
  }

  FastLED.show();
}

void setLed(int index, CRGB color, bool unique = false)
{
  if (unique) FastLED.clear();

  if (index < 0 || index >= NUM_LEDS)
  {
    FastLED.show();
    return;
  }

  leds[index] = color;

  FastLED.show();
}

void simpleSetLed(int index, CRGB color)
{
  leds[index] = color;
}


//Helper
int getRandomNumber(int startNum, int endNum) {
  randomSeed(ESP.getChipId());
  return random(startNum, endNum);
}

void initSnapshot()
{
  for(int i=0;i<NUM_LEDS;i++)
  {
    snapshotLeds[i] = CHSV(getRandomNumber(0,255),255,255);
  }
}

 void takeSnapshot()
{
  for(int i=0;i<NUM_LEDS;i++)
  {
    snapshotLeds[i] = CRGB(leds[i]);
  }
}

 void showSnapshot()
{
  for(int i=0;i<NUM_LEDS;i++)
  {
    leds[i] = CRGB(snapshotLeds[i]);
  }
}
  
#endif
