#include "StepperComponent.h"
FastAccelStepperEngine StepperComponent::engine = FastAccelStepperEngine();
bool StepperComponent::engineIsInit = false;

bool StepperComponent::initInternal(JsonObject o)
{
    if (!engineIsInit)
    {
        NDBG("Init Stepper Engine");
        engine.init(1);
    }

    AddIntParamConfig(stepPin);
    AddIntParamConfig(dirPin);
    AddIntParamConfig(enPin);

    AddFloatParamConfig(accel);
    AddFloatParamConfig(speed);
    AddFloatParam(position);

    if (stepPin > -1)
    {
        stepper = engine.stepperConnectToPin(stepPin);
        if (stepper)
        {
            stepper->setDirectionPin(dirPin);
            if (enPin > -1)
            {
                NDBG("Set enable pin "+String(enPin));
                delay(500);
                stepper->setEnablePin(enPin);
                stepper->enableOutputs();
                // stepper->setAutoEnable(true);
            }

            stepper->setSpeedInHz(speed);    // 500 steps/s
            stepper->setAcceleration(accel); // 100 steps/s

            NDBG("Connected stepper on stepPin " + String(stepPin)+", dirPin "+String(dirPin)+", enPin "+String(enPin));
        }
        else
        {
            NDBG("Could not connect stepper on stepPin " + String(stepPin));
            return false;
        }
    }
    else
    {
        NDBG("stepPin is not defined, not connecting stepper.");
        return false;
    }

    return true;
}

void StepperComponent::updateInternal()
{
}

void StepperComponent::clearInternal()
{
    stepper->disableOutputs();
}

void StepperComponent::paramValueChangedInternal(void *param)
{
    if(!isInit) return;

    if (param == &position)
    {
        NDBG("Move to position "+String(position));
        stepper->moveTo(position);
    }else if(param == &speed)
    {
        NDBG("Set speed to "+String(speed));
        stepper->setSpeedInHz(speed);
    }else if(param == &accel)
    {
        NDBG("Set acceleration to "+String(accel));
        stepper->setAcceleration(accel);
    }
}

void StepperComponent::onEnabledChanged()
{
    if(!isInit) return;
    if(enabled) stepper->enableOutputs();
    else stepper->disableOutputs();
}