#pragma once

class CommunicationComponent : public Component
{
public:
    CommunicationComponent() : Component(Type_Comm) { instance = this; }
    ~CommunicationComponent() {}
    
    DeclareSingleton(CommunicationComponent);

    SerialComponent serial;
    OSCComponent osc;
    
    bool initInternal(JsonObject o) override;

    void onChildComponentEvent(const ComponentEvent &e) override;

    void sendParameterFeedback(Component * c, Parameter * param);
    void sendMessage(Component * c, const String &mName, const String &val);
    void sendEventFeedback(const ComponentEvent &e);
    
    DeclareComponentEventTypes(MessageReceived);
    DeclareComponentEventNames("MessageReceived");
};