/*
  ==============================================================================

	WasmEngine.h
	Created: 17 Nov 2025 8:40:20am
	Author:  bkupe

  ==============================================================================
*/

#pragma once

#pragma warning(push)
#pragma warning(disable: 4018 4200)
extern "C"
{
#include "wasm3/wasm3.h"
#include "wasm3/m3_function.h"
}
#pragma warning(pop)

class EmbeddedScriptBlock;

class WasmEngine :
	public EnablingControllableContainer,
	public Thread
{
public:
	WasmEngine(EmbeddedScriptBlock* block);
	~WasmEngine();

	EmbeddedScriptBlock* parentBlock;

	std::vector<uint8_t> wasmData;
	IM3Runtime runtime;
	IM3Module mod;
	IM3Environment env;

	IM3Function initFunc;
	IM3Function updateFunc;
	IM3Function stopFunc;
	IM3Function setScriptParamFunc;
	IM3Function triggerFunctionFunc;

	IntParameter* testPropID;
	IntParameter* testResolution;
	BoolParameter* testButton;
	Point3DParameter* testOrientation;
	FloatParameter* testDistance;

	Array<Colour, CriticalSection> ledColors;

	SimplexNoise noise;
	float timeAtLaunch;

	bool init(File f = File());
	M3Result linkFunctions();
	void stop();

	void run() override;

	void logWasm(String funcName, M3Result r);

	void onContainerParameterChanged(Parameter* p) override;

	//led functioins 

	void point(Colour c, float pos, float radius);


private:
	// Add any private member variables or functions here
};

m3ApiRawFunction(m3_as_abort);

m3ApiRawFunction(m3_arduino_millis);
m3ApiRawFunction(m3_arduino_getTime);
m3ApiRawFunction(m3_arduino_delay);
m3ApiRawFunction(m3_printFloat);
m3ApiRawFunction(m3_printInt);
m3ApiRawFunction(m3_printString);
m3ApiRawFunction(m3_getPropID);
m3ApiRawFunction(m3_sendEvent);
m3ApiRawFunction(m3_sendParamFeedback);

m3ApiRawFunction(m3_clearLeds);
m3ApiRawFunction(m3_dimLeds);
m3ApiRawFunction(m3_fillLeds);
m3ApiRawFunction(m3_fillLedsRGB);
m3ApiRawFunction(m3_fillLedsHSV);
m3ApiRawFunction(m3_setLed);
m3ApiRawFunction(m3_getLed);
m3ApiRawFunction(m3_setLedRGB);
m3ApiRawFunction(m3_setLedHSV);
m3ApiRawFunction(m3_pointRGB);
m3ApiRawFunction(m3_pointHSV);
m3ApiRawFunction(m3_setIR);
m3ApiRawFunction(m3_playVariant);
m3ApiRawFunction(m3_updateLeds);

m3ApiRawFunction(m3_getFXSpeed);
m3ApiRawFunction(m3_getFXIsoSpeed);
m3ApiRawFunction(m3_getFXStaticOffset);
m3ApiRawFunction(m3_getFXFlipped);
m3ApiRawFunction(m3_setFXSpeed);
m3ApiRawFunction(m3_setFXIsoSpeed);
m3ApiRawFunction(m3_setFXIsoAxis);
m3ApiRawFunction(m3_setFXStaticOffset);
m3ApiRawFunction(m3_resetFX);

m3ApiRawFunction(m3_getOrientation);
m3ApiRawFunction(m3_getYaw);
m3ApiRawFunction(m3_getPitch);
m3ApiRawFunction(m3_getRoll);
m3ApiRawFunction(m3_getThrowState);
m3ApiRawFunction(m3_getProjectedAngle);
m3ApiRawFunction(m3_setProjectedAngleOffset);
m3ApiRawFunction(m3_calibrateIMU);
m3ApiRawFunction(m3_setIMUEnabled);
m3ApiRawFunction(m3_getActivity);
m3ApiRawFunction(m3_getSpin);

m3ApiRawFunction(m3_getButtonState);


m3ApiRawFunction(m3_setMicEnabled);
m3ApiRawFunction(m3_getMicLevel);

m3ApiRawFunction(m3_setBatterySendEnabled);

m3ApiRawFunction(m3_getDistance);

m3ApiRawFunction(m3_randomInt);
m3ApiRawFunction(m3_noise);