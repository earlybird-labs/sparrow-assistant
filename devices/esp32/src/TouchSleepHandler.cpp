#include "TouchSleepHandler.h"
#include <Arduino.h>

// Assuming GPIO 33 for the touch sensor output, adjust as needed
#define TOUCH_PIN 4
#define LED_PIN 43

/**
 * Constructor for the TouchSleepHandler class.
 * Initializes the state and sets up the touch sensor pin.
 */
TouchSleepHandler::TouchSleepHandler() : isSleepMode(true) // Default to sleep mode
{
    pinMode(TOUCH_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);   // Initialize the LED pin as an output
    digitalWrite(LED_PIN, LOW); // Start with the LED off, assuming sleep mode is default
}

/**
 * Monitors the touch sensor and updates the sleep mode state accordingly.
 */
void TouchSleepHandler::handleTouch()
{
    // Read the state of the touch sensor
    bool touchState = digitalRead(TOUCH_PIN);

    // If the sensor is touched, toggle the sleep mode state
    if (touchState == HIGH)
    { // Assuming HIGH means touched
        isSleepMode = !isSleepMode;
        if (isSleepMode)
        {
            enterSleepMode();
        }
        else
        {
            exitSleepMode();
        }
    }
}

/**
 * Enters sleep mode.
 * Implement actions to reduce power consumption and prepare for sleep.
 */
void TouchSleepHandler::enterSleepMode()
{
    Serial.println("Entering sleep mode");
    digitalWrite(LED_PIN, LOW); // Turn off LED to indicate sleep mode

    // Implement actions to enter sleep mode, e.g., disable sensors, LEDs, etc.
}

/**
 * Exits sleep mode.
 * Implement actions to resume normal operation.
 */
void TouchSleepHandler::exitSleepMode()
{
    Serial.println("Exiting sleep mode");
    digitalWrite(LED_PIN, HIGH); // Turn on LED to indicate normal operation

    // Implement actions to exit sleep mode and resume normal operation
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