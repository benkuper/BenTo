/*
  ==============================================================================

    IMUComponent.cpp
    Created: 8 May 2020 3:08:55pm
    Author:  bkupe

  ==============================================================================
*/

#include "IMUComponent.h"

IMUPropComponent::IMUPropComponent(Prop* prop, var params) :
    PropComponent(prop, "IMU", true),
    angleConfigCC("Angle Config"),
    throwConfigCC("Throw Config"),
    trailIndex(0),
    inSpeed(false)
{
    feedbackEnabled = true;
    enabled->setValue(false);
    
    editorIsCollapsed = true;

    orientation = addPoint3DParameter("Orientation", "Orientation of the prop");
    orientation->setControllableFeedbackOnly(true);
    orientation->setBounds(-180, -90, -180, 180, 90, 180);

    accel = addPoint3DParameter("Accel", "Linear Acceleration of the prop");
    accel->setControllableFeedbackOnly(true);

    linearAccel = addPoint3DParameter("Linear Accel", "Linear Acceleration of the prop");
    linearAccel->setControllableFeedbackOnly(true);


    //gyro = addPoint3DParameter("Gyro", "Linear Acceleration of the prop");
//gyro->setControllableFeedbackOnly(true);

//gravity = addPoint3DParameter("Gravity", "Linear Acceleration of the prop");
//gravity->setControllableFeedbackOnly(true);

    projectedAngle = addFloatParameter("Projected Angle", "Normalized projected angle for ease of use", 0, 0, 1);
    projectedAngle->setControllableFeedbackOnly(true);

    throwState = addEnumParameter("Throw State", "The current detected state of throw");
    throwState->addOption("None", 0)->addOption("Flat", 1)->addOption("Single", 2)->addOption("Double", 3)->addOption("Semi-flat", 4)->addOption("Loftie", 5);
    //throwState->setControllableFeedbackOnly(true);

    angleConfigCC.editorIsCollapsed = true;
    addChildControllableContainer(&angleConfigCC);
    throwConfigCC.editorIsCollapsed = true;
    addChildControllableContainer(&throwConfigCC);


    calibrate = angleConfigCC.addTrigger("Calibrate Yaw", "");
    offset = angleConfigCC.addFloatParameter("Yaw Offset", "", 0, -180, 180);
    angleOffset = angleConfigCC.addFloatParameter("Angle Offset", "", 0, 0, 1);
    invert = angleConfigCC.addBoolParameter("Invert Direction", "", false);

    var curAccel;

    maxTrail = throwConfigCC.addIntParameter("Trail Count", "Number of values to keep to average the accel data", 5, 1, 100);
    throwThresholds = throwConfigCC.addPoint2DParameter("Throw Thresholds", "");
    speedThresholds = throwConfigCC.addPoint2DParameter("Speed Thresholds", "");
    flatThresholds = throwConfigCC.addPoint2DParameter("Flat Thresholds", "");
    throwThresholds->setPoint(.5f, 2);
    speedThresholds->setPoint(.2f, 1);
    flatThresholds->setPoint(.5f, 2);

    semiFlatThreshold = throwConfigCC.addFloatParameter("Semi-flat Threshold", "Throws below this value will be detected as semi-flat", 2);
    loftieThreshold = throwConfigCC.addFloatParameter("Loftie Threshold", "Throws below this value will be detected as loftie", 12);
    singleThreshold = throwConfigCC.addFloatParameter("Single Threshold", "Throws below this value will be detected as single", 25);

    trail.resize(maxTrail->intValue());

    //set at end to avoid bad init
    linearAccel->setBounds(-100, -100, -100, 100, 100, 100);
    accel->setBounds(-100, -100, -100, 100, 100, 100);
}



IMUPropComponent::~IMUPropComponent()
{
}

void IMUPropComponent::handePropConnected()
{
    sendControl("enabled", enabled->boolValue());
}

void IMUPropComponent::onContainerParameterChanged(Parameter* p)
{
    if (p == accel)
    {
        computeAngle();
        computeThrows();

    }if (p == maxTrail)
    {
        trail.resize(maxTrail->intValue());
    }
}

void IMUPropComponent::onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c)
{
    PropComponent::onControllableFeedbackUpdate(cc, c);
    offset->setValue(orientation->x);
}

void IMUPropComponent::computeAngle()
{
    if (!enabled->boolValue()) return;

    Vector3D<float> euler = accel->getVector();
    euler.x += offset->floatValue();
    Vector3D<float> eulerRadians = euler * float_Pi / 180;

    Vector3D<float> lookAt(cos(euler.y) * sinf(euler.x), sinf(euler.y), cosf(euler.y) * cosf(euler.x));

    float result = atanf(lookAt.y / lookAt.z) - float_Pi / 2;
    if (lookAt.z > 0) result += float_Pi;

    result = (result / float_Pi) * .5 + .5;
    result = fmodf(result + angleOffset->floatValue(), 1);
    if (invert->boolValue()) result = 1 - result;
    projectedAngle->setValue(result);
}

void IMUPropComponent::computeThrows()
{
    if (!enabled->boolValue()) return;
   
    trail.set(trailIndex, accel->getVector());
    trailIndex = (trailIndex + 1) % trail.size();

    Vector3D<float> sa;
    for (auto& ti : trail) sa += ti;
   
    Vector3D<float> prevSA = smoothAccel;
    Vector3D<float> aSpeed;
    float maxYZSpeed = 0;

    sa /= maxTrail->intValue();
    
    float maxLinear = jmax(fabsf(sa.x), fabsf(sa.y), fabsf(sa.z));

    aSpeed = sa - prevSA;
    maxYZSpeed = jmax(aSpeed.y, aSpeed.y);

    int curThrowState = (int)throwState->getValueData();
    
    bool curIsThrowing = curThrowState > 0;
    bool curIsFlat = curThrowState == 1;

    float throwThresh = curIsThrowing ? throwThresholds->y : throwThresholds->x;
    float speedThresh = inSpeed ? speedThresholds->y : speedThresholds->x;
    float flatThresh = curIsFlat ? flatThresholds->y : flatThresholds->x;

    bool throwPotential = sa.y < throwThresh;

    inSpeed = maxYZSpeed < speedThresh;

    bool isThrowing = throwPotential && inSpeed;
    bool isFlatting = maxLinear < flatThresh;

    if (isThrowing != curIsThrowing || isFlatting != curIsFlat)
    {
        int result = 0;
        if (isThrowing)
        {
            if (isFlatting) result = 1;
            else if (sa.x < semiFlatThreshold->floatValue()) result = 4;
            else if (sa.y < loftieThreshold->floatValue()) result = 5;
            else if (sa.z < singleThreshold->floatValue()) result = 2;
            else result = 3;
        }

        throwState->setValueWithData(result);
    }
}
