Script::Script(Component *localComponent) :
                                            isRunning(false),
                                            localComponent(localComponent),
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

void Script::load(const String &path)
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
    IM3Module module = runtime->modules;
    const char *util = "util";

    m3_LinkRawFunction(module, util, "millis", "i()", &m3_arduino_millis);
    m3_LinkRawFunction(module, util, "delay", "v(i)", &m3_arduino_delay);
    m3_LinkRawFunction(module, util, "log", "v(f)", &m3_logUTF16);
    m3_LinkRawFunction(module, util, "printFloat", "v(f)", &m3_printFloat);
    m3_LinkRawFunction(module, util, "printInt", "v(i)", &m3_printInt);
    m3_LinkRawFunction(module, util, "randomInt", "i(ii)", &m3_randomInt);
    m3_LinkRawFunction(module, util, "noise", "f(ff)", &m3_noise);

    if(localComponent != NULL) localComponent->linkScriptFunctions(this, true);
    RootComponent::instance->linkScriptFunctions(this);

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