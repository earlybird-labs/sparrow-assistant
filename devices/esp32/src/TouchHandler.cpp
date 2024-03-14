#include "TouchHandler.h"

int TouchHandler::baselineTouchValue = 0;
volatile bool TouchHandler::isrFlag = false;
int TouchHandler::touchMode = 0;

TouchHandler *TouchHandler::instance = nullptr;

TouchHandler::TouchHandler(uint8_t touchPin, void (*touchCallback)())
    : touchPin(touchPin), touchCallback()
{
    TouchHandler::instance = this;
}

void TouchHandler::begin()
{
    baselineTouchValue = touchRead(touchPin); // Set the initial baseline touch value
    printf("Baseline touch value: %d, Mode: %d \n", baselineTouchValue, touchMode);
}

bool TouchHandler::checkTouch()
{
    if (!instance)
        return false; // Return false if instance is nullptr

    int currentTouchValue = touchRead(instance->touchPin);
    float changePercent = abs(currentTouchValue - instance->baselineTouchValue) / (float)instance->baselineTouchValue * 100.0;
    const float sensitivityThreshold = 20.0;

    if (changePercent > sensitivityThreshold)
    {
        instance->touchMode = !instance->touchMode; // Toggle mode
        // Update baselineTouchValue for next comparison if waking up
        if (instance->touchMode == 1)
        {
            instance->baselineTouchValue = currentTouchValue;
        }
        return true; // Return true because a mode change occurred
    }
    return false; // Return false if no mode change occurred
}

uint8_t TouchHandler::getTouchPin() const
{
    return touchPin;
}

uint16_t TouchHandler::getTouchThreshold() const
{
    // Define a suitable threshold value based on your application's needs
    const uint16_t threshold = 40; // Example threshold value
    return threshold;
}