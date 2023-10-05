#pragma once

DeclareComponentSingleton(Serial, "serial",)

    char buffer[512];
    byte bufferIndex;

    Parameter sendFeedback{"sendFeedback", false, var(), var(), true};

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void clearInternal() override;

    
    void processMessage(String buffer);
    void sendMessage(String source, String command, var * data, int numData);

    void send(const String &message);

    DeclareComponentEventTypes(MessageReceived);
    DeclareComponentEventNames("MessageReceived");

EndDeclareComponent