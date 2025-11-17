/*
  ==============================================================================

	WasmEngine.cpp
	Created: 17 Nov 2025 8:40:20am
	Author:  bkupe

  ==============================================================================
*/

#include "LightBlock/LightBlockIncludes.h"

WasmEngine::WasmEngine(EmbeddedScriptBlock* block) :
	EnablingControllableContainer("Wasm Engine", true),
	Thread("WasmEngineThread"),
	runtime(nullptr),
	env(nullptr),
	timeAtLaunch(0),
	parentBlock(block)
{
	testPropID = addIntParameter("Test Prop ID", "ID of the prop to test with", 0, 0, 1000);
	testResolution = addIntParameter("Test Resolution", "Resolution for testing", 32, 1, 4096);
	testButton = addBoolParameter("Test Button", "A test button", false);
	testOrientation = addPoint3DParameter("Test Orientation", "A test 3D orientation");
	testOrientation->setBounds(-180.0f, -90.0f, -180.0f, 180.0f, 90.0f, 180.0f);
	testDistance = addFloatParameter("Test Distance", "A test distance", 0.0f, 0.0f, 1.0f);

	ledColors.resize(testResolution->intValue());
}

WasmEngine::~WasmEngine()
{
	stop();
}

bool WasmEngine::init(File f)
{
	stop();

	M3Result result = m3Err_none;

	env = m3_NewEnvironment();
	runtime = m3_NewRuntime(env, 16 * 64, this);

	// Load wasm file
	FileInputStream wasmFileStream(f);
	if (!wasmFileStream.openedOk()) {
		LOGERROR("Failed to open WASM file: " + f.getFullPathName());
		return false;
	}

	wasmData.resize((size_t)wasmFileStream.getTotalLength());
	wasmFileStream.read(wasmData.data(), (int)wasmData.size());

	result = m3_ParseModule(env, &mod, wasmData.data(), wasmData.size());
	if (result)
	{
		LOGERROR("Parse error");
		return false;
	}


	result = m3_LoadModule(runtime, mod);
	if (result)
		LOGERROR("LoadModule error");

	linkFunctions();

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

	result = m3_FindFunction(&triggerFunctionFunc, runtime, "triggerFunction");
	if (triggerFunctionFunc != NULL)
		foundFunc += " / triggerFunction";

	LOG("Found functions : " + foundFunc);


	timeAtLaunch = Time::getMillisecondCounterHiRes() / 1000.0f;

	LOG("Running WebAssembly...");

	if (initFunc != NULL)
	{
		LOG("[script] Calling init");
		result = m3_CallV(initFunc);
		logWasm("init", result);
	}
	else
	{
		LOG("[script] No init function found");
	}
	startThread();

	return true;
}

