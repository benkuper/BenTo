#pragma once

#define LONGPRESS_TIME 500      // more than 500ms is long press
#define VERYLONGPRESS_TIME 1500 // more than 1500ms is very long press
#define SHORTPRESS_TIME 500     // less than 500ms is short press
#define MULTIPRESS_TIME 300     // each new press shorter than 500ms after the previous one will increase the multiclick
#define BUTTONPRESS_DEBOUNCE 5  // denoising, needs five reads to validate a change

class ButtonComponent : public IOComponent
{
public:
    ButtonComponent(bool _enabled) : IOComponent("button", _enabled) {}

    bool isSystem;

    int debounceCount;
    long timeAtPress;
    int multiPressCount;
    bool isLongPressed;
    bool isVeryLongPressed;

    bool initInternal() override;
    void updateInternal() override;
    void onParameterEvent(const ParameterEvent &e) override;

    DeclareComponentEventTypes(ShortPress, LongPress, VeryLongPress, MultiPress);
    DeclareComponentEventNames("ShortPress", "LongPress", "VeryLongPress", "MultiPress");
};