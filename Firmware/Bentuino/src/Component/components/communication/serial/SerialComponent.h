#pragma once

DeclareComponentSingleton(Serial, "serial", )

    char buffer[512];
byte bufferIndex;
DeclareBoolParam(sendFeedback, false);

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

void processMessage(String buffer);
void sendMessage(String source, String command, var *data, int numData);

void send(const String &message);

DeclareComponentEventTypes(MessageReceived);
DeclareComponentEventNames("MessageReceived");

HandleSetParamInternalStart
    CheckAndSetParam(sendFeedback);
HandleSetParamInternalEnd;

FillSettingsInternalStart
    FillSettingsParam(sendFeedback);
FillSettingsInternalEnd;

FillOSCQueryInternalStart
    FillOSCQueryBoolParam(sendFeedback);
FillOSCQueryInternalEnd

    EndDeclareComponent