M3Result WasmEngine::linkFunctions()
{
	M3Result r = m3Err_none;
	m3_LinkRawFunction(mod, "env", "abort", "v(iiii)", &m3_as_abort);

	const char* arduino = "arduino";

	m3_LinkRawFunction(mod, arduino, "millis", "i()", &m3_arduino_millis);
	m3_LinkRawFunction(mod, arduino, "getTime", "f()", &m3_arduino_getTime);
	m3_LinkRawFunction(mod, arduino, "delay", "v(i)", &m3_arduino_delay);
	m3_LinkRawFunction(mod, arduino, "printFloat", "v(f)", &m3_printFloat);
	m3_LinkRawFunction(mod, arduino, "printInt", "v(i)", &m3_printInt);
	m3_LinkRawFunction(mod, arduino, "printString", "v(ii)", &m3_printString);
	m3_LinkRawFunction(mod, arduino, "sendEvent", "v(i)", &m3_sendEvent);
	m3_LinkRawFunction(mod, arduino, "sendParamFeedback", "v(if)", &m3_sendParamFeedback);
	m3_LinkRawFunction(mod, arduino, "getPropID", "i()", &m3_getPropID);

	m3_LinkRawFunction(mod, arduino, "clearLeds", "v()", &m3_clearLeds);
	m3_LinkRawFunction(mod, arduino, "dimLeds", "v(f)", &m3_dimLeds);
	m3_LinkRawFunction(mod, arduino, "fillLeds", "v(i)", &m3_fillLeds);
	m3_LinkRawFunction(mod, arduino, "fillLedsRGB", "v(iii)", &m3_fillLedsRGB);
	m3_LinkRawFunction(mod, arduino, "fillLedsHSV", "v(fff)", &m3_fillLedsHSV);
	m3_LinkRawFunction(mod, arduino, "setLed", "v(ii)", &m3_setLed);
	m3_LinkRawFunction(mod, arduino, "getLed", "i(i)", &m3_getLed);

	m3_LinkRawFunction(mod, arduino, "setLedRGB", "v(iiii)", &m3_setLedRGB);
	m3_LinkRawFunction(mod, arduino, "setLedHSV", "v(ifff)", &m3_setLedHSV);
	m3_LinkRawFunction(mod, arduino, "pointRGB", "v(ffiii)", &m3_pointRGB);
	m3_LinkRawFunction(mod, arduino, "pointHSV", "v(fffff)", &m3_pointHSV);
	m3_LinkRawFunction(mod, arduino, "setIR", "v(f)", &m3_setIR);
	m3_LinkRawFunction(mod, arduino, "playVariant", "v(i)", &m3_playVariant);
	m3_LinkRawFunction(mod, arduino, "updateLeds", "v()", &m3_updateLeds);

	m3_LinkRawFunction(mod, arduino, "getFXSpeed", "f()", &m3_getFXSpeed);
	m3_LinkRawFunction(mod, arduino, "getFXIsoSpeed", "f()", &m3_getFXIsoSpeed);
	m3_LinkRawFunction(mod, arduino, "getFXStaticOffset", "f()", &m3_getFXStaticOffset);
	m3_LinkRawFunction(mod, arduino, "getFXFlipped", "i()", &m3_getFXFlipped);
	m3_LinkRawFunction(mod, arduino, "setFXSpeed", "v(f)", &m3_setFXSpeed);
	m3_LinkRawFunction(mod, arduino, "setFXIsoSpeed", "v(f)", &m3_setFXIsoSpeed);
	m3_LinkRawFunction(mod, arduino, "setFXIsoAxis", "v(i)", &m3_setFXIsoAxis);
	m3_LinkRawFunction(mod, arduino, "setFXStaticOffset", "v(f)", &m3_setFXStaticOffset);
	m3_LinkRawFunction(mod, arduino, "resetFX", "v()", &m3_resetFX);

	m3_LinkRawFunction(mod, arduino, "getOrientation", "f(i)", &m3_getOrientation);
	m3_LinkRawFunction(mod, arduino, "getYaw", "f()", &m3_getYaw);
	m3_LinkRawFunction(mod, arduino, "getPitch", "f()", &m3_getPitch);
	m3_LinkRawFunction(mod, arduino, "getRoll", "f()", &m3_getRoll);
	m3_LinkRawFunction(mod, arduino, "getProjectedAngle", "f()", &m3_getProjectedAngle);
	m3_LinkRawFunction(mod, arduino, "setProjectedAngleOffset", "v(ff)", &m3_setProjectedAngleOffset);
	m3_LinkRawFunction(mod, arduino, "setIMUEnabled", "v(i)", &m3_setIMUEnabled);
	m3_LinkRawFunction(mod, arduino, "calibrateIMU", "v()", &m3_calibrateIMU);
	m3_LinkRawFunction(mod, arduino, "getThrowState", "i()", &m3_getThrowState);
	m3_LinkRawFunction(mod, arduino, "getActivity", "f()", &m3_getActivity);
	m3_LinkRawFunction(mod, arduino, "getSpin", "f()", &m3_getSpin);

	m3_LinkRawFunction(mod, arduino, "getButtonState", "i(i)", &m3_getButtonState);

	m3_LinkRawFunction(mod, arduino, "setMicEnabled", "v(i)", &m3_setMicEnabled);
	m3_LinkRawFunction(mod, arduino, "getMicLevel", "f()", &m3_getMicLevel);

	m3_LinkRawFunction(mod, arduino, "setBatterySendEnabled", "v(i)", &m3_setBatterySendEnabled);

	m3_LinkRawFunction(mod, arduino, "getDistance", "f(i)", &m3_getDistance);

	m3_LinkRawFunction(mod, arduino, "randomInt", "i(ii)", &m3_randomInt);
	m3_LinkRawFunction(mod, arduino, "noise", "f(ff)", &m3_noise);


	return m3Err_none;
}

