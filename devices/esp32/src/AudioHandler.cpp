#include "AudioHandler.h"
#include "WebSocketHandler.h"

#define I2S_WS 15
#define I2S_SD 13
#define I2S_SCK 2
#define I2S_PORT I2S_NUM_0

#define bufferCnt 10
#define bufferLen 1024

// Constructor initialization
// Add two new parameters to the constructor to allow sending messages
AudioHandler::AudioHandler(WebSocketHandler *webSocketHandler) : webSocketHandler(webSocketHandler), isSpeaking(false), lastAboveThresholdTime(0), lastBelowThresholdTime(0), consecutiveAboveThreshold(0), consecutiveBelowThreshold(0) {}

bool AudioHandler::getIsSpeaking() const
{
    return isSpeaking;
}

void AudioHandler::begin()
{
    i2s_install();
    i2s_setpin();
    i2s_start(I2S_PORT);
}

void AudioHandler::i2s_install()
{
    const i2s_config_t i2s_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 44100,
        .bits_per_sample = i2s_bits_per_sample_t(16),
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
        .intr_alloc_flags = 0,
        .dma_buf_count = bufferCnt,
        .dma_buf_len = bufferLen,
        .use_apll = false};

    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void AudioHandler::i2s_setpin()
{
    const i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = -1,
        .data_in_num = I2S_SD};

    i2s_set_pin(I2S_PORT, &pin_config);
}

void AudioHandler::readMic(int16_t *buffer, size_t bufLen, size_t &bytesRead)
{
    esp_err_t result = i2s_read(I2S_PORT, buffer, bufLen, &bytesRead, portMAX_DELAY);
    if (result != ESP_OK)
    {
        // Handle error
        return;
    }

    // Sound intensity filter logic
    const float gainFactor = 1.0;  // Example gain factor; adjust as needed
    const int16_t threshold = 100; // Define your threshold here
    int64_t sum = 0;
    for (int i = 0; i < bytesRead / sizeof(int16_t); i++)
    {
        int32_t temp = buffer[i] * gainFactor;
        temp = (temp > INT16_MAX) ? INT16_MAX : (temp < INT16_MIN) ? INT16_MIN
                                                                   : temp;
        buffer[i] = temp;
        sum += abs(temp);
    }
    int16_t average = sum / (bytesRead / sizeof(int16_t));

    // Update speaking state based on average signal level
    updateSpeakingState(average, threshold);
}

void AudioHandler::updateSpeakingState(int16_t average, int16_t threshold)
{
    uint32_t currentTime = millis();
    if (average > threshold)
    {
        lastAboveThresholdTime = currentTime;
        consecutiveAboveThreshold++;
        consecutiveBelowThreshold = 0;
    }
    else
    {
        lastBelowThresholdTime = currentTime;
        consecutiveBelowThreshold++;
        consecutiveAboveThreshold = 0;
    }

    // Transition to SPEAKING mode
    // Modify the transitions to send messages
    if (!isSpeaking && consecutiveAboveThreshold >= 5)
    {
        isSpeaking = true;
        webSocketHandler->sendText("START_SPEAKING"); // Send start speaking signal
        Serial.println("Transitioning to SPEAKING mode");
        consecutiveAboveThreshold = 0;
        consecutiveBelowThreshold = 0;
    }

    if (isSpeaking && consecutiveBelowThreshold >= 100)
    {
        isSpeaking = false;
        webSocketHandler->sendText("STOP_SPEAKING"); // Send stop speaking signal
        Serial.println("Transitioning to PASSIVE mode");
        consecutiveAboveThreshold = 0;
        consecutiveBelowThreshold = 0;
    }
}