#include "UnityIncludes.h"
#include "RootComponent.h"

ImplementSingleton(RootComponent);

bool RootComponent::availablePWMChannels[16] = {true};

bool RootComponent::initInternal(JsonObject)
{
    BoardInit;

    exposeEnabled = false;
    saveEnabled = false;

    timeAtStart = millis();
    timeAtShutdown = 0;

    // parameters.clear(); // remove enabled in root component
    Settings::loadSettings();
    JsonObject o = Settings::settings.as<JsonObject>();

    AddOwnedComponent(&comm);
    AddOwnedComponent(&settings);

#ifdef USE_DISPLAY
    AddOwnedComponent(&display);
#endif

#ifdef USE_LEDSTRIP
    AddOwnedComponent(&strips);
#endif

#ifdef USE_BATTERY
    AddOwnedComponent(&battery);
#endif

#ifdef USE_FILES
    AddOwnedComponent(&files);
#endif

#if USE_SCRIPT
    AddOwnedComponent(&script);
#endif

#ifdef USE_SERVER
    AddOwnedComponent(&server);
#endif

#ifdef USE_STREAMING
    AddOwnedComponent(&streamReceiver);
#endif

#ifdef USE_IO
    memset(availablePWMChannels, true, sizeof(availablePWMChannels));
    AddOwnedComponent(&ios);
#ifdef USE_BUTTON
    AddOwnedComponent(&buttons);
#endif
#endif

#if USE_MOTION
    AddOwnedComponent(&motion);
#endif

#if USE_SERVO
    AddOwnedComponent(&servo);
#endif

#if USE_STEPPER
    AddOwnedComponent(&stepper);
#endif

#if USE_DC_MOTOR
    AddOwnedComponent(&motor);
#endif

#ifdef USE_BEHAVIOUR
    AddOwnedComponent(&behaviours);
#endif

#ifdef USE_DUMMY
    AddOwnedComponent(&dummies);
#endif

#ifdef USE_SEQUENCE
    AddOwnedComponent(&sequence);
#endif

#ifdef USE_WIFI
    AddOwnedComponent(&wifi);
#endif

    return true;
}

void RootComponent::updateInternal()
{
    timer.tick();
}

void RootComponent::restart()
{
    clear();
    delay(500);
    ESP.restart();
}

void RootComponent::shutdown()
{
    NDBG("Sleep now, baby.");
    timeAtShutdown = millis();
    timer.in(1000, [](void *) -> bool
             {  RootComponent::instance->powerdown(); return false; });
}

void RootComponent::powerdown()
{
    clear();

    // NDBG("Sleep now, baby.");

    delay(500);

#ifdef USE_POWER
    if (settings.wakeUpButton > 0)
        esp_sleep_enable_ext0_wakeup((gpio_num_t)settings.wakeUpButton, settings.wakeUpState);
        // #elif defined TOUCH_WAKEUP_PIN
        //     touchAttachInterrupt((gpio_num_t)TOUCH_WAKEUP_PIN, touchCallback, 110);
        //     esp_sleep_enable_touchpad_wakeup();
        // #endif
#endif

#ifdef ESP8266
    ESP.deepSleep(5e6);
#else
    esp_deep_sleep_start();
#endif
}

void RootComponent::onChildComponentEvent(const ComponentEvent &e)
{
    if (isShuttingDown())
        return;

    if (e.component == &comm
#ifdef USE_BEHAVIOUR
        || e.component == &behaviours
#endif
    )
    {
        if ((e.component == &comm && e.type == CommunicationComponent::MessageReceived)
#ifdef USE_BEHAVIOUR
            || (e.component == &behaviours && e.type == BehaviourManagerComponent::CommandLaunched)
#endif
        )
        {
            if (Component *targetComponent = getComponentWithName(e.data[0].stringValue()))
            {
                bool handled = targetComponent->handleCommand(e.data[1], &e.data[2], e.numData - 2);
                if (!handled)
                    NDBG("Command was not handled " + e.data[0].stringValue() + " > " + e.data[1].stringValue());
            }
            else
            {
                NDBG("No component found for " + e.data[0].stringValue());
            }

            return;
        }
    }
#ifdef USE_WIFI
    else if (e.component == &wifi)
    {
        if (e.type == WifiComponent::ConnectionStateChanged)
        {
            if (wifi.state == WifiComponent::Connected)
            {
                NDBG("Setup connections now");
                server.setupConnection();

#if USE_STREAMING
                streamReceiver.setupConnection();
#endif

#if USE_OSC
                comm.osc.setupConnection();
#endif
            }
        }
    }
#endif

#if USE_BATTERY
    else if (e.component == &battery)
    {
        if (e.type == BatteryComponent::CriticalBattery)
        {
#if USE_LEDSTRIP
            strips.items[0]->setBrightness(.05f);
#endif
            shutdown();
        }
    }
#endif

    comm.sendEventFeedback(e);
}

void RootComponent::childParamValueChanged(Component *caller, Component *comp, void *param)
{
#ifdef USE_BUTTON
    if (caller == &buttons)
    {
        ButtonComponent *bc = (ButtonComponent *)comp;
        // DBG("Root param value changed " + bc->name+" > "+String(param == &bc->veryLongPress) + " / " + String(bc->veryLongPress)+" can sd : "+String(bc->canShutDown)+" / "+String(buttons.items[0]->canShutDown));
        if (param == &bc->veryLongPress && bc->veryLongPress && bc->canShutDown)
        {
            NDBG("Shutdown from button");
            shutdown();
        }
    }
#endif
}

bool RootComponent::handleCommandInternal(const String &command, var *data, int numData)
{
    if (command == "shutdown")
        shutdown();
    else if (command == "restart")
        restart();
    else if (command == "stats")
    {
        DBG("Heap " + String(ESP.getFreeHeap()) + " free / " + String(ESP.getHeapSize()) + " total");
        DBG("Free Stack size  " + String((int)uxTaskGetStackHighWaterMark(NULL)) + " free");
        // comm->sendMessage(this, "freeHeap", String(ESP.getFreeHeap()) + " bytes");
    }
    else
    {
        return false;
    }

    return true;
}

int RootComponent::getFirstAvailablePWMChannel() const
{
    for (int i = 0; i < 16; i++)
    {
        if (availablePWMChannels[i])
            return i;
    }
    return -1;
}
