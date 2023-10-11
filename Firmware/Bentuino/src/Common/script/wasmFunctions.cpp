SimplexNoise sn;

m3ApiRawFunction(m3_arduino_millis)
{
    m3ApiReturnType(uint32_t);

    m3ApiReturn(millis());
}

m3ApiRawFunction(m3_arduino_getTime)
{
    m3ApiReturnType(float);
    float v = millis() / 1000.0f - Script::timeAtLaunch;
    m3ApiReturn(v);
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
    DBG("Print from script : " + String((int)val));
    m3ApiSuccess();
}

// m3ApiRawFunction(m3_print)
// {
//     m3ApiGetArgMem(const uint8_t *, buf);
//     m3ApiGetArg(uint32_t, len);

//     // printf("api: print %p %d\n", buf, len);
//     DBG("[Script] " + String((const char *)buf));

//     m3ApiSuccess();
// }

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

// m3ApiRawFunction(m3_logUTF16)
// {

//     m3ApiGetArgMem(const uint8_t *, buf);
//     m3ApiGetArg(uint32_t, len);

//     DBG("Log UTF16");
//     DBG("Log UTF16 After Arg");

//     int outlen = len * 4;
//     int inlen = len * 2;
//     byte out[outlen];

//     DBG("Log UTF16 Conversion");
//     UTF16toUTF8(out, &outlen, buf, &inlen);

//     DBG("[Wasm] " + String((char *)out));
//     m3ApiSuccess();
// }

// m3ApiRawFunction(m3_getIntUTF16)
// {
//     m3ApiReturnType(uint32_t);
//     m3ApiGetArgMem(const uint8_t *, buf);
//     m3ApiGetArg(uint32_t, len);

//     if (Parameter *p = getParameterFromM3(buf, len))
//         m3ApiReturn((uint32_t)p->intValue());
//     m3ApiReturn(0);
// }

// m3ApiRawFunction(m3_getFloatUTF16)
// {
//     m3ApiReturnType(float);
//     m3ApiGetArgMem(const uint8_t *, buf);
//     m3ApiGetArg(uint32_t, len);

//     if (Parameter *p = getParameterFromM3(buf, len))
//         m3ApiReturn(p->floatValue());
//     m3ApiReturn(0.0f);
// }

// m3ApiRawFunction(m3_setIntUTF16)
// {
//     m3ApiGetArgMem(const uint8_t *, buf);
//     m3ApiGetArg(uint32_t, len);
//     m3ApiGetArg(uint32_t, val);

//     if (Parameter *p = getParameterFromM3(buf, len))
//         p->set((int)val);

//     m3ApiSuccess();
// }

// m3ApiRawFunction(m3_setFloatUTF16)
// {
//     m3ApiGetArgMem(const uint8_t *, buf);
//     m3ApiGetArg(uint32_t, len);
//     m3ApiGetArg(float, val);

//     if (Parameter *p = getParameterFromM3(buf, len))
//         p->set(val);

//     m3ApiSuccess();
// }

// Parameter *getParameterFromM3(const uint8_t *buf, uint32_t len)
// {
//     int outlen = len * 4;
//     int inlen = len * 2;
//     byte out[outlen];
//     UTF16toUTF8(out, &outlen, buf, &inlen);

//     String target((char *)out);
//     int tcIndex = target.lastIndexOf('.');

//     String tc = target.substring(0, tcIndex);     // component name
//     String pName = target.substring(tcIndex + 1); // parameter name

//     // if (Component *targetComponent = RootComponent::instance->getComponentWithName(tc))
//     // {
//     //     return targetComponent->getParameterWithName(pName);
//     // }

//     return nullptr;
// }