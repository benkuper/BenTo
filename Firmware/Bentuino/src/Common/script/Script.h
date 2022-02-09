#pragma once

#define SCRIPT_MAX_SIZE 32000
#define WASM_STACK_SLOTS 2048
#define NATIVE_STACK_SIZE (32 * 1024)
#define WASM_MEMORY_LIMIT 4096*4
#define WASM_ASYNC 0

class Script
{
public:
    Script();
    ~Script();

    bool isRunning;
    unsigned char scriptData[SCRIPT_MAX_SIZE];
    long scriptSize;

    IM3Runtime runtime;
    IM3Environment env;

    IM3Function initFunc;
    IM3Function updateFunc;
    IM3Function stopFunc;

    void load(const String &name);
    
    long tstart;
    long tend;

    void init();
    //void update();

    void launchWasm();
    void shutdown();

    void stop();

 #if WASM_ASYNC
    static void launchWasmTaskStatic(void *);
#endif
   void launchWasmTask();

    M3Result LinkArduino(IM3Runtime runtime);
};