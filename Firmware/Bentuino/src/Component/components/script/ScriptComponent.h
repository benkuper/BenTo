#pragma once

DeclareComponent(Script, "script", )

    Script script;

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

virtual bool handleCommandInternal(const String &command, var *data, int numData);

EndDeclareComponent