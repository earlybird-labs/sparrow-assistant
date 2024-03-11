#ifndef TOUCH_HANDLER_H
#define TOUCH_HANDLER_H

#include <Arduino.h>
#include <driver/touch_sensor.h>

typedef void (*TouchCallback)(bool isSleep);

class TouchHandler
{
public:
    TouchHandler();
    void start();
    void setTouchThreshold(uint16_t threshold); // Correctly added
    void onSubscribe(TouchCallback callback);

private:
    static void touchTask(void *parameter);
    void handleTouch();
    void onTouched(bool isSleep); // Updated to accept a bool parameter

    static const uint16_t DEFAULT_TOUCH_THRESHOLD = 50000;
    uint16_t TOUCH_THRESHOLD = DEFAULT_TOUCH_THRESHOLD;
    TouchCallback touchCallback = nullptr;
};

#endif // TOUCH_HANDLER_H