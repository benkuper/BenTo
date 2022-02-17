#pragma once

class ScriptComponent : public Component
{
public:
    ScriptComponent() : Component(Type_Script) {}
    ~ScriptComponent() {}

    Script script;

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void clearInternal() override;

    virtual bool handleCommandInternal(const String &command, var *data, int numData);
};