#include "TouchHandler.h"

// Initialize the touch pad peripheral.
TouchHandler::TouchHandler()
{
    touch_pad_init();
}

// Task to handle touch input.
void TouchHandler::touchTask(void *parameter)
{
    TouchHandler *touchHandler = static_cast<TouchHandler *>(parameter);
    touchHandler->handleTouch();
}

// Start the touch handling task.
void TouchHandler::start()
{
    xTaskCreate(&TouchHandler::touchTask, "touch_task", 2048, this, 5, NULL);
}

// Subscribe to touch events with a callback.
void TouchHandler::onSubscribe(TouchCallback callback)
{
    touchCallback = callback;
}

// Private method to handle touch logic.
void TouchHandler::handleTouch()
{
    uint16_t touch_value;
    while (1)
    {
        touch_value = touchRead(T3);
        printf("Touch value: %d\n", touch_value);
        if (touch_value < DEFAULT_TOUCH_THRESHOLD)
        {
            onTouched(true); // True for SLEEP
        }
        else
        {
            onTouched(false); // False for AWAKE
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

// Notify subscribed callback about the touch event.
void TouchHandler::onTouched(bool isSleep)
{
    if (touchCallback != nullptr)
    {
        touchCallback(isSleep);
    }
}