void WasmEngine::stop()
{
	stopThread(100);

	if (runtime != NULL)
		m3_FreeRuntime(runtime);

	if (env != NULL)
		m3_FreeEnvironment(env);

	runtime = nullptr;
	env = nullptr;
}

void WasmEngine::run()
{
	while (!threadShouldExit())
	{
		try
		{
			if (updateFunc != NULL)
			{
				GenericScopedLock<CriticalSection> lock(ledColors.getLock());
				M3Result result = m3_CallV(updateFunc);
				logWasm("update", result);
			}
		}
		catch (...)
		{
			LOG("[script] Error occurred");
			return;
		}
		wait(10);
	}

	if (stopFunc != NULL)
	{
		LOG("[script] Calling stop");
		M3Result result = m3_CallV(stopFunc);
		logWasm("stop", result);
	}
}

void WasmEngine::logWasm(String funcName, M3Result r)
{
	if (!r)
		return;
	M3ErrorInfo info;
	m3_GetErrorInfo(runtime, &info);
	LOG(String("[m3] Calling ") + funcName + " failed: " + info.message);
	if (info.file && info.function)
		LOG(String(" @ ") + String(info.file) + " :: " + String(info.function->names[0]) + " (line " + String(info.line) + ")");
}

void WasmEngine::onContainerParameterChanged(Parameter* p)
{
	EnablingControllableContainer::onContainerParameterChanged(p);

	if (p == testResolution)
	{
		ledColors.resize(testResolution->intValue());
	}
	else if (p == enabled)
	{
		if(!enabled->boolValue()) stop();
	}
}

void WasmEngine::point(Colour c, float pos, float radius)
{
	GenericScopedLock<CriticalSection> lock(ledColors.getLock());
	int numLeds = testResolution->intValue();
	for (int i = 0; i < numLeds; i++)
	{
		float rel = i * 1.0f / jmax(numLeds - 1, 1);
		float fac = jmax(1 - (abs((float)(pos - rel)) / radius), 0.f);
		Colour tc = c.withMultipliedBrightness(fac);
		//add to existing color
		Colour ec = ledColors[i];
		uint8_t nr = jmin(tc.getRed() + ec.getRed(), 255);
		uint8_t ng = jmin(tc.getGreen() + ec.getGreen(), 255);
		uint8_t nb = jmin(tc.getBlue() + ec.getBlue(), 255);
		ledColors.set(i, Colour::fromRGB(nr, ng, nb));
	}
}

m3ApiRawFunction(m3_as_abort)
{
	// We don’t need the strings; just consume the args to keep the stack sane
	m3ApiGetArg(uint32_t, msg);
	m3ApiGetArg(uint32_t, file);
	m3ApiGetArg(uint32_t, line);
	m3ApiGetArg(uint32_t, col);
	// Optional: log something if desired
	// Serial.LOGERROR("AS abort: line=%u col=%u\n", line, col);

	LOG("Script abort called from WebAssembly : line " + String(line) + " col " + String(col));
	m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_millis)
{
	m3ApiReturnType(uint32_t);

	m3ApiReturn(Time::getMillisecondCounter());
}

