#pragma once

DeclareComponent(IMU, "imu", )

Adafruit_BNO055 bno;
Parameter *isConnected;

bool initInternal() override
{
    isConnected = addParameter("connected", false);

    // Wire.begin(SDA_PIN, SCL_PIN);

    if (!bno.begin())
    {
        NDBG("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        return false;
    }

    bno.setMode(Adafruit_BNO055::OPERATION_MODE_CONFIG);
    // bno.setAxisRemap(IMU_REMAP_CONFIG);
    // bno.setAxisSign(IMU_REMAP_SIGN);
    bno.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);

    // bno.setExtCrystalUse(true);
    bno.enterNormalMode();

    return true;
}

void updateInternal()
{
}

void clearInternal()
{
}

EndDeclareComponent