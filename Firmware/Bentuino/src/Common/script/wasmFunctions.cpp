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
    DBG("Print from script : " + String(val));
    m3ApiSuccess();
}

m3ApiRawFunction(m3_printInt)
{
    m3ApiGetArg(uint32_t, val);
    DBG("Print from script : " + String(val));
    m3ApiSuccess();
}

m3ApiRawFunction(m3_randomInt)
{
    m3ApiReturnType(uint32_t);
    m3ApiGetArg(uint32_t, min);
    m3ApiGetArg(uint32_t, max);

    m3ApiReturn((uint32_t)random(min, max + 1));
}

m3ApiRawFunction(m3_noise)
{
    m3ApiReturnType(float);
    m3ApiGetArg(float, x);
    m3ApiGetArg(float, y);

    float n = (float)sn.noise(x, y);
    n = (n + 1) / 2; // convert to value range [0..1]

    m3ApiReturn(n);
}

m3ApiRawFunction(m3_logUTF16)
{
    m3ApiGetArgMem(const uint8_t *, buf);
    m3ApiGetArg(uint32_t, len);

    int outlen = len * 4;
    int inlen = len * 2;
    byte out[outlen];
    UTF16toUTF8(out, &outlen, buf, &inlen);

    DBG("[Wasm] " + String((char *)out));
    m3ApiSuccess();
}

m3ApiRawFunction(m3_setInt)
{
    m3ApiGetArgMem(const uint8_t *, buf);
    m3ApiGetArg(uint32_t, len);
    m3ApiGetArg(uint32_t, val);

    handleM3SetVal(buf, len, var((int)val));

    m3ApiSuccess();
}

m3ApiRawFunction(m3_setFloat)
{
    m3ApiGetArgMem(const uint8_t *, buf);
    m3ApiGetArg(uint32_t, len);
    m3ApiGetArg(float, val);

    handleM3SetVal(buf, len, var(val));

    m3ApiSuccess();
}

void handleM3SetVal(const uint8_t *buf, uint32_t len, var val)
{
    int outlen = len * 4;
    int inlen = len * 2;
    byte out[outlen];
    UTF16toUTF8(out, &outlen, buf, &inlen);

    String target((char *)out);
    int tcIndex = target.lastIndexOf('.');

    String tc = target.substring(0, tcIndex);   // component name
    String cmd = target.substring(tcIndex + 1); // parameter name

    if (Component *targetComponent = RootComponent::instance->getComponentWithName(tc))
    {
        var data[1]{val};
        targetComponent->handleCommand(cmd, data, 1);
    }
}