m3ApiRawFunction(m3_arduino_getTime)
{
	m3ApiReturnType(float);
	WasmEngine* engine = (WasmEngine*)m3_GetUserData(runtime);
	float v = Time::getMillisecondCounter() / 1000.0f - engine->timeAtLaunch;
	m3ApiReturn(v);
}

m3ApiRawFunction(m3_arduino_delay)
{
	m3ApiGetArg(uint32_t, ms);
	Thread::sleep(ms);
	m3ApiSuccess();
}

m3ApiRawFunction(m3_printFloat)
{
	m3ApiGetArg(float, val);
	LOG("Print from script : " + String(val));
	m3ApiSuccess();
}

m3ApiRawFunction(m3_printInt)
{
	m3ApiGetArg(uint32_t, val);
	LOG("Print from script : " + String(val));
	m3ApiSuccess();
}

m3ApiRawFunction(m3_printString)
{
	m3ApiGetArgMem(const uint8_t*, buf); // pointer into Wasm memory
	m3ApiGetArg(uint32_t, len);           // length in bytes

	LOG("Print from script : " << String((const char*)buf, len));

	m3ApiSuccess();
}

m3ApiRawFunction(m3_sendEvent)
{
	m3ApiGetArg(uint32_t, eventId);
	LOG("Sending script event : " << eventId);
	m3ApiSuccess();
}

m3ApiRawFunction(m3_sendParamFeedback)
{
	m3ApiGetArg(uint32_t, paramId);
	m3ApiGetArg(float, value);
	LOG("Sending script param feedback : " << paramId << " = " << value);
	m3ApiSuccess();
}

m3ApiRawFunction(m3_getPropID)
{
	m3ApiReturnType(uint32_t);
	WasmEngine* engine = (WasmEngine*)m3_GetUserData(runtime);
	uint32_t deviceId = engine->testPropID->intValue();
	m3ApiReturn(deviceId);
}

m3ApiRawFunction(m3_clearLeds)
{
	// RootComponent::instance->strips.items[0]->bakeLayer(LedManager::Mode::Stream);
	WasmEngine* engine = (WasmEngine*)m3_GetUserData(runtime);
	engine->ledColors.fill(Colours::black);

	m3ApiSuccess();
}

m3ApiRawFunction(m3_dimLeds)
{
	m3ApiGetArg(float, v);
	// RootComponent::instance->strips.items[0]->scriptLayer.dimLayer(v);

	m3ApiSuccess();
}

