bool IMUComponent::initInternal(JsonObject o)
{
    AddBoolParamConfig(connected);
    connected = false;

    AddIntParam(sendLevel);

    AddIntParamConfig(orientationSendRate);
    AddIntParamConfig(sdaPin);
    AddIntParamConfig(sclPin);

    AddP3DParam(orientation);
    AddP3DParam(accel);
    AddP3DParam(gyro);
    AddP3DParam(linearAccel);
    AddP3DParam(gravity);

    AddFloatParamConfig(orientationXOffset);
    AddIntParamConfig(throwState);
    AddFloatParamConfig(activity);
    AddP2DParamConfig(flatThresholds);
    AddP3DParamConfig(accelThresholds);
    AddFloatParamConfig(diffThreshold);
    AddFloatParamConfig(semiFlatThreshold);
    AddFloatParamConfig(loftieThreshold);
    AddFloatParamConfig(singleThreshold);
    AddFloatParamConfig(angleOffset);
    AddFloatParam(projectedAngle);
    AddFloatParamConfig(xOnCalibration);

    if (sdaPin == 0 || sclPin == 0)
    {
        String npin;
        if (sdaPin == 0)
            npin += "SDA,";
        if (sclPin == 0)
            npin += "SCL";

        NDBG(npin + " pins not defined, not using IMU");
        return false;
    }

    Wire.begin(sdaPin, sclPin);

    if (enabled)
    {
        startIMUTask();
    }

    return true;
}

void IMUComponent::updateInternal()
{
    // NDBG("updateInternal " + String(hasNewData)); // + ", " + String(orientation[1]) + ", " + String(orientation[2]));
    if (!hasNewData)
        return;

    hasNewData = false;
    imuLock = true;

    long curTime = millis();
    int orientationSendMS = 1000 / orientationSendRate;

    if (curTime > timeSinceOrientationLastSent + orientationSendMS)
    {
        if (sendLevel >= 1)
        {
            var oData[3]{orientation[0], orientation[1], orientation[2]};
            sendEvent(OrientationUpdate, oData, 3);
            if (sendLevel >= 2)
            {
                var aData[3]{accel[0], accel[1], accel[2]};
                sendEvent(AccelUpdate, aData, 3);

                var laData[3]{linearAccel[0], linearAccel[1], linearAccel[2]};
                sendEvent(LinearAccelUpdate, laData, 3);

                var gData[3]{gyro[0], gyro[1], gyro[2]};
                sendEvent(GyroUpdate, gData, 3);

                sendEvent(ActivityUpdate);
                sendEvent(ProjectedAngleUpdate);
                // sendEvent(Gravity, gravity, 3);
            }
        }

        timeSinceOrientationLastSent = curTime;
    }

    imuLock = false;
}

void IMUComponent::clearInternal()
{
    shouldStopRead = true;
}

void IMUComponent::onEnabledChanged()
{
    if (enabled)
        startIMUTask();
    else
        shouldStopRead = true;
}

void IMUComponent::startIMUTask()
{
    hasNewData = false,
    shouldStopRead = false;
    imuLock = false;
    xTaskCreate(&IMUComponent::readIMUStatic, "imu", IMU_NATIVE_STACK_SIZE, this, 1, NULL);
}

void IMUComponent::readIMUStatic(void *_imu)
{
    DBG("[IMU] Async Thread Start");
    IMUComponent *imuComp = (IMUComponent *)_imu;

    bool result = imuComp->setupBNO();

    if (!result)
    {
        vTaskDelete(NULL);
        return;
    }

    imuComp->bno.enterNormalMode();

    while (!imuComp->shouldStopRead)
    {
        imuComp->readIMU();
        delay(5);
    }

    imuComp->bno.enterSuspendMode();

    DBG("[IMU] Async Thread Stop");
    vTaskDelete(NULL);
}

bool IMUComponent::setupBNO()
{
    if (connected)
        return true;

    NDBG("Setup BNO...");
    if (!bno.begin())
    {
        NDBG("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        return false;
    }

    bno.setMode(Adafruit_BNO055::OPERATION_MODE_CONFIG);
    bno.setAxisRemap(Adafruit_BNO055::REMAP_CONFIG_P0);
    bno.setAxisSign(Adafruit_BNO055::REMAP_SIGN_P0);
    bno.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);
    bno.setExtCrystalUse(true);

    SetParam(connected, true);
    NDBG("BNO is setup");

    return true;
}

void IMUComponent::readIMU()
{
    if (!enabled)
        return;

    if (imuLock)
        return;

    // NDBG("ReadIMU " + String(hasNewData));
    imu::Quaternion q = bno.getQuat();
    q.normalize();

    imu::Vector<3> euler = q.toEuler();

    if (isnan(euler.x()) || isnan(euler.y()) || isnan(euler.z()))
    {
        //  NDBG("Reading is sh*t (nan)");
        return;
    }
    // NDBG("Euler " + String(euler.x()));

    orientation[0] = (float)(fmodf(((euler.x() * 180 / PI) + orientationXOffset + 180.0f * 5), 360.0f) - 180.0f);
    orientation[1] = (float)(euler.y() * 180 / PI); // Pitch
    orientation[2] = (float)(euler.z() * 180 / PI); // Roll

    imu::Vector<3> acc = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    accel[0] = (float)acc.x();
    accel[1] = (float)acc.y();
    accel[2] = (float)acc.z();

    imu::Vector<3> laccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    linearAccel[0] = (float)laccel.x();
    linearAccel[1] = (float)laccel.y();
    linearAccel[2] = (float)laccel.z();

    imu::Vector<3> gyr = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    gyro[0] = (float)gyr.x();
    gyro[1] = (float)gyr.y();
    gyro[2] = (float)gyr.z();

    // imu::Vector<3> grav = bno.getVector(Adafruit_BNO055::VECTOR_GRAVITY);
    // gravity[0] = grav.x();
    // gravity[1] = grav.y();
    // gravity[2] = grav.z();

    computeThrow();
    computeActivity();
    computeProjectedAngle();

    hasNewData = true;
}

