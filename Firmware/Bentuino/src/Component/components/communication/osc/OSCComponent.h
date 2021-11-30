#pragma once
DeclareComponentSingleton(OSC, "osc",)

    bool initInternal() override
    {
        return true;
    }

    void updateInternal()
    {

    }

    void clearInternal()
    {

    }

    DeclareEventTypes(MessageReceived);
    DeclareEventNames("MessageReceived");    
    
EndDeclareComponent