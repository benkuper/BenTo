Script::Script() : isRunning(false),
                   runtime(NULL),
                   initFunc(NULL),
                   updateFunc(NULL),
                   stopFunc(NULL)
{

}

void Script::init()
{
    DBG("Script Manager init.");
    env = m3_NewEnvironment();

    if (!env)
        DBG("Script environment error");
}

// void Script::update()
// {
//     if (isRunning)
//     {
//         // TSTART()
//         if (updateFunc != NULL)
//         {
//             m3_CallV(updateFunc);
//         }
//         // TFINISH("Script ")
//     }
// }

void Script::load(const String& path)
{
    if (isRunning)
    {
        DBG("Script is running, stop before load");
        stop();
    }

    DBG("Load script " + path + "...");

    File f = FilesComponent::instance->openFile("/scripts/" + path + ".wasm", false); // false is for reading
    if (!f)
    {
        DBG("Error reading file " + path);
        return;
    }

    long totalBytes = f.size();
#ifdef SCRIPT_MAX_SIZE
    if (totalBytes > SCRIPT_MAX_SIZE)
    {
        DBG("Script size is more than max size");
        return;
    }
#endif
    scriptSize = totalBytes;

    f.read(scriptData, scriptSize);

    DBG("Script read " + String(scriptSize) + " bytes");
    launchWasm();
}

void Script::launchWasm()
{
    DBG("Script Launching wasm...");
    if (isRunning)
        stop();

#if WASM_ASYNC
    xTaskCreate(&Script::launchWasmTaskStatic, "wasm3", NATIVE_STACK_SIZE, NULL, 5, NULL);
#else
    launchWasmTask();
#endif

    DBG("Wasm task launched...");
}

#if WASM_ASYNC
void Script::launchWasmTaskStatic(void *v)
{
    instance->launchWasmTask();
}
#endif

void Script::launchWasmTask()
{
    M3Result result = m3Err_none;
    IM3Module module;

    if (runtime != NULL)
    {
        DBG("New run free Runtime");
        m3_FreeRuntime(runtime);
    }

    runtime = m3_NewRuntime(env, WASM_STACK_SLOTS, NULL);
    if (!runtime)
    {
        DBG("Script runtime setup error");
        return;
    }

#ifdef WASM_MEMORY_LIMIT
    runtime->memoryLimit = WASM_MEMORY_LIMIT;
#endif

    result = m3_ParseModule(env, &module, scriptData, scriptSize);
    if (result)
    {
        DBG("ParseModule error " + String(result));
        return;
    }

    result = m3_LoadModule(runtime, module);
    if (result)
    {
        DBG("LoadModule error " + String(result));
        return;
    }

    result = LinkArduino(runtime);
    if (result)
    {
        DBG("LinkArduino error " + String(result));
        return;
    }

    String foundFunc;

    result = m3_FindFunction(&initFunc, runtime, "init");
    if (initFunc != NULL)
        foundFunc += "init";
    // if (result)
    // {
    //     DBG("FindFunction init error " + String(result));
    //     return;
    // }

    result = m3_FindFunction(&updateFunc, runtime, "update");
    if (updateFunc != NULL)
        foundFunc += " / update";

    // if (result)
    // {
    //     DBG("FindFunction update error " + String(result));
    //     return;
    // }

    result = m3_FindFunction(&stopFunc, runtime, "stop");
    if (stopFunc != NULL)
        foundFunc += " / stop";

    // if (result)
    // {
    //     DBG("FindFunction stop error " + String(result));
    // }

    DBG("Found functions : " + foundFunc);

    isRunning = true;

    if (initFunc != NULL)
        result = m3_CallV(initFunc);

#if WASM_ASYNC
    vTaskDelete(NULL);
#endif
}