m3ApiRawFunction(m3_fillLeds)
{
	m3ApiGetArg(uint32_t, color);
	// RootComponent::instance->strips.items[0]->bakeLayer(LedManager::Mode::Stream);
	WasmEngine* engine = (WasmEngine*)m3_GetUserData(runtime);
	engine->ledColors.fill(Colour::fromRGBA((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, (color >> 24) & 0xFF));
	m3ApiSuccess();
}

m3ApiRawFunction(m3_fillLedsRGB)
{
	m3ApiGetArg(uint32_t, r);
	m3ApiGetArg(uint32_t, g);
	m3ApiGetArg(uint32_t, b);
	WasmEngine* engine = (WasmEngine*)m3_GetUserData(runtime);
	engine->ledColors.fill(Colour::fromRGB(r, g, b));

	m3ApiSuccess();
}

m3ApiRawFunction(m3_fillLedsHSV)
{
	m3ApiGetArg(float, h);
	m3ApiGetArg(float, s);
	m3ApiGetArg(float, v);
	WasmEngine* engine = (WasmEngine*)m3_GetUserData(runtime);
	engine->ledColors.fill(Colour::fromHSV(h, s, v, 1.f));

	m3ApiSuccess();
}

m3ApiRawFunction(m3_setLed)
{
	m3ApiGetArg(uint32_t, index);
	m3ApiGetArg(uint32_t, color);
	WasmEngine* engine = (WasmEngine*)m3_GetUserData(runtime);

	if (index >= 0 && index < engine->testResolution->intValue())
	{
		Colour c = Colour::fromRGBA((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, (color >> 24) & 0xFF);
		engine->ledColors.set(index, c);
	}

	m3ApiSuccess();
}

m3ApiRawFunction(m3_getLed)
{
	m3ApiReturnType(uint32_t)
		m3ApiGetArg(uint32_t, index);

	WasmEngine* engine = (WasmEngine*)m3_GetUserData(runtime);
	if (index >= 0 && index < engine->testResolution->intValue())
	{
		Colour c = engine->ledColors[index];
		uint32_t val = c.getAlpha() << 24 | c.getRed() << 16 | c.getGreen() << 8 | c.getBlue();
		m3ApiReturn(val);
	}

	m3ApiReturn(0)
}

m3ApiRawFunction(m3_setLedRGB)
{
	m3ApiGetArg(uint32_t, index);
	m3ApiGetArg(uint32_t, r);
	m3ApiGetArg(uint32_t, g);
	m3ApiGetArg(uint32_t, b);

	WasmEngine* engine = (WasmEngine*)m3_GetUserData(runtime);
	if (index >= 0 && index < engine->testResolution->intValue())
	{
		engine->ledColors.set(index, Colour::fromRGB(r, g, b));
	}
	m3ApiSuccess();
}

m3ApiRawFunction(m3_setLedHSV)
{
	m3ApiGetArg(uint32_t, index);
	m3ApiGetArg(float, h);
	m3ApiGetArg(float, s);
	m3ApiGetArg(float, v);

	WasmEngine* engine = (WasmEngine*)m3_GetUserData(runtime);
	if (index >= 0 && index < engine->testResolution->intValue())
	{
		engine->ledColors.set(index, Colour::fromHSV(h, s, v, 1.f));
	}

	m3ApiSuccess();
}

m3ApiRawFunction(m3_pointRGB)
{
	m3ApiGetArg(float, pos);
	m3ApiGetArg(float, radius);
	m3ApiGetArg(uint32_t, r);
	m3ApiGetArg(uint32_t, g);
	m3ApiGetArg(uint32_t, b);


	//fade point relative to resolution

	if (radius <= 0)
		m3ApiSuccess();

	if (r == 0 && g == 0 && b == 0)
		m3ApiSuccess();

	Colour c = Colour::fromRGB(r, g, b);

	WasmEngine* engine = (WasmEngine*)m3_GetUserData(runtime);
	engine->point(c, pos, radius);

	m3ApiSuccess();
}

m3ApiRawFunction(m3_pointHSV)
{
	m3ApiGetArg(float, pos);
	m3ApiGetArg(float, radius);
	m3ApiGetArg(float, h);
	m3ApiGetArg(float, s);
	m3ApiGetArg(float, v);

	if (radius <= 0 || v == 0)
		m3ApiSuccess();

	Colour c = Colour::fromHSV(h, s, v, 1.f);

	WasmEngine* engine = (WasmEngine*)m3_GetUserData(runtime);
	engine->point(c, pos, radius);

	m3ApiSuccess();
}

m3ApiRawFunction(m3_setIR)
{
	m3ApiGetArg(float, v);
	LOG("Set IR level: " + String(v));
	// RootComponent::instance->strips.items[0]->bakeLayer.setBrightness(v);

	m3ApiSuccess();
}

m3ApiRawFunction(m3_updateLeds)
{
	//RootComponent::instance->strips.items[0]->scriptLayer.update();
	m3ApiSuccess();
}

m3ApiRawFunction(m3_getFXSpeed)
{
	m3ApiReturnType(float);
	float v = 0; // RootComponent::instance->strips.items[0]->bakeLayer.offsetSpeed;
	m3ApiReturn((float)v);
}

m3ApiRawFunction(m3_getFXIsoSpeed)
{
	m3ApiReturnType(float);
	float v = 0; // RootComponent::instance->strips.items[0]->bakeLayer.isolationSpeed;
	m3ApiReturn((float)v);
}

m3ApiRawFunction(m3_getFXStaticOffset)
{
	m3ApiReturnType(float);
	float v = 0; // RootComponent::instance->strips.items[0]->bakeLayer.staticOffset;
	m3ApiReturn((float)v);
}

m3ApiRawFunction(m3_getFXFlipped)
{
	m3ApiReturnType(uint32_t);
	bool v = false; // RootComponent::instance->strips.items[0]->bakeLayer.boardIsFlipped;
	m3ApiReturn((uint32_t)v);
}

m3ApiRawFunction(m3_setFXSpeed)
{
	m3ApiGetArg(float, sp);
	LOG("Set FX speed: " + String(sp));
	// RootComponent::instance->strips.items[0]->bakeLayer.offsetSpeed = sp;
	m3ApiSuccess();
}

m3ApiRawFunction(m3_setFXIsoSpeed)
{
	m3ApiGetArg(float, sp);
	LOG("Set FX isolation speed: " + String(sp));
	// RootComponent::instance->strips.items[0]->bakeLayer.isolationSpeed = sp;
	m3ApiSuccess();
}

m3ApiRawFunction(m3_setFXIsoAxis)
{
	m3ApiGetArg(uint32_t, ax);
	LOG("Set FX isolation axis: " + String(ax));
	// RootComponent::instance->strips.items[0]->bakeLayer.isolationAxis = ax;
	m3ApiSuccess();
}

m3ApiRawFunction(m3_setFXStaticOffset)
{
	m3ApiGetArg(float, sp);
	LOG("Set FX static offset: " + String(sp));
	// RootComponent::instance->strips.items[0]->bakeLayer.staticOffset = sp;
	m3ApiSuccess();
}

m3ApiRawFunction(m3_resetFX)
{
	// RootComponent::instance->strips.items[0]->bakeLayer.reset();
	m3ApiSuccess();
}

m3ApiRawFunction(m3_setBatterySendEnabled)
{
	m3ApiGetArg(uint32_t, en);
	LOG("Set battery send enabled: " + String(en));
	//RootComponent::instance->battery.feedbackInterval = en ? 5 : 0;
	m3ApiSuccess();
}

m3ApiRawFunction(m3_playVariant)
{
	m3ApiGetArg(uint32_t, v);
	LOG("Play variant: " + String(v));
	//String name = RootComponent::instance->strips.items[0]->bakeLayer.curFile.name();
	//float time = RootComponent::instance->strips.items[0]->bakeLayer.curTimeMs;
	//uint32_t start = millis();
	//char l = name.charAt(name.length() - 1);

	//if (l >= '0' && l <= '9')
	//{
	//	name.remove(name.length() - 1);
	//}

	//name = name + String(v);

	//RootComponent::instance->strips.items[0]->bakeLayer.load(name);

	//time = (time + (millis() - start)) / 1000;
	//RootComponent::instance->strips.items[0]->bakeLayer.play(time);
	m3ApiSuccess();
}

m3ApiRawFunction(m3_getOrientation)
{
	m3ApiReturnType(float);
	m3ApiGetArg(uint32_t, oi);

	WasmEngine* engine = (WasmEngine*)m3_GetUserData(runtime);
	float v = -1;
	switch (oi)
	{
	case 0:
		v = engine->testOrientation->x;
		break;

	case 1:
		v = engine->testOrientation->y;
		break;

	case 2:
		v = engine->testOrientation->z;
		break;
	}

	m3ApiReturn(v);
}

m3ApiRawFunction(m3_getYaw)
{
	m3ApiReturnType(float);

	WasmEngine* engine = (WasmEngine*)m3_GetUserData(runtime);
	m3ApiReturn(engine->testOrientation->x);
}

m3ApiRawFunction(m3_getPitch)
{
	m3ApiReturnType(float);
	WasmEngine* engine = (WasmEngine*)m3_GetUserData(runtime);
	m3ApiReturn(engine->testOrientation->y);
}
m3ApiRawFunction(m3_getRoll)
{
	m3ApiReturnType(float);
	WasmEngine* engine = (WasmEngine*)m3_GetUserData(runtime);
	m3ApiReturn(engine->testOrientation->z);
}

m3ApiRawFunction(m3_getThrowState)
{
	m3ApiReturnType(uint32_t);
	m3ApiReturn(false);// (uint32_t)RootComponent::instance->motion.throwState);
}

m3ApiRawFunction(m3_getProjectedAngle)
{
	m3ApiReturnType(float);
	m3ApiReturn(0.f);
	//m3ApiReturn(RootComponent::instance->motion.projectedAngle);
}

m3ApiRawFunction(m3_setProjectedAngleOffset)
{
	m3ApiGetArg(float, yaw);
	m3ApiGetArg(float, angle);
	LOG("Set projected angle offset: yaw=" + String(yaw) + " angle=" + String(angle));
	// RootComponent::instance->motion.setProjectAngleOffset(yaw, angle);
	m3ApiSuccess();
}

m3ApiRawFunction(m3_calibrateIMU)
{
	// RootComponent::instance->motion.calibrate();
	m3ApiSuccess();
}

m3ApiRawFunction(m3_setIMUEnabled)
{
	m3ApiGetArg(uint32_t, en);
	LOG("Set IMU enabled: " + String(en));
	//RootComponent::instance->motion.setEnabledFromScript((bool)en);
	m3ApiSuccess();
}

m3ApiRawFunction(m3_getActivity)
{
	m3ApiReturnType(float);
	m3ApiReturn(0.f);
	//m3ApiReturn(RootComponent::instance->motion.activity);
}

m3ApiRawFunction(m3_getSpin)
{
	m3ApiReturnType(float);
	m3ApiReturn(0.f);
	//m3ApiReturn(RootComponent::instance->motion.spin);
}

m3ApiRawFunction(m3_getButtonState)
{
	m3ApiReturnType(uint32_t);

	m3ApiGetArg(uint32_t, btIndex);

	WasmEngine* engine = (WasmEngine*)m3_GetUserData(runtime);

	bool v = engine->testButton->boolValue();

	m3ApiReturn((uint32_t)v);
}

m3ApiRawFunction(m3_setMicEnabled)
{
	m3ApiGetArg(uint32_t, en);
	LOG("Set mic enabled: " + String(en));
	//MicManager::instance->setEnabled((bool)en);
	m3ApiSuccess();
}

m3ApiRawFunction(m3_getMicLevel)
{
	m3ApiReturnType(float);

	float v = 0.f;// MicManager::instance->enveloppe;
	m3ApiReturn((float)v);
}

m3ApiRawFunction(m3_getDistance)
{
	m3ApiReturnType(float);

	m3ApiGetArg(uint32_t, sensorIndex);

	WasmEngine* engine = (WasmEngine*)m3_GetUserData(runtime);
	float v = engine->testDistance->floatValue();
	//float v = sensorIndex < RootComponent::instance->distances.count ? RootComponent::instance->distances.items[sensorIndex]->value : -1.0f;

	m3ApiReturn((float)v);
}

m3ApiRawFunction(m3_randomInt)
{
	m3ApiReturnType(uint32_t);
	m3ApiGetArg(uint32_t, min);
	m3ApiGetArg(uint32_t, max);

	Random r;
	int v = r.nextInt(Range<int>(min, max));
	m3ApiReturn((uint32_t)v);
}

m3ApiRawFunction(m3_noise)
{
	m3ApiReturnType(float);
	m3ApiGetArg(float, x);
	m3ApiGetArg(float, y);

	WasmEngine* engine = (WasmEngine*)m3_GetUserData(runtime);
	float n = (float)engine->noise.noise(x, y);
	n = (n + 1) / 2; // convert to value range [0..1]

	m3ApiReturn(n);
}
