#pragma once

#define LEDSTRIP_NUM_USER_LAYERS 3

DeclareComponent(LedStrip, "ledstrip", )

int pin;
int count;
Parameter * brightness;

Color *colors;

// user layers, may be more than one later
LedStripBakeLayer *bakeLayer;
LedStripStreamLayer *streamLayer;
LedStripScriptLayer *scriptLayer;

// on top of all
LedStripSystemLayer *systemLayer;

LedStripLayer *userLayers[LEDSTRIP_NUM_USER_LAYERS];

Adafruit_NeoPixel *strip;

bool initInternal() override;
void updateInternal() override;
void clearInternal() override;

// Layer functions
void processLayer(LedStripLayer *layer);

// Color functions
void clearColors();
void showLeds();

EndDeclareComponent