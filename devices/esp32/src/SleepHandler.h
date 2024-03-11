#ifndef SLEEPHANDLER_H
#define SLEEPHANDLER_H

#include "Arduino.h"

class SleepHandler
{
public:
    SleepHandler();
    bool isSleepMode() const;
    void enterSleepMode();
    void exitSleepMode();

private:
    bool sleepMode;
    void toggleSleepMode();
};

#endif // SLEEPHANDLER_H