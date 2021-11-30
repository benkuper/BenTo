#pragma once
DeclareComponentSingleton(Communication, "comm",)

    SerialComponent * serial;
    OSCComponent * osc;
    
    bool initInternal() override;

EndDeclareComponent