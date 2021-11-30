#pragma once

DeclareComponent(IMU, "imu", )

    Adafruit_BNO055 bno;
Parameter *isConnected;
bool initInternal() override
{
    isConnected = addParameter("connected", false);

    int sdaPin = GetIntConfig("sdaPin");
    int sclPin = GetIntConfig("sclPin");

    if (sdaPin == 0)
        sdaPin = 23;
    if (sclPin == 0)
        sclPin = 22;

    Wire.begin(sdaPin, sclPin);

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