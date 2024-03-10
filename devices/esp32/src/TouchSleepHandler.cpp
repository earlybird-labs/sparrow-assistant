#include "TouchSleepHandler.h"
#include <Arduino.h>

// Assuming GPIO 33 for the touch sensor output, adjust as needed
#define TOUCH_PIN 4
#define LED_PIN 43

/**
 * Constructor for the TouchSleepHandler class.
 * Initializes the state and sets up the touch sensor pin.
 */
TouchSleepHandler::TouchSleepHandler(void (*onEnterSleep)(), void (*onExitSleep)()) : isSleepMode(false), onEnterSleepAction(onEnterSleep), onExitSleepAction(onExitSleep)
{
    pinMode(TOUCH_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW); // Assume LOW means LED off, indicating sleep mode
}

/**
 * Monitors the touch sensor and updates the sleep mode state accordingly.
 */
void TouchSleepHandler::handleTouch()
{
    bool touchState = digitalRead(TOUCH_PIN);
    if (touchState == HIGH)
    {
        isSleepMode = !isSleepMode;
        digitalWrite(LED_PIN, isSleepMode ? LOW : HIGH); // Reflect sleep state with LED
        if (isSleepMode)
        {
            if (onEnterSleepAction)
                onEnterSleepAction();
        }
        else
        {
            if (onExitSleepAction)
                onExitSleepAction();
        }
    }
}

/**
 * Returns the current sleep mode state.
 *
 * @return True if in sleep mode, false otherwise.
 */
bool TouchSleepHandler::getIsSleepMode() const
{
    return isSleepMode;
}