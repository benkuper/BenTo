#pragma once

#define LONGPRESS_TIME 500      // more than 500ms is long press
#define VERYLONGPRESS_TIME 1500 // more than 1500ms is very long press
#define SHORTPRESS_TIME 500     // less than 500ms is short press
#define MULTIPRESS_TIME 300     // each new press shorter than 500ms after the previous one will increase the multiclick
#define BUTTONPRESS_DEBOUNCE 5  // denoising, needs five reads to validate a change

class ButtonComponent : public IOComponent
{
public:
    ButtonComponent(const String &name, bool _enabled) : IOComponent(name, _enabled) {}

    Parameter *isSystem;

    int debounceCount;
    long timeAtPress;
    int multiPressCount;
    bool isLongPressed;
    bool isVeryLongPressed;

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void onParameterEvent(const ParameterEvent &e) override;

    DeclareComponentEventTypes(ShortPress, LongPress, VeryLongPress, MultiPress);
    DeclareComponentEventNames("ShortPress", "LongPress", "VeryLongPress", "MultiPress");

    LinkScriptFunctionsStart
    LinkScriptFunction(ButtonComponent, getState, i, );
    LinkScriptFunction(ButtonComponent, getMultipress, i, );
    LinkScriptFunctionsEnd

    DeclareScriptFunctionReturn0(ButtonComponent, getState, uint32_t) {  return isVeryLongPressed?3:isLongPressed?2:value->intValue(); }
    DeclareScriptFunctionReturn0(ButtonComponent, getMultipress, uint32_t) {  return multiPressCount; }
};