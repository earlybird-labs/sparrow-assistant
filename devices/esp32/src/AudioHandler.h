#ifndef AUDIOHANDLER_H
#define AUDIOHANDLER_H

#include <Arduino.h>
#include <driver/i2s.h>
#include "WebSocketHandler.h"

class AudioHandler
{
public:
    AudioHandler(WebSocketHandler *webSocketHandler);
    void begin();
    void readMic(int16_t *buffer, size_t bufLen, size_t &bytesRead);
    bool getIsSpeaking() const;
    void getPreBufferData(int16_t **buffer, size_t *length); // Ensure this method is correctly declared

private:
    WebSocketHandler *webSocketHandler;

    static const int preBufferLen = 44100; // Adjust based on your needs, ensuring enough pre-buffer
    int16_t preBuffer[preBufferLen];
    int preBufferIndex = 0;
    bool preBufferFull = false;

    void i2s_install();
    void i2s_setpin();
    void updateSpeakingState(int16_t average, int16_t threshold);

    bool isSpeaking;
    uint32_t lastAboveThresholdTime;
    uint32_t lastBelowThresholdTime;
    uint32_t consecutiveAboveThreshold;
    uint32_t consecutiveBelowThreshold;
};

#endif // AUDIOHANDLER_H