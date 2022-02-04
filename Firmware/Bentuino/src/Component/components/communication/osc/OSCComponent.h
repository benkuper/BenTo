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

    DeclareComponentEventTypes(MessageReceived);
    DeclareComponentEventNames("MessageReceived");    
    
EndDeclareComponent