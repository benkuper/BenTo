#include "PWMManager.h"

PWMManager::PWMManager() : Component("pwm")
{

}

void PWMManager::init()
{
#ifdef PWM_COUNT
#ifdef ESP32
    for (int i = 0; i < PWM_COUNT; i++)
    {
        if (pwmPins[i] > 0)
        {
            ledcSetup(i, PWM_FREQUENCY, PWM_RESOLUTION);
            ledcAttachPin(pwmPins[i], i);
        }
    }
#endif
#endif
}

void PWMManager::update()
{
#ifdef PWM_COUNT

#endif
}

void PWMManager::setPWM(int index, float value)
{
#ifdef PWM_COUNT
    ledcWrite(index, PWMVAL(value));
#endif
}

void PWMManager::shutdown()
{
#ifdef PWM_COUNT
   for (int i = 0; i < PWM_COUNT; i++) setPWM(i, 0);
#endif
}


bool PWMManager::handleCommand(String command, var *data, int numData)
{
#ifdef PWM_COUNT
    if (checkCommand(command, "set", numData, 2))
    {
        setPWM(data[0].intValue(), data[1].floatValue());
        return true;
    }
#endif

    return false;
}