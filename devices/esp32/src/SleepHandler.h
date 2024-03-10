#ifndef SleepHandler_H
#define SleepHandler_H

#include "esp_sleep.h"
#include "driver/touch_pad.h"

class SleepHandler {
public:
    SleepHandler();
    void begin(int touchPin, uint64_t timeToSleepInSeconds);
    void goToDeepSleep();
    esp_sleep_wakeup_cause_t getWakeupReason();
};

#endif