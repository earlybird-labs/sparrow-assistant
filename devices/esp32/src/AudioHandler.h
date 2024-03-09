#ifndef AUDIOHANDLER_H
#define AUDIOHANDLER_H

#include <Arduino.h>
#include <driver/i2s.h>

class AudioHandler
{
public:
    AudioHandler();
    void begin();
    void readMic(int16_t *buffer, size_t bufLen, size_t &bytesRead);

private:
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