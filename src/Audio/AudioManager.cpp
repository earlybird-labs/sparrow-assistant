#include "AudioManager.h"
#include <Arduino.h>

AudioManager::AudioManager()
{
    // Constructor can be left empty or used for initial setup
}

void AudioManager::begin()
{
    initI2S();
    Serial.println("AudioManager initialized");
}

void AudioManager::initI2S()
{
    // Example I2S configuration for ESP32
    const i2s_config_t i2s_config = {
        .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 16000,                         // Sample rate
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // Bit depth
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,  // Mono audio
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = 0, // Default interrupt priority
        .dma_buf_count = 4,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0};

    const i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_PIN_NO_CHANGE,
        .ws_io_num = I2S_PIN_NO_CHANGE,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_PIN_NO_CHANGE};

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_set_clk(I2S_NUM_0, 16000, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
}

bool AudioManager::isRecording()
{
    return _isRecording; // Return the current recording state
}

void AudioManager::recordAudio()
{
    if (!isRecording())
    {
        Serial.println("Recording audio...");
        _isRecording = true;
        clearAudioData(); // Clear previous data

        size_t bytes_read;
        uint8_t i2s_read_buff[1024];
        while (isRecording())
        {
            // Read data from the I2S peripheral
            i2s_read(I2S_NUM_0, &i2s_read_buff, sizeof(i2s_read_buff), &bytes_read, portMAX_DELAY);
            // Append data to audioData vector
            audioData.insert(audioData.end(), i2s_read_buff, i2s_read_buff + bytes_read);
            // Here you might want to add a condition to break out of the loop
            // For example, checking a flag that gets set in stopRecording()
        }
    }
}

void AudioManager::stopRecording()
{
    if (isRecording())
    {
        Serial.println("Stopped recording.");
        _isRecording = false;
        // Placeholder for stopping the recording logic
        // Ensure you stop the loop that reads data from I2S
    }
}

void AudioManager::streamAudio()
{
    // Placeholder for streaming audio logic
    Serial.println("Streaming audio...");
    // This method would be used to stream audio data over BLE or another interface
}

std::vector<uint8_t> AudioManager::getAudioData()
{
    return audioData; // Return the recorded audio data
}

void AudioManager::clearAudioData()
{
    audioData.clear(); // Clear the audio buffer
}