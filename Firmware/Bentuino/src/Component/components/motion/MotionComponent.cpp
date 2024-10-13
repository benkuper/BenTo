bool MotionComponent::initInternal(JsonObject o)
{
    saveEnabled = false;
    enabled = false;

    AddBoolParamConfig(connected);
    connected = false;

    AddIntParam(sendLevel);

    AddIntParamConfig(orientationSendRate);

#ifdef IMU_TYPE_BNO055
    AddIntParamConfig(sdaPin);
    AddIntParamConfig(sclPin);
#endif

    AddP3DParam(orientation);
    AddP3DParam(accel);
    AddP3DParam(gyro);
    AddP3DParam(linearAccel);
    // AddP3DParam(gravity);

    AddFloatParamConfig(orientationXOffset);
    AddIntParam(throwState);
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

#ifdef IMU_TYPE_BNO055
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
#endif

    if (enabled)
    {
        startIMUTask();
    }

    return true;
}

void MotionComponent::updateInternal()
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
            SendMultiParamFeedback(orientation);

            if (sendLevel >= 2)
            {
                SendMultiParamFeedback(accel);
                SendMultiParamFeedback(gyro);
                SendMultiParamFeedback(linearAccel);
                SendParamFeedback(projectedAngle);

                // sendEvent(ActivityUpdate);
                // sendEvent(ProjectedAngleUpdate);
                // sendEvent(Gravity, gravity, 3);
            }
        }

        timeSinceOrientationLastSent = curTime;
    }

    imuLock = false;
}

void MotionComponent::clearInternal()
{
    shouldStopRead = true;
#ifdef IMU_TYPE_BNO055
    bno.enterSuspendMode();
#endif
}

void MotionComponent::onEnabledChanged()
{
    if (enabled)
        startIMUTask();
    else
        shouldStopRead = true;
}

void MotionComponent::startIMUTask()
{
    hasNewData = false,
    shouldStopRead = false;
    imuLock = false;
    xTaskCreate(&MotionComponent::readIMUStatic, "imu", IMU_NATIVE_STACK_SIZE, this, 1, NULL);
}

void MotionComponent::readIMUStatic(void *_imu)
{
    DBG("[motion] Start reading IMU");
    MotionComponent *imuComp = (MotionComponent *)_imu;

    bool result = imuComp->setupIMU();

    if (!result)
    {
        vTaskDelete(NULL);
        return;
    }

#ifdef IMU_TYPE_BNO055
    imuComp->bno.enterNormalMode();
#endif

    while (!imuComp->shouldStopRead)
    {
        imuComp->readIMU();
        delay(5);
    }

#ifdef IMU_TYPE_BNO055
    imuComp->bno.enterSuspendMode();
#endif

    DBG("[motion] Stopped reading IMU");
    vTaskDelete(NULL);
}

