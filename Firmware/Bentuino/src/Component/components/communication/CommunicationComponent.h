#pragma once
DeclareComponentSingleton(Communication, "comm",)

    SerialComponent * serial;
    OSCComponent * osc;
    
    bool initInternal() override;

    void onChildComponentEvent(const ComponentEvent &e) override;

    void sendParameterFeedback(Component * c, Parameter * param);

    DeclareEventTypes(MessageReceived);
    DeclareEventNames("MessageReceived");

EndDeclareComponent