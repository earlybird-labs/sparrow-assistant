#ifndef TOUCHSLEEPHANDLER_H
#define TOUCHSLEEPHANDLER_H

class TouchSleepHandler
{
public:
    TouchSleepHandler();
    void handleTouch();
    bool getIsSleepMode() const;

private:
    void enterSleepMode();
    void exitSleepMode();
    bool isSleepMode;
};

#endif // TOUCHSLEEPHANDLER_H