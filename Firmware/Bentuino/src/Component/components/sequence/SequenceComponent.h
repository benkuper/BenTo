#pragma once
class SequenceComponent : public Component
{
public:
    SequenceComponent() : Component(Type_Sequence) { instance = this; }
    ~SequenceComponent() {}

    DeclareSingleton(SequenceComponent);

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    void clearInternal() override;
};