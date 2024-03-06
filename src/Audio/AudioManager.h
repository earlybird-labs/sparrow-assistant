#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <driver/i2s.h>
#include <vector>

// Define the GPIO pins for I2S interface based on the wiring information
#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_26
#define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_22
#define I2S_MIC_SERIAL_DATA GPIO_NUM_21

class AudioManager
{
public:
    AudioManager();
    void begin();
    void recordAudio();
    void stopRecording();
    void streamAudio();
    std::vector<uint8_t> getAudioData(); // Method to get recorded audio data
    bool isRecording();                  // Method to check if audio is currently being recorded

private:
    bool _isRecording = false;
    std::vector<uint8_t> audioData; // Buffer to store audio data
    void initI2S();
    void clearAudioData(); // Helper method to clear the audio buffer
};

#endif // AUDIOMANAGER_H