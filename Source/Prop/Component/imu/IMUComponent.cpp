/*
  ==============================================================================

    IMUComponent.cpp
    Created: 8 May 2020 3:08:55pm
    Author:  bkupe

  ==============================================================================
*/

IMUPropComponent::IMUPropComponent(Prop* prop, var params) :
    PropComponent(prop, "IMU", true),
    offsetConfigCC("Orientation Offset Config"),
    angleConfigCC("Angle Config"),
    throwConfigCC("Throw Config"),
    timeAtThrow(0)
{
    saveAndLoadRecursiveData = true;

    feedbackEnabled = true;
    enabled->setValue(false);
    
    editorIsCollapsed = true;

    updateRate = addIntParameter("Update Rate", "Frequency of IMU update", 60, 1, 200);

    orientation = addPoint3DParameter("Orientation", "Orientation of the prop");
    orientation->setControllableFeedbackOnly(true);
    orientation->setBounds(-180, -90, -180, 180, 90, 180);

    sendLevel = addEnumParameter("Send Level", "What to send from the device");
    sendLevel->addOption("No Send", -1)->addOption("Throws only", 0)->addOption("Orientation", 1)->addOption("All", 2);

    accel = addPoint3DParameter("Accel", "Linear Acceleration of the prop");
    accel->setControllableFeedbackOnly(true);

    linearAccel = addPoint3DParameter("Linear Accel", "Linear Acceleration of the prop");
    linearAccel->setControllableFeedbackOnly(true);


    gyro = addPoint3DParameter("Gyro", "Linear Acceleration of the prop");
    gyro->setControllableFeedbackOnly(true);

    //gravity = addPoint3DParameter("Gravity", "Linear Acceleration of the prop");
    //gravity->setControllableFeedbackOnly(true);

    activity = addFloatParameter("Activity", "Activity based on linear-acceleration", 0, 0, 1);
    activity->setControllableFeedbackOnly(true);

    projectedAngle = addFloatParameter("Projected Angle", "Normalized projected angle for ease of use", 0, 0, 1);
    projectedAngle->setControllableFeedbackOnly(true);

    projectedAngleClub = addFloatParameter("Projected Angle Club", "Normalized projected angle for ease of use", 0, 0, 1);
    projectedAngleClub->setControllableFeedbackOnly(true);

    throwState = addEnumParameter("Throw State", "The current detected state of throw");
    throwState->addOption("None", 0)->addOption("Flat", 1)->addOption("Single", 2)->addOption("Double", 3)->addOption("Semi-flat", 4)->addOption("Loftie", 5);

    weightSmoothing = addFloatParameter("Throw Smoothing", "Smoothing of weight when throwing", .2f, 0, 1);
    weightSmoothing2 = addFloatParameter("Catch Smoothing", "Smoothing of weight when catching", .2f, 0, 1);
    
    flatWeight = addFloatParameter("Flat Weight", "Weight of flat", 0, 0, 1);
    flatWeight->setControllableFeedbackOnly(true);
    throwWeight = addFloatParameter("Throw Weight", "Weight of throw", 0, 0, 1);
    throwWeight->setControllableFeedbackOnly(true);
    throwTime = addFloatParameter("Throw Time", "Time of throw", 0, 0, 5);
    throwTime->setControllableFeedbackOnly(true);

    
    offsetConfigCC.editorIsCollapsed = true;
    addChildControllableContainer(&offsetConfigCC);
    angleConfigCC.editorIsCollapsed = true;
    addChildControllableContainer(&angleConfigCC);
    throwConfigCC.editorIsCollapsed = true;
    addChildControllableContainer(&throwConfigCC);

    orientationCalibrate = offsetConfigCC.addTrigger("Calibrate", "");
    orientationXOffset = offsetConfigCC.addFloatParameter("Orientation X Offset", "", 0, -180, 180);

    calibrate = angleConfigCC.addTrigger("Calibrate Yaw", "");
    offset = angleConfigCC.addFloatParameter("Yaw Offset", "", 0, -180, 180);
    angleOffset = angleConfigCC.addFloatParameter("Angle Offset", "", 0, 0, 1);
    invert = angleConfigCC.addBoolParameter("Invert Direction", "", false);

    accelThresholds = throwConfigCC.addPoint2DParameter("Accel Thresholds", "");
    diffThreshold = throwConfigCC.addFloatParameter("Diff Threshold", "", 8, 0);
    flatThresholds = throwConfigCC.addPoint2DParameter("Flat Thresholds", "");
    accelThresholds->setPoint(.8f, 2);
    flatThresholds->setPoint(.8f, 2);

    semiFlatThreshold = throwConfigCC.addFloatParameter("Semi Flat Threshold", "Throws below this value will be detected as semi-flat", 2);
    loftieThreshold = throwConfigCC.addFloatParameter("Loftie Threshold", "Throws below this value will be detected as loftie", 12);
    singleThreshold = throwConfigCC.addFloatParameter("Single Threshold", "Throws below this value will be detected as single", 25);

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
    sendControl(sendLevel->shortName, (int)sendLevel->getValueData());
}

void IMUPropComponent::onContainerParameterChanged(Parameter* p)
{
    PropComponent::onContainerParameterChanged(p);
    if (p == orientation) computeAngle();
    else if (p == sendLevel)
    {
        sendControl(p->shortName, (int)sendLevel->getValueData());
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
    } else if (c == orientationCalibrate) {
        Parameter* p = orientationXOffset;
        sendControl(p->shortName, p->value);
    }
    
    if (cc == &throwConfigCC)
    {
        if (c->type != c->TRIGGER) sendControl(c->shortName, ((Parameter*)c)->value);
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


void IMUPropComponent::update()
{
    bool isThrowing = (int)throwState->getValueData() > 0;
    throwTime->setValue(Time::getMillisecondCounter() / 1000.0f - timeAtThrow);
    float targetWeight = isThrowing ? 1 : 0;
    float targetFlatWeight = (int)throwState->getValueData() == 1 ? 1 : 0;
    
    float curVal = throwWeight->floatValue();
    float curFlatVal = flatWeight->floatValue();
    
    float smooth = targetWeight > curVal ? weightSmoothing->floatValue() : weightSmoothing2->floatValue();
    float smoothFlat = targetFlatWeight > curFlatVal ? weightSmoothing->floatValue() : weightSmoothing2->floatValue();

    throwWeight->setValue(curVal + (targetWeight - curVal) * smooth);
    flatWeight->setValue(curFlatVal + (targetFlatWeight - curFlatVal) * smoothFlat);
}
