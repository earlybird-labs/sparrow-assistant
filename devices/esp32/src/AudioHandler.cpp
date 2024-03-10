#include "AudioHandler.h"
#include "WebSocketHandler.h"

// Pin definitions for I2S communication
#define I2S_WS 8
#define I2S_SD 44
#define I2S_SCK 7

#define I2S_PORT I2S_NUM_0

// Buffer configurations for audio processing
#define bufferCnt 10
#define bufferLen 1024

/**
 * Constructor for the AudioHandler class.
 * Initializes the WebSocketHandler for sending messages and sets initial states.
 *
 * @param webSocketHandler A pointer to the WebSocketHandler for sending messages.
 */
AudioHandler::AudioHandler(WebSocketHandler *webSocketHandler) : webSocketHandler(webSocketHandler), isSpeaking(false), lastAboveThresholdTime(0), lastBelowThresholdTime(0), consecutiveAboveThreshold(0), consecutiveBelowThreshold(0) {}

/**
 * Returns the current speaking state.
 *
 * @return True if currently speaking, false otherwise.
 */
bool AudioHandler::getIsSpeaking() const
{
    return isSpeaking;
}

/**
 * Initializes the I2S hardware for audio input.
 * Configures the I2S with predefined settings for audio capture.
 */
void AudioHandler::begin()
{
    i2s_install();
    i2s_setpin();
    i2s_start(I2S_PORT);
}

/**
 * Installs and configures the I2S driver.
 * Sets up the I2S configuration for receiving audio data.
 */
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

/**
 * Configures the I2S pins for audio input.
 * Assigns the specific GPIO pins to the I2S peripheral for audio data capture.
 */
void AudioHandler::i2s_setpin()
{
    const i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = -1,
        .data_in_num = I2S_SD};

    i2s_set_pin(I2S_PORT, &pin_config);
}

/**
 * Reads audio data from the microphone into a buffer.
 * Processes the audio data to determine the sound intensity and updates the speaking state.
 *
 * @param buffer The buffer to store the audio data.
 * @param bufLen The length of the buffer.
 * @param bytesRead The number of bytes actually read into the buffer.
 */
void AudioHandler::readMic(int16_t *buffer, size_t bufLen, size_t &bytesRead)
{
    esp_err_t result = i2s_read(I2S_PORT, buffer, bufLen, &bytesRead, portMAX_DELAY);
    if (result != ESP_OK)
    {
        // Handle error
        printf("Error reading from I2S: %d\n", result);
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

/**
 * Updates the speaking state based on the average signal level.
 * Determines if the device should transition between speaking and not speaking states.
 *
 * @param average The average signal level calculated from the audio buffer.
 * @param threshold The threshold above which the device is considered to be speaking.
 */
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
    if (!isSpeaking && consecutiveAboveThreshold >= 5)
    {
        isSpeaking = true;
        webSocketHandler->sendText("START_SPEAKING"); // Send start speaking signal
        Serial.println("Transitioning to SPEAKING mode");
        consecutiveAboveThreshold = 0;
        consecutiveBelowThreshold = 0;
    }

    // Transition to PASSIVE mode with a delay
    // This modification introduces a delay before transitioning to PASSIVE mode,
    // allowing for brief pauses in speech without immediately stopping.
    const uint32_t speakingHoldTime = 3000; // Continue in SPEAKING mode for 3 seconds after signal drops
    if (isSpeaking && (currentTime - lastAboveThresholdTime > speakingHoldTime))
    {
        isSpeaking = false;
        webSocketHandler->sendText("STOP_SPEAKING"); // Send stop speaking signal
        Serial.println("Transitioning to PASSIVE mode");
        consecutiveAboveThreshold = 0;
        consecutiveBelowThreshold = 0;
    }
}