M3Result Script::LinkArduino(IM3Runtime runtime)
{
    //IM3Module module = runtime->modules;
    RootComponent::instance->linkScriptFunctions(this);

    // m3_LinkRawFunction(module, arduino, "millis", "i()", &m3_arduino_millis);
    // m3_LinkRawFunction(module, arduino, "delay", "v(i)", &m3_arduino_delay);
    // m3_LinkRawFunction(module, arduino, "printFloat", "v(f)", &m3_printFloat);
    // m3_LinkRawFunction(module, arduino, "printInt", "v(i)", &m3_printInt);

    // m3_LinkRawFunction(module, arduino, "clearLeds", "v()", &m3_clearLeds);
    // m3_LinkRawFunction(module, arduino, "fillLeds", "v(i)", &m3_fillLeds);
    // m3_LinkRawFunction(module, arduino, "fillLedsRGB", "v(iii)", &m3_fillLedsRGB);
    // m3_LinkRawFunction(module, arduino, "fillLedsHSV", "v(iii)", &m3_fillLedsHSV);
    // m3_LinkRawFunction(module, arduino, "setLed", "v(ii)", &m3_setLed);
    // m3_LinkRawFunction(module, arduino, "getLed", "i(i)", &m3_getLed);

    // m3_LinkRawFunction(module, arduino, "setLedRGB", "v(iiii)", &m3_setLedRGB);
    // m3_LinkRawFunction(module, arduino, "setLedHSV", "v(iiii)", &m3_setLedHSV);
    // m3_LinkRawFunction(module, arduino, "pointRGB", "v(ffiii)", &m3_pointRGB);
    // m3_LinkRawFunction(module, arduino, "pointHSV", "v(ffiii)", &m3_pointHSV);
    // m3_LinkRawFunction(module, arduino, "getOrientation", "f(i)", &m3_getOrientation);
    // m3_LinkRawFunction(module, arduino, "getYaw", "f()", &m3_getYaw);
    // m3_LinkRawFunction(module, arduino, "getPitch", "f()", &m3_getPitch);
    // m3_LinkRawFunction(module, arduino, "getRoll", "f()", &m3_getRoll);
    // m3_LinkRawFunction(module, arduino, "getProjectedAngle", "f()", &m3_getProjectedAngle);
    // m3_LinkRawFunction(module, arduino, "setProjectedAngleOffset", "v(ff)", &m3_setProjectedAngleOffset);
    // m3_LinkRawFunction(module, arduino, "setIMUEnabled", "v(i)", &m3_setIMUEnabled);
    // m3_LinkRawFunction(module, arduino, "getThrowState", "i()", &m3_getThrowState);
    // m3_LinkRawFunction(module, arduino, "updateLeds", "v()", &m3_updateLeds);
    // m3_LinkRawFunction(module, arduino, "getButtonState", "i(i)", &m3_getButtonState);
    // m3_LinkRawFunction(module, arduino, "getActivity", "f()", &m3_getActivity);

    // m3_LinkRawFunction(module, arduino, "setBatterySendEnabled", "v(i)", &m3_setBatterySendEnabled);
    // m3_LinkRawFunction(module, arduino, "randomInt", "i(ii)", &m3_randomInt);
    // m3_LinkRawFunction(module, arduino, "noise", "f(ff)", &m3_noise);

    return m3Err_none;
}

void Script::stop()
{
    if (isRunning)
    {
        DBG("Stopping script");

        if (stopFunc != NULL)
            m3_CallV(stopFunc);

        isRunning = false;
        m3_FreeRuntime(runtime);
        runtime = NULL;
    }
    else
    {
        DBG("Not stopping script, because non was running");
    }
}

void Script::shutdown()
{
    stop();
    m3_FreeEnvironment(env);
    env = NULL;
}

// bool Script::handleCommand(String command, var *data, int numData)
// {
//     if (checkCommand(command, "load", numData, 1))
//     {
//         launchScript(data[0].stringValue());
//         return true;
//     }
//     if (checkCommand(command, "stop", numData, 0))
//     {
//         stop();
//         return true;
//     }

//     return false;
// }