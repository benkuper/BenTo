#include "ScriptManager.h"
#include "../files/FileManager.h"

ScriptManager *ScriptManager::instance = nullptr;

ScriptManager::ScriptManager() : Component("scripts")
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
}

void ScriptManager::launchScript(String path)
{
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
    xTaskCreate(&ScriptManager::launchWasmTask, "wasm3", NATIVE_STACK_SIZE, NULL, 5, NULL);
    NDBG("Wasm task launched...");
}

void ScriptManager::launchWasmTask(void *)
{
    M3Result result = m3Err_none;

    IM3Environment env = instance->env;
    IM3Runtime runtime = instance->runtime;



    IM3Module module;
    try
    {

        #ifdef WASM_MEMORY_LIMIT
            runtime->memoryLimit = WASM_MEMORY_LIMIT;
        #endif
        
        result = m3_ParseModule(env, &module, instance->scriptData, instance->scriptSize);
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
            DBG("FindFunction error " + String(result));
            return;
        }

        DBG("Running WebAssembly...");

        result = m3_CallV(f);

        // Should not arrive here

        if (result)
        {
            M3ErrorInfo info;
            m3_GetErrorInfo(runtime, &info);
            String e = "Script Error: " + String(result) + " (" + String(info.message) + ")";
            if (info.file && strlen(info.file) && info.line)
            {
                e += " at " + String(info.file) + ":" + String(info.line);
            }
            DBG(e);
        }
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

    return m3Err_none;
}

void ScriptManager::shutdown()
{
}

bool ScriptManager::handleCommand(String command, var *data, int numData)
{
    if (checkCommand(command, "load", numData, 1))
    {
        launchScript(data[0].stringValue());
        return true;
    }
    return false;
}