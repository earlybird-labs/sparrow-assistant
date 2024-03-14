#include "SleepHandler.h"
#include "esp_sleep.h"
#include "TouchHandler.h" // Include TouchHandler to access the touch pin

extern TouchHandler touchHandler; // Ensure there's an external declaration for touchHandler

SleepHandler::SleepHandler() : sleepMode(false) {}

void SleepHandler::enterSleepMode()
{
    Serial.println("Entering sleep mode...");
    // Additional actions before toggling the state
    sleepMode = !sleepMode;
    if (sleepMode)
    {
        // Enable touchpad wakeup
        esp_sleep_enable_touchpad_wakeup();
        esp_deep_sleep_start(); // Enter deep sleep
    }
    // If exiting sleep mode, additional actions can be added here
}

void SleepHandler::exitSleepMode()
{
    if (sleepMode)
    {
        Serial.println("Exiting sleep mode...");
        // Additional sleep mode exit actions can be added here
        sleepMode = false;
    }
}

void SleepHandler::toggleSleepMode()
{
    if (!sleepMode)
    {
        enterSleepMode();
    }
    else
    {
        exitSleepMode();
    }
}

bool SleepHandler::isSleepMode() const
{
    return sleepMode;
}