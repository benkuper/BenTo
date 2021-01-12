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
    saveAndLoadRecursiveData = true;

    feedbackEnabled = true;
    enabled->setValue(false);
    
    editorIsCollapsed = true;

    orientation = addPoint3DParameter("Orientation", "Orientation of the prop");
    orientation->setControllableFeedbackOnly(true);
    orientation->setBounds(-180, -90, -180, 180, 90, 180);

    sendRawData = addBoolParameter("Send Raw Data", "Send raw data from sensor", false);

    accel = addPoint3DParameter("Accel", "Linear Acceleration of the prop");
    accel->setControllableFeedbackOnly(true);

    linearAccel = addPoint3DParameter("Linear Accel", "Linear Acceleration of the prop");
    linearAccel->setControllableFeedbackOnly(true);


    gyro = addPoint3DParameter("Gyro", "Linear Acceleration of the prop");
    gyro->setControllableFeedbackOnly(true);

    //gravity = addPoint3DParameter("Gravity", "Linear Acceleration of the prop");
    //gravity->setControllableFeedbackOnly(true);

    projectedAngle = addFloatParameter("Projected Angle", "Normalized projected angle for ease of use", 0, 0, 1);
    projectedAngle->setControllableFeedbackOnly(true);

    throwState = addEnumParameter("Throw State", "The current detected state of throw");
    throwState->addOption("None", 0)->addOption("Flat", 1)->addOption("Single", 2)->addOption("Double", 3)->addOption("Semi-flat", 4)->addOption("Loftie", 5);

    weightSmoothing = addFloatParameter("Throw Smoothing", "Smoothing of weight when throwing", .2f, 0, 1);
    weightSmoothing2 = addFloatParameter("Catch Smoothing", "Smoothing of weight when catching", .6f, 0, 1);
    
    throwWeight = addFloatParameter("Throw Weight", "Weight of throw", 0, 0, 1);
    throwWeight->setControllableFeedbackOnly(true);
    throwTime = addFloatParameter("Throw Time", "Time of throw", 0, 0, 5);
    throwTime->setControllableFeedbackOnly(true);

    
    angleConfigCC.editorIsCollapsed = true;
    addChildControllableContainer(&angleConfigCC);
    throwConfigCC.editorIsCollapsed = true;
    addChildControllableContainer(&throwConfigCC);

    calibrate = angleConfigCC.addTrigger("Calibrate Yaw", "");
    offset = angleConfigCC.addFloatParameter("Yaw Offset", "", 0, -180, 180);
    angleOffset = angleConfigCC.addFloatParameter("Angle Offset", "", 0, 0, 1);
    invert = angleConfigCC.addBoolParameter("Invert Direction", "", false);

    computeLocally = throwConfigCC.addBoolParameter("Compute Locally", "If enabled, this will compute throws locally instead of on board", false);
    maxTrail = throwConfigCC.addIntParameter("Trail Count", "Number of values to keep to average the accel data", 1, 1, 100);
    throwThresholds = throwConfigCC.addPoint2DParameter("Throw Thresholds", "");
    speedThresholds = throwConfigCC.addPoint2DParameter("Speed Thresholds", "");
    flatThresholds = throwConfigCC.addPoint2DParameter("Flat Thresholds", "");
    throwThresholds->setPoint(.7f, 2);
    speedThresholds->setPoint(.2f, 1);
    flatThresholds->setPoint(.8f, 2);

    semiFlatThreshold = throwConfigCC.addFloatParameter("Semi Flat Threshold", "Throws below this value will be detected as semi-flat", 2);
    loftieThreshold = throwConfigCC.addFloatParameter("Loftie Threshold", "Throws below this value will be detected as loftie", 12);
    singleThreshold = throwConfigCC.addFloatParameter("Single Threshold", "Throws below this value will be detected as single", 25);

    trail.resize(maxTrail->intValue());

    //set at end to avoid bad init
    linearAccel->setBounds(-50, -50, -50, 50, 50, 50);
    accel->setBounds(-50, -50, -50, 50, 50, 50);


    excludeControlControllables.add(throwState);
    excludeControlControllables.add(weightSmoothing);
    excludeControlControllables.add(weightSmoothing2);
}


IMUPropComponent::~IMUPropComponent()
{
}