bool MotionComponent::setupIMU()
{
    connected = false;

#ifdef IMU_TYPE_BNO055
    NDBG("Setup BNO...");
    if (!bno.begin())
    {
        NDBG("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        return false;
    }

    bno.setMode(adafruit_bno055_opmode_t::OPERATION_MODE_CONFIG);
    bno.setAxisRemap(Adafruit_BNO055::REMAP_CONFIG_P0);
    bno.setAxisSign(Adafruit_BNO055::REMAP_SIGN_P0);
    bno.setMode(adafruit_bno055_opmode_t::OPERATION_MODE_NDOF);
    // bno.setExtCrystalUse(true); //works on club but not on tab
    bno.enterNormalMode();

    SetParam(connected, true);
#elif defined IMU_TYPE_M5MPU
    int result = mpu.Init();
    if (result != 0)
    {
        NDBG("Error initializing MPU6886");
        return false;
    }
    lastUpdateTime = millis();
#endif

    NDBG("IMU is setup");

    return true;
}

void MotionComponent::readIMU()
{
    if (!enabled)
        return;

    if (imuLock)
        return;

#ifdef IMU_TYPE_BNO055
    // NDBG("ReadIMU " + String(hasNewData));
    imu::Quaternion q = bno.getQuat();
    q.normalize();

    imu::Vector<3> euler = q.toEuler();

    if (isnan(euler.x()) || isnan(euler.y()) || isnan(euler.z()))
    {
        NDBG("Reading is sh*t (nan)");
        return;
    }

    // NDBG("Euler " + String(euler.x()));

    SetParam3(orientation,
              (float)(fmodf(((euler.x() * 180 / PI) + orientationXOffset + 180.0f * 5), 360.0f) - 180.0f),
              (float)(euler.y() * 180 / PI),
              (float)(euler.z() * 180 / PI));

    imu::Vector<3> acc = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    SetParam3(accel, (float)acc.x(), (float)acc.y(), (float)acc.z());

    imu::Vector<3> laccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    SetParam3(linearAccel, (float)laccel.x(), (float)laccel.y(), (float)laccel.z());

    imu::Vector<3> gyr = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    SetParam3(gyro, (float)gyr.x(), (float)gyr.y(), (float)gyr.z());

    // imu::Vector<3> grav = bno.getVector(Adafruit_BNO055::VECTOR_GRAVITY);
    // gravity[0] = grav.x();
    // gravity[1] = grav.y();
    // gravity[2] = grav.z();

    computeThrow();
    computeActivity();
    computeProjectedAngle();

#elif defined IMU_TYPE_M5MPU

    float gyroX, gyroY, gyroZ;
    float accX, accY, accZ;

    mpu.getGyroData(&gyroX, &gyroY, &gyroZ);
    mpu.getAccelData(&accX, &accY, &accZ);
    // mpu.getAhrsData(&pitch, &roll, &yaw);


    double dt = (millis() - lastUpdateTime) / 1000.0;
    lastUpdateTime = millis();

    // Calculate pitch and roll from accelerometer
    float accelPitch = atan2(accY, sqrt(accX * accX + accZ * accZ)) * 180 / PI;
    float accelRoll = atan2(-accX, accZ) * 180 / PI;

    // Integrate gyroscope data
    float gyroPitch = orientation[1] + gyroX * dt;
    float gyroRoll = orientation[2] + gyroY * dt;

    // Complementary filter
    float alpha = 0.98;
    float pitch = alpha * gyroPitch + (1 - alpha) * accelPitch;
    float roll = alpha * gyroRoll + (1 - alpha) * accelRoll;

    SetParam3(orientation, 0, pitch, roll); // no yaw with only accel and gyro
    SetParam3(accel, accX, accY, accZ);
    SetParam3(gyro, gyroX, gyroY, gyroZ);
#endif
    hasNewData = true;
}

void MotionComponent::computeProjectedAngle()
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

void MotionComponent::computeThrow()
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

void MotionComponent::computeActivity()
{
    float maxLinearAccel = max(max(fabsf(linearAccel[0]), fabsf(linearAccel[1])), fabsf(linearAccel[2]));
    maxLinearAccel = (((maxLinearAccel - 0) * (1 - 0)) / (40 - 0)) + 0; // remap to 0..1 range
    maxLinearAccel = min(maxLinearAccel, (float)1.0);

    activity = prevActivity + (maxLinearAccel - prevActivity) * 0.1;
    prevActivity = activity;
}

void MotionComponent::sendCalibrationStatus()
{
#ifdef IMU_TYPE_BNO055
    uint8_t system, gyro, accel, mag;
    system = gyro = accel = mag = 0;
    bno.getCalibration(&system, &gyro, &accel, &mag);

    var data[4];
    data[0] = (float)system;
    data[1] = (float)gyro;
    data[2] = (float)accel;
    data[3] = (float)mag;

    sendEvent(CalibrationStatus, data, 4);
#endif
}

void MotionComponent::setOrientationXOffset(float offset = 0.0f)
{
    orientationXOffset = offset;
}

void MotionComponent::setProjectAngleOffset(float yaw = 0.0f, float angle = 0.0f)
{
    angleOffset = angle;
    xOnCalibration = yaw;
}

bool MotionComponent::handleCommandInternal(const String &command, var *data, int numData)
{
    if (checkCommand(command, "calibrationStatus", numData, 0))
    {
        sendCalibrationStatus();
        return true;
    }

    return false;
}

// Script functions