void IMUComponent::computeProjectedAngle()
{
    float eulerRadians[3];
    float lookAt[3];
    float result;

    // Recalculate x orientation for the projected angle, based on xOnCalibration
    float xOrientation = orientation[0];
    float newX = 0;
    if (xOnCalibration < 0)
    {
        if (xOrientation > xOnCalibration)
        {
            if (xOrientation < 0)
            {
                newX = (xOnCalibration * -1) - (xOrientation * -1);
            }
            else
            {
                if (xOrientation + (xOnCalibration * -1) > 180.0f)
                {
                    newX = (360.0f - xOrientation - (xOnCalibration * -1)) * -1;
                }
                else
                {
                    newX = xOrientation + (xOnCalibration * -1);
                }
            }
        }
        else
        {
            newX = (xOnCalibration * -1) - (xOrientation * -1);
        }
    }
    else
    {
        if (xOrientation > xOnCalibration)
        {
            newX = xOrientation - xOnCalibration;
        }
        else
        {
            if ((xOrientation - xOnCalibration) < -180)
            {
                newX = (180.0f - xOnCalibration) + (180.0f - (xOrientation * -1));
            }
            else
            {
                newX = xOrientation - xOnCalibration;
            }
        }
    }

    eulerRadians[0] = newX * PI / 180.0f;
    eulerRadians[1] = orientation[1] * PI / 180.0f;
    eulerRadians[2] = orientation[2] * PI / 180.0f;

    lookAt[0] = cos(eulerRadians[1]) * sin(eulerRadians[0]);
    lookAt[1] = sin(eulerRadians[1]);
    lookAt[2] = cos(eulerRadians[1]) * cos(eulerRadians[0]);

    result = atan(lookAt[1] / lookAt[2]) - PI / 2.0f;

    if (lookAt[2] > 0.0f)
    {
        result = result + PI;
    }

    result = (result / PI) * 0.5f + 0.5f;
    result = fmod((result + angleOffset), 1.0f);

    projectedAngle = result;
}

void IMUComponent::computeThrow()
{
    float maxAccelYZ = max(fabsf(accel[1]), fabsf(accel[2]));
    float maxAccel = max(maxAccelYZ, fabsf(accel[0]));
    float maxLinearAccel = max(max(fabsf(linearAccel[0]), fabsf(linearAccel[1])), fabsf(linearAccel[2]));
    float accLinearDiff = fabsf(maxAccelYZ - maxLinearAccel);

    bool curIsFlat = throwState == 1;
    float flatThresh = curIsFlat ? flatThresholds[1] : flatThresholds[0];
    bool isFlatting = maxAccel < flatThresh;
    bool isFastSpin = false;

    int newState = 0;
    if (isFlatting)
    {
        newState = 1; // flat
    }
    else
    {
        bool curIsThrowing = throwState > 1;
        float throwThresh = curIsThrowing ? accelThresholds[1] : accelThresholds[0];
        throwThresh = isFastSpin ? throwThresh : accelThresholds[2];

        bool accelCheck = maxAccelYZ < throwThresh;

        bool isThrowing = false;

        if (curIsThrowing)
        {
            isThrowing = accelCheck;
        }
        else
        {
            bool diffCheck = accLinearDiff > diffThreshold;
            isThrowing = accelCheck && diffCheck;
        }

        float throwPower = fabsf(accel[0]);
        if (isThrowing)
        {
            if (throwPower < semiFlatThreshold)
                newState = 4;
            else if (throwPower < loftieThreshold)
                newState = 5;
            else if (throwPower < singleThreshold)
                newState = 2;
            else
                newState = 3; // double
        }
    }

    if (newState != throwState)
    {
        throwState = newState;
        var data[1]{throwState};
        sendEvent(ThrowState, data, 1);
    }
}

void IMUComponent::computeActivity()
{
    float maxLinearAccel = max(max(fabsf(linearAccel[0]), fabsf(linearAccel[1])), fabsf(linearAccel[2]));
    maxLinearAccel = (((maxLinearAccel - 0) * (1 - 0)) / (40 - 0)) + 0; // remap to 0..1 range
    maxLinearAccel = min(maxLinearAccel, (float)1.0);

    activity = prevActivity + (maxLinearAccel - prevActivity) * 0.1;
    prevActivity = activity;
}

void IMUComponent::sendCalibrationStatus()
{
    uint8_t system, gyro, accel, mag;
    system = gyro = accel = mag = 0;
    bno.getCalibration(&system, &gyro, &accel, &mag);

    var data[4];
    data[0] = (float)system;
    data[1] = (float)gyro;
    data[2] = (float)accel;
    data[3] = (float)mag;

    sendEvent(CalibrationStatus, data, 4);
}

void IMUComponent::setOrientationXOffset(float offset = 0.0f)
{
    orientationXOffset = offset;
}

void IMUComponent::setProjectAngleOffset(float yaw = 0.0f, float angle = 0.0f)
{
    angleOffset = angle;
    xOnCalibration = yaw;
}

bool IMUComponent::handleCommandInternal(const String &command, var *data, int numData)
{
    if (checkCommand(command, "calibrationStatus", numData, 0))
    {
        sendCalibrationStatus();
        return true;
    }

    return false;
}

// Script functions
