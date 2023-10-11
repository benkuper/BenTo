FastAccelStepperEngine StepperComponent::engine = FastAccelStepperEngine();
bool StepperComponent::engineIsInit = false;

bool StepperComponent::initInternal(JsonObject o)
{
    if (!engineIsInit)
    {
        NDBG("Init Stepper Engine");
        engine.init();
    }

    AddIntParam(stepPin);
    AddIntParam(dirPin);
    AddIntParam(enPin);

    AddFloatParam(accel);
    AddFloatParam(speed);

    AddFloatParam(position);

    if (stepPin > 0)
    {
        stepper = engine.stepperConnectToPin(stepPin);
        if (stepper)
        {
            stepper->setDirectionPin(dirPin);
            stepper->setEnablePin(enPin);
            stepper->setAutoEnable(true);

            stepper->setSpeedInHz(speed);    // 500 steps/s
            stepper->setAcceleration(accel); // 100 steps/s²
            stepper->move(1000);
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
}

// void StepperComponent::onParameterEventInternal(const ParameterEvent &e)
// {
//     if (e.parameter == &position)
//     {
//         stepper->moveTo(position);
//     }
// }