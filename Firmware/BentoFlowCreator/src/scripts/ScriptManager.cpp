#include "ScriptManager.h"
#include "../files/FileManager.h"

ScriptManager *ScriptManager::instance = nullptr;

ScriptManager::ScriptManager() : 
    Component("scripts"),
    isRunning(false)
{
    instance = this;
}

void ScriptManager::init()
{
    NDBG("Script Manager init.");
    env = m3_NewEnvironment();
    if (!env)
        NDBG("Script environment error");

    runtime = m3_NewRuntime(env, WASM_STACK_SLOTS, NULL);
    if (!runtime)
        NDBG("Script runtime setup error");
}

void ScriptManager::update()
{
    if(isRunning) m3_CallV(updateFunc);
}

void ScriptManager::launchScript(String path)
{
    if(isRunning)
    {
        NDBG("Script is running, stop before load");
        stop();
    }

    NDBG("Load script " + path + "...");

    File f = FileManager::openFile("/" + path + ".wasm", false); // false is for reading
    if (!f)
    {
        NDBG("Error reading file " + path);
        return;
    }

    long totalBytes = f.size();
    if (totalBytes > SCRIPT_MAX_SIZE)
    {
        NDBG("Script size is more than max size");
        return;
    }

    scriptSize = totalBytes;

    f.read(scriptData, scriptSize);

    NDBG("Read " + String(scriptSize) + " bytes");

    launchWasm();
}

void ScriptManager::launchWasm()
{
    NDBG("Launching wasm...");
    if(isRunning) stop();
    xTaskCreate(&ScriptManager::launchWasmTaskStatic, "wasm3", NATIVE_STACK_SIZE, NULL, 5, NULL);
    NDBG("Wasm task launched...");
}

void ScriptManager::launchWasmTaskStatic(void * v)
{
    instance->launchWasmTask(v);
}

void ScriptManager::launchWasmTask(void *)
{
    M3Result result = m3Err_none;

    IM3Module module;

    try
    {

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

        IM3Function f;
        result = m3_FindFunction(&f, runtime, "_start");
        if (result)
        {
            DBG("FindFunction _start error " + String(result));
            return;
        }

        result = m3_FindFunction(&updateFunc, runtime, "_update");
        if (result)
        {
            DBG("FindFunction _update error " + String(result));
            return;
        }

        result = m3_FindFunction(&stopFunc, runtime, "_stop");
        if (result)
        {
            DBG("FindFunction _stop error " + String(result));
            return;
        }


        DBG("Running WebAssembly...");

        isRunning = true;

        result = m3_CallV(f);

        // Should not arrive here

        // if (result)
        // {
        //     M3ErrorInfo info;
        //     m3_GetErrorInfo(runtime, &info);
        //     String e = "Script Error: " + String(result) + " (" + String(info.message) + ")";
        //     if (info.file && strlen(info.file) && info.line)
        //     {
        //         e += " at " + String(info.file) + ":" + String(info.line);
        //     }
        //     DBG(e);
        // }
    }
    catch (const std::exception &e)
    {
        DBG("Error launching wasm !");
    }

}

M3Result ScriptManager::LinkArduino(IM3Runtime runtime)
{
    IM3Module module = runtime->modules;
    const char *arduino = "arduino";

    m3_LinkRawFunction(module, arduino, "millis", "i()", &m3_arduino_millis);
    m3_LinkRawFunction(module, arduino, "delay", "v(i)", &m3_arduino_delay);
    m3_LinkRawFunction(module, arduino, "pinMode", "v(ii)", &m3_arduino_pinMode);
    m3_LinkRawFunction(module, arduino, "digitalWrite", "v(ii)", &m3_arduino_digitalWrite);

    // Test functions
    m3_LinkRawFunction(module, arduino, "getPinLED", "i()", &m3_arduino_getPinLED);
    m3_LinkRawFunction(module, arduino, "getGreeting", "v(*i)", &m3_arduino_getGreeting);
    m3_LinkRawFunction(module, arduino, "print", "v(*i)", &m3_arduino_print);
    
    m3_LinkRawFunction(module, arduino, "clearLeds", "v()", &m3_clearLeds);
    m3_LinkRawFunction(module, arduino, "fillLeds", "v(i)", &m3_fillLeds);
    m3_LinkRawFunction(module, arduino, "setLed", "v(ii)", &m3_setLed);
    m3_LinkRawFunction(module, arduino, "setLedRGB", "v(iiii)", &m3_setLedRGB);
    m3_LinkRawFunction(module, arduino, "setLedHSV", "v(iiii)", &m3_setLedHSV);

    return m3Err_none;
}

void ScriptManager::stop()
{
    NDBG("Stopping script");
    m3_CallV(stopFunc);
    vTaskDelete( taskHandle );
}

void ScriptManager::shutdown()
{
    stop();
    isRunning = false;
}

bool ScriptManager::handleCommand(String command, var *data, int numData)
{
    if (checkCommand(command, "load", numData, 1))
    {
        launchScript(data[0].stringValue());
        return true;
    }
    if (checkCommand(command, "stop", numData, 0))
    {
        stop();
        return true;
    }
    return false;
}