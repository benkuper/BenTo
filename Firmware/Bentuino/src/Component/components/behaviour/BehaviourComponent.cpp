ImplementManagerSingleton(Behaviour)

bool BehaviourComponent::initInternal(JsonObject o)
{
    AddStringParamConfig(paramName);
    AddIntParamConfig(comparator);
    // comparator.options = operatorOptions;
    // comparator.numOptions = OPERATOR_MAX;

    AddFloatParamConfig(compareValue);
    AddFloatParamConfig(validationTime);
    AddBoolParamConfig(alwaysTrigger);
    AddBoolParam(valid);
    // valid.readOnly = true;

    AddIntParamConfig(triggerAction);
    // triggerAction.options = triggerActionOptions;
    // triggerAction.numOptions = ActionMax;

    AddStringParamConfig(triggerValue);

    updateTargetParameter();

    return true;
}

void BehaviourComponent::updateInternal()
{
    if (!delayedValidation && valid && validationTime > 0)
    {
        if (millis() / 1000.0f - timeAtValidation >= validationTime)
        {
            trigger();
            delayedValidation = true;
        }
    }
}

void BehaviourComponent::updateTargetParameter()
{
    // DBG("Update target");
    // Parameter *newTarget = nullptr;
    // String pName = paramName;
    // int tcIndex = pName.lastIndexOf('.');

    // String tc = pName.substring(0, tcIndex);  // component name
    // String pn = pName.substring(tcIndex + 1); // parameter name

    // DBG("Target component: " + tc + " parameter: " + pn);

    // if (Component *c = RootComponent::instance->getComponentWithName(tc))
    // {
    //     Parameter *p = c->getParameterWithName(pn);
    //     if (p)
    //         newTarget = p;
    // }

    // if (newTarget == targetParam)
    //     return;

    // if (targetParam != nullptr)
    // {
    //     DBG("Target remove listener here");
    //     targetParam->removeListener(listenerIndex);
    //     listenerIndex = -1;
    // }

    // targetParam = newTarget;

    // if (targetParam != nullptr)
    // {
    //     DBG("Add listener here on " + targetParam->name);
    //     listenerIndex = targetParam->addListener(std::bind(&BehaviourComponent::onTargetParameterChanged, this, std::placeholders::_1));
    // }
}

// void BehaviourComponent::onParameterEventInternal(const ParameterEvent &e)
// {
//     if (e.parameter == &paramName)
//     {
//         updateTargetParameter();
//     }
// }

// void BehaviourComponent::onTargetParameterChanged(const ParameterEvent &e)
// {
//     bool isValid = false;

//     Comparator cmp = (Comparator)comparator.intValue();

//     switch (cmp)
//     {
//     case EQUAL:
//         isValid = e.parameter == compareValue;
//         break;
//     case GREATER:
//         isValid = e.parameter > compareValue;
//         break;
//     case GREATER_EQUAL:
//         isValid = e.parameter >= compareValue;
//         break;
//     case LESS:
//         isValid = e.parameter < compareValue;
//         break;
//     case LESS_EQUAL:
//         isValid = e.parameter <= compareValue;
//         break;

//     default:
//         break;
//     }

//     if (valid != isValid || alwaysTrigger)
//     {
//         if (isValid)
//         {
//             delayedValidation = false;
//             timeAtValidation = millis() / 1000.0f;
//             if (validationTime == 0)
//             {
//                 trigger();
//             }
//         }

//         SetParam(valid, isValid);
//     }
// }

void BehaviourComponent::trigger()
{
    Action a = (Action)triggerAction;
    switch (a)
    {
    case None:
        break;

    case Shutdown:
        RootComponent::instance->shutdown();
        break;
    case LaunchSeq:
        // SequenceComponent::instance->launchSequenceWithName(triggerValue);
        break;
    case LaunchScript:
        // ScriptComponent::instance->launchScriptWithName(triggerValue);
        break;

    case LaunchCommand:
    {
        String cmd = triggerValue;
        cmd.replace("{value}", getParamString(targetParam));
        StringHelpers::processStringMessage(cmd, [this](var *data, int numData)
                                            { sendEvent(CommandLaunched, data, numData); });
    }
    break;

    default:
        break;
    }
}

void BehaviourManagerComponent::onChildComponentEvent(const ComponentEvent &e)
{
    if (e.type == BehaviourComponent::CommandLaunched)
    {
        sendEvent(CommandLaunched, e.data, e.numData);
    }
}