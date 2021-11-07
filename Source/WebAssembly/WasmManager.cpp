/*
  ==============================================================================

    WasmManager.cpp
    Created: 7 Nov 2021 11:15:18am
    Author:  bkupe

  ==============================================================================
*/

#include "WasmManager.h"

juce_ImplementSingleton(WasmManager)

WasmManager::WasmManager() :
    BaseManager("WASM Scripts")
{

}

WasmManager::~WasmManager()
{
}