void IMUPropComponent::handePropConnected()
{
    sendControl(enabled->shortName, enabled->boolValue());
    sendControl(sendRawData->shortName, sendRawData->boolValue());
}

void IMUPropComponent::onContainerParameterChanged(Parameter* p)
{
    PropComponent::onContainerParameterChanged(p);
    if (p == orientation) computeAngle();
    else if(p == accel) computeThrows();
    else if (p == maxTrail)
    {
        trail.resize(maxTrail->intValue());
    }
    else if (p == sendRawData)
    {
        sendControl(p->shortName, p->boolValue());
    }
    else if (p == throwState)
    {
        bool isThrowing = (int)throwState->getValueData() > 0;
        if (isThrowing)
        {
            timeAtThrow = Time::getMillisecondCounter() / 1000.0f;
        }
    }
}

void IMUPropComponent::onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c)
{
    PropComponent::onControllableFeedbackUpdate(cc, c);
    if (c == calibrate)
    {
        float tVal = orientation->x - 180;
        if (tVal < -180) tVal += 360;
        offset->setValue(tVal);
    }if (cc == &throwConfigCC)
    {
        if (c != computeLocally)
        {
            if (c->type != c->TRIGGER) sendControl(c->shortName, ((Parameter*)c)->value);
        }
    }
}

void IMUPropComponent::computeAngle()
{
    if (!enabled->boolValue()) return;

    Vector3D<float> euler = orientation->getVector();
    euler.x += offset->floatValue();
    Vector3D<float> eulerRadians = euler * float_Pi / 180;
    Vector3D<float> lookAt(cosf(eulerRadians.y) * sinf(eulerRadians.x), sinf(eulerRadians.y), cosf(eulerRadians.y) * cosf(eulerRadians.x));

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
    if (!computeLocally->boolValue()) return;

    Vector3D<float> curAccel = accel->getVector();

    int trailCount = maxTrail->intValue();
    trail.set(trailIndex, curAccel);
    trailIndex = (trailIndex + 1) % trailCount;

    Vector3D<float> sa;
    for (int i = 0; i < trailCount;i++) sa += trail[i];
    sa /= maxTrail->intValue();

    Vector3D<float> prevSA = smoothAccel;
    Vector3D<float> aSpeed = sa - prevSA;
    float maxYZSpeed = jmax(fabsf(aSpeed.y), fabsf(aSpeed.y));
    float maxSmoothed = jmax(fabsf(sa.x), fabsf(sa.y), fabsf(sa.z));
    //float maxRaw = jmax(fabsf(curAccel.x), fabsf(curAccel.y), fabsf(curAccel.z));

    smoothAccel = sa;

    int curThrowState = (int)throwState->getValueData();
    
    bool curIsThrowing = curThrowState > 0;
    bool curIsFlat = curThrowState == 1;

    float throwThresh = curIsThrowing ? throwThresholds->y : throwThresholds->x;
    float speedThresh = inSpeed ? speedThresholds->y : speedThresholds->x;
    float flatThresh = curIsFlat ? flatThresholds->y : flatThresholds->x;

    bool throwPotential = fabsf(sa.y) < throwThresh;

    inSpeed = maxYZSpeed < speedThresh;

    bool isThrowing = throwPotential && inSpeed;
    bool isFlatting = maxSmoothed < flatThresh;

    if (isThrowing != curIsThrowing || isFlatting != curIsFlat)
    {
        int result = 0;
        if (isThrowing)
        {
            if (isFlatting) result = 1;
            else if (sa.x < semiFlatThreshold->floatValue()) result = 4;
            else if (sa.x < loftieThreshold->floatValue()) result = 5;
            else if (sa.x < singleThreshold->floatValue()) result = 2;
            else result = 3;
        }
        else
        {
            throwTime->setValue(0);
        }

        throwState->setValueWithData(result);
    }
}

void IMUPropComponent::update()
{
    bool isThrowing = (int)throwState->getValueData() > 0;
    throwTime->setValue(Time::getMillisecondCounter() / 1000.0f - timeAtThrow);
    float targetWeight = isThrowing ? 1 : 0;
    float curVal = throwWeight->floatValue();
    float smooth = targetWeight > curVal ? weightSmoothing->floatValue() : weightSmoothing2->floatValue();
    throwWeight->setValue(curVal + (targetWeight - curVal) * smooth);
}
