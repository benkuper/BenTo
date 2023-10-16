#pragma once

#define SCRIPT_MAX_SIZE 16000
#define WASM_STACK_SLOTS 2000
#define SCRIPT_NATIVE_STACK_SIZE (16 * 1024)
#define WASM_MEMORY_LIMIT 4096
#define WASM_ASYNC 0

class Component;

class Script
{
public:
    Script(Component *localComponent = NULL);
    ~Script() {}

    bool isRunning;
    unsigned char scriptData[SCRIPT_MAX_SIZE];
    long scriptSize;

    Component *localComponent;

    IM3Runtime runtime;
    IM3Environment env;

    IM3Function initFunc;
    IM3Function updateFunc;
    IM3Function stopFunc;
    IM3Function setScriptParamFunc;

    static float timeAtLaunch;

    void load(const String &name);

    long tstart;
    long tend;

    void init();
    void update();
    void launchWasm();
    void shutdown();
    void stop();

    void setScriptParam(int index, float value);

#if WASM_ASYNC
    static void launchWasmTaskStatic(void *);
#endif
    void launchWasmTask();

    M3Result LinkArduino(IM3Runtime runtime);
};