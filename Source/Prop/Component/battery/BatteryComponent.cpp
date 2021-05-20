/*
  ==============================================================================

    BatteryComponent.cpp
    Created: 8 May 2020 3:08:03pm
    Author:  bkupe

  ==============================================================================
*/

BatteryPropComponent::BatteryPropComponent(Prop* prop, var params) :
    PropComponent(prop, "Battery")
{
    resetBattery = addTrigger("Reset", "Reset Battery Max value calibration");

    level = addFloatParameter("Level", "Level of the battery", 1, 0, 1);
    level->setControllableFeedbackOnly(true);

    voltage = addFloatParameter("Voltage", "Voltage of the battery", 0);
    voltage->setControllableFeedbackOnly(true);

    rawValue = addFloatParameter("Raw Value", "Raw value of the battery", 0);
    rawValue->setControllableFeedbackOnly(true);
}

BatteryPropComponent::~BatteryPropComponent()
{

}
