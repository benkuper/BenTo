#pragma once

class SerialComponent : public Component
{
public:
    SerialComponent() : Component(Type_Serial) { instance = this; }
    ~SerialComponent() {}
    
    DeclareSingleton(SerialComponent);

    char buffer[512];
    byte bufferIndex;

    Parameter * feedbackEnabled;

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void clearInternal() override;

    
    void processMessage(String buffer);
    void sendMessage(String source, String command, var * data, int numData);

    void send(const String &message);

    DeclareComponentEventTypes(MessageReceived);
    DeclareComponentEventNames("MessageReceived");
};