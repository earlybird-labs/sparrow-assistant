#ifndef TOUCHHANDLER_H
#define TOUCHHANDLER_H

#include <Arduino.h>
#include <esp32-hal-touch.h>

class TouchHandler
{
public:
    TouchHandler(uint8_t touchPin, void (*touchCallback)());
    void begin();
    bool checkTouch(); // Add this line to declare the checkTouch method

    uint8_t getTouchPin() const;
    uint16_t getTouchThreshold() const;

    static TouchHandler *instance; // Singleton instance
    static bool isSleepMode;       // Indicates if the system is in sleep mode
    static int baselineTouchValue; // Static member to hold the baseline touch value
    static volatile bool isrFlag;  // Flag set by the ISR (not used in the simplified version but kept for compatibility)
    static int touchMode;          // 0 for sleep, 1 for wake - Add this line

private:
    uint8_t touchPin; // Keep this non-static
    void (*touchCallback)(bool);
};

#endif