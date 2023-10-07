#pragma once
#include "../common/Common.h"

#ifdef HAS_PWM
class PWMManager : public Component {
public:
    PWMManager();
    ~PWMManager(){}
    
    //source
#ifdef PWM_COUNT
    float values[PWM_COUNT];
#endif

    void init();
    void update();

    void setPWM(int index, float value);

    void shutdown();
    
    bool handleCommand(String command, var *data, int numData) override;
};

#endif