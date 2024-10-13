#include "DisplayComponent.h"

bool DisplayComponent::initInternal(JsonObject o)
{
#if DISPLAY_TYPE == M5StickC
    initM5StickC();
#endif

    return true;
}

void DisplayComponent::updateInternal()
{
    if (shouldUpdateDisplay)
    {
        canvas.pushSprite(0, 0);
        shouldUpdateDisplay = false;
    }
}

void DisplayComponent::clearInternal()
{
    clearDisplay();
}

#if DISPLAY_TYPE == M5StickC
void DisplayComponent::initM5StickC()
{
    display.begin();

    if (display.isEPD())
    {
        display.setEpdMode(epd_mode_t::epd_fastest);
        display.invertDisplay(true);
        display.clear(TFT_BLACK);
    }
    if (display.width() < display.height())
    {
        NDBG("Rotate display");
        display.setRotation(display.getRotation() ^ 3);
    }

    canvas.setColorDepth(1); // mono color
    canvas.createSprite(display.width(), display.height());
    canvas.setTextSize((float)canvas.width() / 160);
    canvas.setTextScroll(true);
}
#endif

void DisplayComponent::clearDisplay()
{
#if DISPLAY_TYPE == M5StickC
    canvas.clear(TFT_BLACK);
    display.clear();
#endif
}

bool DisplayComponent::handleCommandInternal(const String &command, var *data, int numData)
{
    if (command == "log")
    {
        log(data[0].s);
        return true;
    }

    return false;
}

void DisplayComponent::log(const String &text)
{
    canvas.printf("%s\r\n", text.c_str());
    shouldUpdateDisplay = true;
}

void DisplayComponent::logWarning(const String &text)
{
    log("Warning: " + text);
}

void DisplayComponent::logError(const String &text)
{
    log("Error: " + text);
}