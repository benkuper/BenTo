#include "BehaviourComponent.h"
bool BehaviourComponent::initInternal(JsonObject o)
{
    AddAndSetParameter(paramName);
    AddAndSetParameter(comparator);
    comparator.options = operatorOptions;
    comparator.numOptions = OPERATOR_MAX;

    AddAndSetParameter(compareValue);
    AddAndSetParameter(validationTime);
    AddAndSetParameter(alwaysTrigger);
    AddParameter(valid);
    valid.readOnly = true;

    AddAndSetParameter(triggerAction);
    triggerAction.options = triggerActionOptions;
    triggerAction.numOptions = ActionMax;

    AddAndSetParameter(triggerValue);

    updateTargetParameter();

    return true;
}

void BehaviourComponent::updateInternal()
{
    if (!delayedValidation && valid.boolValue() && validationTime.floatValue() > 0)
    {
        if (millis() / 1000.0f - timeAtValidation >= validationTime.floatValue())
        {
            trigger();
            delayedValidation = true;
        }
    }
}

void BehaviourComponent::updateTargetParameter()
{
    DBG("Update target");
    Parameter *newTarget = nullptr;
    String pName = paramName.stringValue();
    int tcIndex = pName.lastIndexOf('.');

    String tc = pName.substring(0, tcIndex);  // component name
    String pn = pName.substring(tcIndex + 1); // parameter name

    DBG("Target component: " + tc + " parameter: " + pn);

    if (Component *c = RootComponent::instance->getComponentWithName(tc))
    {
        Parameter *p = c->getParameterWithName(pn);
        if (p)
            newTarget = p;
    }

    if (newTarget == targetParam)
        return;

    if (targetParam != nullptr)
    {
        DBG("Target remove listener here");
        targetParam->removeListener(listenerIndex);
        listenerIndex = -1;
    }

    targetParam = newTarget;

    if (targetParam != nullptr)
    {
        DBG("Add listener here on " + targetParam->name);
        listenerIndex = targetParam->addListener(std::bind(&BehaviourComponent::onTargetParameterChanged, this, std::placeholders::_1));
    }
}

void BehaviourComponent::onParameterEventInternal(const ParameterEvent &e)
{
    if (e.parameter == &paramName)
    {
        updateTargetParameter();
    }
}

void BehaviourComponent::onTargetParameterChanged(const ParameterEvent &e)
{
    DBG("Target Parameter Changed");
    bool isValid = false;

    Comparator cmp = (Comparator)comparator.intValue();

    switch (cmp)
    {
    case EQUAL:
        isValid = e.parameter->floatValue() == compareValue.floatValue();
        break;
    case GREATER:
        isValid = e.parameter->floatValue() > compareValue.floatValue();
        break;
    case GREATER_EQUAL:
        isValid = e.parameter->floatValue() >= compareValue.floatValue();
        break;
    case LESS:
        isValid = e.parameter->floatValue() < compareValue.floatValue();
        break;
    case LESS_EQUAL:
        isValid = e.parameter->floatValue() <= compareValue.floatValue();
        break;

    default:
        break;
    }

    if (valid.boolValue() != isValid || alwaysTrigger.boolValue())
    {
        if (isValid)
        {
            DBG("Behaviour " + name + " is valid");
            delayedValidation = false;
            timeAtValidation = millis() / 1000.0f;
            if (validationTime.floatValue() == 0)
            {
                trigger();
            }
        }

        valid.set(isValid);
    }
}

void BehaviourComponent::trigger()
{
    DBG("Trigger Behaviour " + name);

    Action a = (Action)triggerAction.intValue();
    switch (a)
    {
    case None:
        break;

    case Shutdown:
        RootComponent::instance->shutdown();
        break;
    case LaunchSeq:
        // SequenceComponent::instance->launchSequenceWithName(triggerValue.stringValue());
        break;
    case LaunchScript:
        // ScriptComponent::instance->launchScriptWithName(triggerValue.stringValue());
        break;

    case LaunchCommand:
    {
        String cmd = triggerValue.stringValue();
        cmd.replace("{value}", targetParam->stringValue());
        ParsingHelper::processStringMessage(cmd, [this](var* data, int numData){
            sendEvent(CommandLaunched, data, numData);});
    }
    break;

    default:
        break;
    }
}
