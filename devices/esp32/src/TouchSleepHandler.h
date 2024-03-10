#ifndef TOUCHSLEEPHANDLER_H
#define TOUCHSLEEPHANDLER_H

class TouchSleepHandler
{
public:
    TouchSleepHandler(void (*onEnterSleep)(), void (*onExitSleep)());
    void handleTouch();
    bool getIsSleepMode() const;

private:
    bool isSleepMode;
    void (*onEnterSleepAction)();
    void (*onExitSleepAction)();
};

#endif // TOUCHSLEEPHANDLER_H