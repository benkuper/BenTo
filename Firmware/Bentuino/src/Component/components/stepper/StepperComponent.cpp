FastAccelStepperEngine StepperComponent::engine = FastAccelStepperEngine();
bool StepperComponent::engineIsInit = false;

bool StepperComponent::initInternal(JsonObject o)
{
    if (!engineIsInit)
    {
        NDBG("Init Stepper Engine");
        engine.init();
    }

    stepPin = AddConfigParameter("stepPin", 0);
    dirPin = AddConfigParameter("dirPin", 0);
    enPin = AddConfigParameter("enPin", 0);

    speed = AddConfigParameter("speed", 0);
    accel = AddConfigParameter("accel", 0);

    position = AddParameter("position", 0);
    speed = AddParameter("speed", 0);

    if (stepPin->intValue() > 0)
    {
        stepper = engine.stepperConnectToPin(stepPin->intValue());
        if (stepper)
        {
            stepper->setDirectionPin(dirPin->intValue());
            stepper->setEnablePin(enPin->intValue());
            stepper->setAutoEnable(true);

            stepper->setSpeedInHz(speed->intValue());    // 500 steps/s
            stepper->setAcceleration(accel->intValue()); // 100 steps/s²
            stepper->move(1000);
        }
        else
        {
            NDBG("Could not connect stepper on stepPin " + stepPin->stringValue());
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

void StepperComponent::onParameterEventInternal(const ParameterEvent &e)
{
    if(e.parameter == position)
    {
        stepper->moveTo(position->intValue());
    }
}