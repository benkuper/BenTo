#pragma once

DeclareComponentSingleton(Serial, "serial",)

    bool initInternal() override
    {
        Serial.begin(115200);
        return true;
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