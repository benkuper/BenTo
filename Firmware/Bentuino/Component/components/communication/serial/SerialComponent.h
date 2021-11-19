#pragma once
DeclareComponentSingleton(Serial, "serial",)

    void initInternal() override
    {
        Serial.begin(115200);
    }

    void updateInternal()
    {

    }

    void clearInternal()
    {

    }

    void send(const String &message)
    {
        Serial.println(message);
    }

EndDeclareComponent