#include "Script.h"
#define FATAL(func, msg)                  \
    {                                     \
        Serial.print("Fatal: " func " "); \
        Serial.println(msg);              \
        return;                           \
    }

float Script::timeAtLaunch = 0.f;

Script::Script(Component *localComponent) : isRunning(false),
                                            localComponent(localComponent),
                                            runtime(NULL),
                                            // env(NULL),
                                            initFunc(NULL),
                                            updateFunc(NULL),
                                            stopFunc(NULL)
{
    DBG("Script Init here");
}

void Script::init()
{
    DBG("Script init.");
    env = m3_NewEnvironment();

    if (!env)
        DBG("Script environment error");
}

void Script::update()
{
    if (isRunning)
    {
        // TSTART()
        if (updateFunc != NULL)
        {
            m3_CallV(updateFunc);
        }
        // TFINISH("Script ")
    }
}

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
    if (totalBytes > SCRIPT_MAX_SIZE)
    {
        DBG("Script size is more than max size");
        return;
    }
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
    xTaskCreate(&Script::launchWasmTaskStatic, "wasm3", SCRIPT_NATIVE_STACK_SIZE, this, 5, NULL);
    DBG("Wasm task launched");
#else
    launchWasmTask();
#endif
}

#if WASM_ASYNC
void Script::launchWasmTaskStatic(void *v)
{
    ((Script *)v)->launchWasmTask();
}
#endif

void Script::launchWasmTask()
{
    M3Result result = m3Err_none;

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

    IM3Module module;
    result = m3_ParseModule(env, &module, scriptData, scriptSize);
    if (result)
        FATAL("ParseModule", result);

    result = m3_LoadModule(runtime, module);
    if (result)
        FATAL("LoadModule", result);

    result = LinkArduino(runtime);
    if (result)
        FATAL("LinkArduino", result);

    DBG("Finding functions");
    String foundFunc;
    result = m3_FindFunction(&initFunc, runtime, "init");
    if (initFunc != NULL)
        foundFunc += "init";
    result = m3_FindFunction(&updateFunc, runtime, "update");
    if (updateFunc != NULL)
        foundFunc += " / update";

    result = m3_FindFunction(&stopFunc, runtime, "stop");
    if (stopFunc != NULL)
        foundFunc += " / stop";

    result = m3_FindFunction(&setScriptParamFunc, runtime, "setParam");
    if (setScriptParamFunc != NULL)
        foundFunc += " / setParam";

    DBG("Found functions : " + foundFunc);

    isRunning = true;

    timeAtLaunch = millis() / 1000.0f;

    if (initFunc != NULL)
    {
        DBG("Calling init");
        result = m3_CallV(initFunc);
    }

    Serial.println("Running WebAssembly...");

#if WASM_ASYNC
    vTaskDelete(NULL);
#endif
}

M3Result Script::LinkArduino(IM3Runtime runtime)
{
    IM3Module module = runtime->modules;
    const char *util = "util";

    // m3_LinkRawFunction(module, util, "logUTF16", "v(*i)", &m3_logUTF16);
    m3_LinkRawFunction(module, util, "millis", "i()", &m3_arduino_millis);
    m3_LinkRawFunction(module, util, "getTime", "f()", &m3_arduino_getTime);
    m3_LinkRawFunction(module, util, "delay", "v(i)", &m3_arduino_delay);
    m3_LinkRawFunction(module, util, "printFloat", "v(f)", &m3_printFloat);
    m3_LinkRawFunction(module, util, "printInt", "v(i)", &m3_printInt);
    m3_LinkRawFunction(module, util, "print", "v(s)", &m3_printInt);
    m3_LinkRawFunction(module, util, "randomInt", "i(ii)", &m3_randomInt);
    m3_LinkRawFunction(module, util, "noise", "f(ff)", &m3_noise);

    // m3_LinkRawFunction(module, util, "getInt", "i(*i)", &m3_getIntUTF16);
    // m3_LinkRawFunction(module, util, "getFloat", "f(*i)", &m3_getFloatUTF16);
    // m3_LinkRawFunction(module, util, "setInt", "v(*ii)", &m3_setIntUTF16);
    // m3_LinkRawFunction(module, util, "setFloat", "v(*if)", &m3_setFloatUTF16);

    if (localComponent != NULL)
        localComponent->linkScriptFunctions(module, true);

    RootComponent::instance->linkScriptFunctions(module);

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

void Script::setScriptParam(int index, float value)
{
    if (stopFunc != NULL)
        m3_CallV(setScriptParamFunc, index, value);
}

void Script::shutdown()
{
    stop();
    m3_FreeEnvironment(env);
    env = NULL;
}
