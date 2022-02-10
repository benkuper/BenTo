SimplexNoise sn;

m3ApiRawFunction(m3_arduino_millis)
{
    m3ApiReturnType(uint32_t);

    m3ApiReturn(millis());
}

m3ApiRawFunction(m3_arduino_delay)
{
    m3ApiGetArg(uint32_t, ms);
    delay(ms);
    m3ApiSuccess();
}

m3ApiRawFunction(m3_printFloat)
{
    m3ApiGetArg(float, val);
    DBG("Print from script : "+String(val));
    m3ApiSuccess();
}

m3ApiRawFunction(m3_printInt)
{
    m3ApiGetArg(uint32_t, val);
    DBG("Print from script : "+String(val));
    m3ApiSuccess();
}

m3ApiRawFunction(m3_randomInt)
{
    m3ApiReturnType(uint32_t);
    m3ApiGetArg(uint32_t, min);
    m3ApiGetArg(uint32_t, max);

    m3ApiReturn((uint32_t) random(min, max+1));
}

m3ApiRawFunction(m3_noise)
{
    m3ApiReturnType(float);
    m3ApiGetArg(float, x);
    m3ApiGetArg(float, y);

    float n = (float) sn.noise(x,y);
    n = (n+1) /2; // convert to value range [0..1]

    m3ApiReturn(n);
}