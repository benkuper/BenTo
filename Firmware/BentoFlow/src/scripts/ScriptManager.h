
#pragma once
#include "../common/Common.h"

#ifdef HAS_SCRIPTS

#include "wasmFunctions.h"

#define SCRIPT_MAX_SIZE 32000

#define WASM_STACK_SLOTS 2048
#define NATIVE_STACK_SIZE (32 * 1024)

#define WASM_MEMORY_LIMIT 4096*4

#define WASM_ASYNC 0


class ScriptManager : public Component
{
public:
    ScriptManager();
    ~ScriptManager() {}

    static ScriptManager *instance;
    unsigned char scriptData[SCRIPT_MAX_SIZE];
    long scriptSize;

    bool isRunning;
    IM3Runtime runtime;
    IM3Environment env;

    IM3Function initFunc;
    IM3Function updateFunc;
    IM3Function stopFunc;
    
    long tstart;
    long tend;

    int blendMode; //to do. 0 = add, 1 = multiply

    float timeAtLaunch = 0;

    void init();
    void update();

    void launchScript(String name);

    void launchWasm();
    void shutdown();

    void stop();

 #if WASM_ASYNC
    static void launchWasmTaskStatic(void *);
#endif
   void launchWasmTask();

    static M3Result LinkArduino(IM3Runtime runtime);
    bool handleCommand(String command, var *data, int numData) override;
};

#endif