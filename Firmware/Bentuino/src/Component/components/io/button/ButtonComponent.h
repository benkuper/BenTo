#pragma once

#define LONGPRESS_TIME 500      // more than 500ms is long press
#define VERYLONGPRESS_TIME 1500 // more than 1500ms is very long press
#define SHORTPRESS_TIME 500     // less than 500ms is short press
#define MULTIPRESS_TIME 300     // each new press shorter than 500ms after the previous one will increase the multiclick
#define BUTTONPRESS_DEBOUNCE 5  // denoising, needs five reads to validate a change

class ButtonComponent : public IOComponent
{
public:
    ButtonComponent(const String &name = "button", bool _enabled = false) : IOComponent(name, _enabled) {}

    int debounceCount;
    long timeAtPress;
    

    DeclareParameter(multiPressCount, 0, false);
    DeclareParameter(isLongPressed, false, false);
    DeclareParameter(isVeryLongPressed, false, false);

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void onParameterEventInternal(const ParameterEvent &e) override;

    DeclareComponentEventTypes(ShortPress, LongPress, VeryLongPress, MultiPress);
    DeclareComponentEventNames("ShortPress", "LongPress", "VeryLongPress", "MultiPress");

    // LinkScriptFunctionsStart
    // LinkScriptFunction(ButtonComponent, getState, i, );
    // LinkScriptFunction(ButtonComponent, getMultipress, i, );
    // LinkScriptFunctionsEnd

    // DeclareScriptFunctionReturn0(ButtonComponent, getState, uint32_t) {  return isVeryLongPressed.boolValue()?3:isLongPressed.boolValue()?2:value.intValue(); }
    // DeclareScriptFunctionReturn0(ButtonComponent, getMultipress, uint32_t) {  return multiPressCount.boolValue(); }
};


DeclareComponentSingleton(ButtonManager, "buttons", )

    DeclareRangeConfigParameter(numButtons, BUTTON_MAX_COUNT, 0, BUTTON_MAX_COUNT);

ButtonComponent buttons[BUTTON_MAX_COUNT];

bool initInternal(JsonObject o) override;

EndDeclareComponent