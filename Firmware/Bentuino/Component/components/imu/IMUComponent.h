#pragma once

DeclareComponent(IMU, "imu", )

    Adafruit_BNO055 bno;
Parameter<bool> *isConnected;

void initInternal() override
{
    isConnected = addParameter<bool>("connected", false);

    // Wire.begin(SDA_PIN, SCL_PIN);

    if (!bno.begin())
    {
        NDBG("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        isConnected->set(false);
        return;
    }

    bno.setMode(Adafruit_BNO055::OPERATION_MODE_CONFIG);
    // bno.setAxisRemap(IMU_REMAP_CONFIG);
    // bno.setAxisSign(IMU_REMAP_SIGN);
    bno.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);

    // bno.setExtCrystalUse(true);
    bno.enterNormalMode();

    NDBG("IMU is connected");
    isConnected->set(true);
}

void updateInternal()
{
}

void clearInternal()
{
}

EndDeclareComponent