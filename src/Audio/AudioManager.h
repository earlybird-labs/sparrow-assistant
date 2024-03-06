#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <driver/i2s.h>
#include <vector>

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