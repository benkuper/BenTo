#pragma once
DeclareComponentSingleton(Communication, "comm",)

    SerialComponent * serial;
    OSCComponent * osc;
    
    bool initInternal() override;

    void onChildComponentEvent(const ComponentEvent &e) override;

    void sendParameterFeedback(Component * c, Parameter * param);
    void sendConfigFeedback(Component * c, const String &configName, const String &val);

    DeclareEventTypes(MessageReceived);
    DeclareEventNames("MessageReceived");

EndDeclareComponent