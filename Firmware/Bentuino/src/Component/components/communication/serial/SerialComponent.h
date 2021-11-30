#pragma once

DeclareComponentSingleton(Serial, "serial",)

    char buffer[512];
    byte bufferIndex;

    Parameter * feedbackEnabled;

    bool initInternal() override;

    void updateInternal();
    void clearInternal();

    
    void processMessage(String buffer);
    void sendMessage(String source, String command, var * data, int numData);

    void send(const String &message);

    DeclareEventTypes(MessageReceived);
    DeclareEventNames("MessageReceived");

EndDeclareComponent