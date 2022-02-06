#pragma once
DeclareComponent(IO, "io", )

    enum PinMode { D_INPUT,
                   D_INPUT_PULLUP,
                   A_INPUT,
                   D_OUTPUT,
                   A_OUTPUT };

int pin;
int mode;
Parameter *value;
float prevValue;

bool initInternal() override;
void updateInternal() override;
void clearInternal() override;

void setupPin();
void updatePin();

EndDeclareComponent