#pragma once
DeclareComponentSingleton(Communication, "comm",)

    SerialComponent * serial;
    OSCComponent * osc;
    
    void initInternal() override;

EndDeclareComponent