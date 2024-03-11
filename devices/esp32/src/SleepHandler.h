#ifndef SLEEPHANDLER_H
#define SLEEPHANDLER_H

#include "Arduino.h"

class SleepHandler
{
public:
    SleepHandler();
    bool isSleepMode() const;
    void toggleSleepMode();
    void enterSleepMode();
    void exitSleepMode();

private:
    bool sleepMode;
};

#endif // SLEEPHANDLER_H